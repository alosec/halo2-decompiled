/* The WMA music decoder behind the guest's in-memory WMA decoder object
 * (port/runtime/guest_dsound_runtime.cpp). Each sound chunk the title streams
 * is a whole ASF file; the guest hands it over at once and pulls 16-bit
 * 44.1 kHz stereo, the format it reports to the title. Decoding uses FFmpeg's
 * wmav1/wmav2 decoder when the build found FFmpeg (H2_HAVE_FFMPEG); without
 * it every open fails and the guest falls back to silence. */

#include "h2/host_abi.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#ifdef H2_HAVE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mem.h>
}
#endif

namespace {

constexpr unsigned int k_output_rate = 44100;

#ifdef H2_HAVE_FFMPEG

struct wma_stream {
    std::vector<std::uint8_t> data;
    std::size_t position = 0;
    AVFormatContext* format = nullptr;
    AVIOContext* io = nullptr;
    AVCodecContext* codec = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    int stream_index = -1;
    bool draining = false;
    bool finished = false;
    /* decoded stereo samples not yet handed out, and the resampler's place */
    std::vector<std::int16_t> pending;
    std::size_t pending_offset = 0;
    double phase = 0.0;
    float last[2] = {};

    ~wma_stream() {
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codec);
        avformat_close_input(&format);
        if (io) {
            av_freep(&io->buffer);
            avio_context_free(&io);
        }
    }
};

int read_memory(void* opaque, std::uint8_t* buffer, int size) {
    auto* stream = static_cast<wma_stream*>(opaque);
    const std::size_t left = stream->data.size() - stream->position;
    if (!left)
        return AVERROR_EOF;
    const std::size_t count = left < static_cast<std::size_t>(size) ? left : static_cast<std::size_t>(size);
    std::memcpy(buffer, stream->data.data() + stream->position, count);
    stream->position += count;
    return static_cast<int>(count);
}

std::int64_t seek_memory(void* opaque, std::int64_t offset, int whence) {
    auto* stream = static_cast<wma_stream*>(opaque);
    const auto size = static_cast<std::int64_t>(stream->data.size());
    if (whence == AVSEEK_SIZE)
        return size;
    std::int64_t target;
    switch (whence & ~AVSEEK_FORCE) {
    case SEEK_SET: target = offset; break;
    case SEEK_CUR: target = static_cast<std::int64_t>(stream->position) + offset; break;
    case SEEK_END: target = size + offset; break;
    default: return -1;
    }
    if (target < 0 || target > size)
        return -1;
    stream->position = static_cast<std::size_t>(target);
    return target;
}

float sample_at(const AVFrame* frame, int channel, int index) {
    const auto format = static_cast<AVSampleFormat>(frame->format);
    const int channels = frame->ch_layout.nb_channels;
    const bool planar = av_sample_fmt_is_planar(format);
    const std::uint8_t* base = planar ? frame->extended_data[channel] : frame->extended_data[0];
    const int at = planar ? index : index * channels + channel;
    switch (av_get_packed_sample_fmt(format)) {
    case AV_SAMPLE_FMT_FLT: return reinterpret_cast<const float*>(base)[at];
    case AV_SAMPLE_FMT_S16: return reinterpret_cast<const std::int16_t*>(base)[at] / 32768.0f;
    case AV_SAMPLE_FMT_S32: return static_cast<float>(reinterpret_cast<const std::int32_t*>(base)[at] / 2147483648.0);
    case AV_SAMPLE_FMT_DBL: return static_cast<float>(reinterpret_cast<const double*>(base)[at]);
    default: return 0.0f;
    }
}

std::int16_t to_s16(float value) {
    value *= 32767.0f;
    if (value > 32767.0f) value = 32767.0f;
    if (value < -32768.0f) value = -32768.0f;
    return static_cast<std::int16_t>(value);
}

/* appends a decoded frame as 44.1 kHz stereo (linear resampling; mono is
 * duplicated, channels past the second are dropped) */
void append_frame(wma_stream& stream, const AVFrame* frame) {
    const int channels = frame->ch_layout.nb_channels;
    if (channels < 1 || frame->nb_samples < 1 || frame->sample_rate < 1)
        return;
    const double step = static_cast<double>(frame->sample_rate) / k_output_rate;
    for (int i = 0; i < frame->nb_samples; i++) {
        float current[2];
        current[0] = sample_at(frame, 0, i);
        current[1] = channels > 1 ? sample_at(frame, 1, i) : current[0];
        /* emit every output sample that falls between the previous input
           sample and this one */
        while (stream.phase < 1.0) {
            const auto t = static_cast<float>(stream.phase);
            stream.pending.push_back(to_s16(stream.last[0] + (current[0] - stream.last[0]) * t));
            stream.pending.push_back(to_s16(stream.last[1] + (current[1] - stream.last[1]) * t));
            stream.phase += step;
        }
        stream.phase -= 1.0;
        stream.last[0] = current[0];
        stream.last[1] = current[1];
    }
}

/* decodes until there is output or the stream ends */
bool decode_more(wma_stream& stream) {
    while (!stream.finished) {
        const int received = avcodec_receive_frame(stream.codec, stream.frame);
        if (received == 0) {
            append_frame(stream, stream.frame);
            av_frame_unref(stream.frame);
            return true;
        }
        if (received == AVERROR_EOF) {
            stream.finished = true;
            break;
        }
        if (received != AVERROR(EAGAIN))
            return false;
        if (stream.draining) {
            stream.finished = true;
            break;
        }
        const int read = av_read_frame(stream.format, stream.packet);
        if (read < 0) {
            stream.draining = true;
            avcodec_send_packet(stream.codec, nullptr);
            continue;
        }
        if (stream.packet->stream_index == stream.stream_index)
            avcodec_send_packet(stream.codec, stream.packet);  /* a bad packet is skipped */
        av_packet_unref(stream.packet);
    }
    return false;
}

std::unique_ptr<wma_stream> open_stream(const void* data, unsigned int size) {
    auto stream = std::make_unique<wma_stream>();
    stream->data.assign(static_cast<const std::uint8_t*>(data), static_cast<const std::uint8_t*>(data) + size);
    constexpr int k_io_size = 16384;
    auto* buffer = static_cast<unsigned char*>(av_malloc(k_io_size));
    if (!buffer)
        return nullptr;
    stream->io = avio_alloc_context(buffer, k_io_size, 0, stream.get(), read_memory, nullptr, seek_memory);
    stream->format = avformat_alloc_context();
    if (!stream->io || !stream->format) {
        if (!stream->io)
            av_free(buffer);
        return nullptr;
    }
    stream->format->pb = stream->io;
    if (avformat_open_input(&stream->format, nullptr, av_find_input_format("asf"), nullptr) < 0)
        return nullptr;  /* the context is freed on failure */
    if (avformat_find_stream_info(stream->format, nullptr) < 0)
        return nullptr;
    const AVCodec* decoder = nullptr;
    stream->stream_index = av_find_best_stream(stream->format, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);
    if (stream->stream_index < 0 || !decoder)
        return nullptr;
    stream->codec = avcodec_alloc_context3(decoder);
    if (!stream->codec ||
        avcodec_parameters_to_context(stream->codec, stream->format->streams[stream->stream_index]->codecpar) < 0 ||
        avcodec_open2(stream->codec, decoder, nullptr) < 0)
        return nullptr;
    stream->packet = av_packet_alloc();
    stream->frame = av_frame_alloc();
    if (!stream->packet || !stream->frame)
        return nullptr;
    return stream;
}

#else

struct wma_stream {};

#endif

struct wma_state {
    std::mutex mutex;
    std::map<unsigned int, std::unique_ptr<wma_stream>> streams;
    unsigned int next_handle = 1;
    bool reported = false;
};

wma_state& state() {
    static wma_state instance;
    return instance;
}

}  // namespace

extern "C" unsigned int host_h2_wma_open(const void* data, unsigned int size) {
    if (!data || !size)
        return 0;
#ifdef H2_HAVE_FFMPEG
    av_log_set_level(AV_LOG_ERROR);
    auto stream = open_stream(data, size);
    auto& s = state();
    const std::lock_guard lock(s.mutex);
    if (!stream) {
        if (!s.reported)
            std::cerr << "h2-port: a WMA sound did not open; it plays silent\n";
        s.reported = true;
        return 0;
    }
    const unsigned int handle = s.next_handle++;
    if (!s.next_handle)
        s.next_handle = 1;
    s.streams[handle] = std::move(stream);
    return handle;
#else
    auto& s = state();
    const std::lock_guard lock(s.mutex);
    if (!s.reported)
        std::cerr << "h2-port: built without FFmpeg; WMA music plays silent\n";
    s.reported = true;
    return 0;
#endif
}

extern "C" unsigned int host_h2_wma_decode(unsigned int handle, void* pcm, unsigned int size) {
#ifdef H2_HAVE_FFMPEG
    auto& s = state();
    wma_stream* stream;
    {
        const std::lock_guard lock(s.mutex);
        const auto found = s.streams.find(handle);
        if (found == s.streams.end() || !pcm)
            return 0;
        stream = found->second.get();
    }
    auto* out = static_cast<std::int16_t*>(pcm);
    const std::size_t wanted = size / 4 * 2;  /* whole stereo frames */
    std::size_t produced = 0;
    while (produced < wanted) {
        if (stream->pending_offset == stream->pending.size()) {
            stream->pending.clear();
            stream->pending_offset = 0;
            if (!decode_more(*stream) && stream->pending.empty())
                break;
            continue;
        }
        std::size_t take = stream->pending.size() - stream->pending_offset;
        if (take > wanted - produced)
            take = wanted - produced;
        std::memcpy(out + produced, stream->pending.data() + stream->pending_offset, take * 2);
        stream->pending_offset += take;
        produced += take;
    }
    return static_cast<unsigned int>(produced * 2);
#else
    (void)handle;
    (void)pcm;
    (void)size;
    return 0;
#endif
}

extern "C" void host_h2_wma_close(unsigned int handle) {
    auto& s = state();
    std::unique_ptr<wma_stream> stream;
    {
        const std::lock_guard lock(s.mutex);
        const auto found = s.streams.find(handle);
        if (found == s.streams.end())
            return;
        stream = std::move(found->second);
        s.streams.erase(found);
    }
}
