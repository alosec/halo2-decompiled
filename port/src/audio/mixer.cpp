/* The host mixer; see mixer.hpp and h2/audio_protocol.h.
 *
 * Levels and 3D follow the public DirectSound model: millibel volumes
 * (gain = 10^(mB / 2000)), and the DirectSound 3D inverse-distance rolloff
 * gain = min / (min + rolloff * (distance - min)), distance clamped to
 * [min, max]. Panning is a constant-power law on the source's direction
 * projected on the listener's right axis (top x front in DirectSound's
 * left-handed frame), normalised so a centred source keeps full level in both
 * channels (the game's own mix-bin levels already carry its panning).
 *
 * Mix bins (Xbox numbering) fold to stereo as
 *   left  = 0 + 4 + 6 + 8 + 0.707 * 2      right = 1 + 5 + 7 + 9 + 0.707 * 2
 * with LFE (3) and the I3DL2 reverb send (10) dropped. A voice with several
 * channels assigns its mix-bin entries to channels in turn (entry i takes
 * channel i % channels), which is how the game builds stereo sends (it
 * appends a bin twice for a stereo source, src/unknown_2ae170.cpp
 * c_sound_effects::add_send). Effect sends (11..31) are mono rows that submix
 * buses with that input bin read, standing in for the DSP effect as a dry
 * pass-through. */
#include "audio/mixer.hpp"
#include <cstdlib>
#include <cstdio>

#include "audio/xbox_adpcm.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace h2::audio {
namespace {

constexpr std::size_t k_block = 256;
constexpr std::uint32_t k_unit_frames = 64;   /* PCM frames decoded per refill */
constexpr std::uint32_t k_max_unit_bytes = 64 * 2 * 2 * 2;
constexpr float k_jump = -1.0f;               /* no gain ramp from the last value */

float length3(const float v[3]) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

}  // namespace

float millibels_to_gain(std::int32_t millibels) {
    if (millibels <= -9600)
        return 0.0f;
    if (millibels == 0)
        return 1.0f;
    return std::pow(10.0f, static_cast<float>(millibels) / 2000.0f);
}

/* H2_TRACE=audio-voices: each voice's life (frames rendered, loudest
   output) at its release */
bool trace_voices() {
    static const bool on = [] {
        const char* trace = std::getenv("H2_TRACE");
        return trace && std::strstr(trace, "audio-voices");
    }();
    return on;
}

struct Mixer::Voice {
    std::uint32_t id = 0;
    std::uint64_t traced_frames = 0;
    float traced_peak = 0.0f;
    float traced_source_peak = 0.0f;
    std::uint32_t traced_unreadable = 0;
    std::uint32_t traced_zero_submit = 0, traced_zero_play = 0, traced_packets = 0;
    std::uint32_t flags = 0;
    h2_audio_format format{};
    std::uint32_t input_mixbin = 0;
    bool bus = false;
    bool spatial = false;
    bool stream = false;
    /* the decoding unit: bytes in, frames out (0 frames: unsupported format,
       played as silence at the format's byte rate) */
    std::uint32_t unit_bytes = 0;
    std::uint32_t unit_frames = 0;
    bool decodable = false;

    /* buffer */
    const std::uint8_t* data = nullptr;
    std::uint32_t data_size = 0;
    std::uint32_t cursor = 0;
    std::uint32_t loop_start = 0;
    std::uint32_t loop_size = 0;
    bool playing = false;
    bool looping = false;
    bool paused = false;

    /* stream */
    struct Packet {
        const std::uint8_t* data;
        std::uint32_t size;
    };
    std::deque<Packet> packets;
    std::uint32_t packet_offset = 0;
    std::uint32_t submitted = 0;
    std::uint32_t completed = 0;

    /* levels */
    std::int32_t volume = 0;
    std::int32_t headroom = 0;
    float frequency = 0.0f;
    std::uint32_t mixbin_count = 0;
    h2_audio_mixbin mixbins[H2_AUDIO_MAX_MIXBINS]{};
    float last_gain[H2_AUDIO_MAX_MIXBINS]{};
    float last_bus_gain = k_jump;
    std::uint32_t output = 0;
    float position[3] = {0, 0, 0};
    float min_distance = 1.0f;
    float max_distance = 1e9f;
    float rolloff = 1.0f;

    /* decoding and resampling */
    std::uint8_t stage[k_max_unit_bytes]{};
    std::uint32_t stage_size = 0;
    float decoded[k_unit_frames * 2]{};
    std::uint32_t decoded_count = 0;
    std::uint32_t decoded_index = 0;
    float s0[2] = {0, 0};
    float s1[2] = {0, 0};
    double phase = 0.0;
    bool primed = false;
    std::uint64_t frames_played = 0;

    /* a bus's input for the block being rendered (mono) */
    std::vector<float> bus_input;

    unsigned channels() const {
        if (bus)
            return 1;
        return format.channels == 2 ? 2 : 1;
    }

    void reset_gains() {
        for (float& gain : last_gain)
            gain = k_jump;
        last_bus_gain = k_jump;
    }

    void reset_decoder() {
        stage_size = 0;
        decoded_count = decoded_index = 0;
        s0[0] = s0[1] = s1[0] = s1[1] = 0.0f;
        phase = 0.0;
        primed = false;
    }
};

Mixer::Mixer(MemoryResolver resolver)
    : resolver_(std::move(resolver)),
      bins_(H2_AUDIO_MIXBIN_COUNT * k_block),
      scratch_(k_block * 2) {
    mixbin_gain_.fill(1.0f);
    record_open();
}

Mixer::~Mixer() {
    if (record_) {
        record_patch();
        std::fclose(record_);
    }
}

/* H2_AUDIO_RECORD=path.wav: everything mixed, as 32-bit float stereo at
   k_rate, playing or silent */
void Mixer::record_open() {
    const char* path = std::getenv("H2_AUDIO_RECORD");
    if (!path || !*path)
        return;
    record_ = std::fopen(path, "wb");
    if (!record_)
        return;
    const std::uint8_t header[44] = {};
    std::fwrite(header, 1, sizeof(header), record_);
    record_patch();
}

void Mixer::record_patch() {
    const auto put32 = [](std::uint8_t* p, std::uint32_t v) {
        for (int i = 0; i < 4; ++i) p[i] = static_cast<std::uint8_t>(v >> (i * 8));
    };
    const auto put16 = [](std::uint8_t* p, std::uint16_t v) {
        p[0] = static_cast<std::uint8_t>(v);
        p[1] = static_cast<std::uint8_t>(v >> 8);
    };
    const std::uint32_t data = static_cast<std::uint32_t>(record_bytes_);
    std::uint8_t h[44];
    std::memcpy(h, "RIFF", 4);
    put32(h + 4, 36 + data);
    std::memcpy(h + 8, "WAVEfmt ", 8);
    put32(h + 16, 16);
    put16(h + 20, 3);  /* IEEE float */
    put16(h + 22, 2);
    put32(h + 24, k_rate);
    put32(h + 28, k_rate * 8);
    put16(h + 32, 8);
    put16(h + 34, 32);
    std::memcpy(h + 36, "data", 4);
    put32(h + 40, data);
    const long end = std::ftell(record_);
    std::fseek(record_, 0, SEEK_SET);
    std::fwrite(h, 1, sizeof(h), record_);
    std::fseek(record_, end, SEEK_SET);
    std::fflush(record_);
}


/* one line per voice's playback, then its counters start again */
void Mixer::trace_voice_end(const Voice& cv, const char* why) {
    if (!trace_voices() || cv.bus || !cv.traced_frames)
        return;
    Voice& v = const_cast<Voice&>(cv);
    std::fprintf(stderr, "audio-voice %08x %s %s fmt 0x%04x %u ch %u Hz%s: %llu frames, source peak %.3f, "
                 "output peak %.3f, unreadable packets %u, packets %u (zero at submit %u, at play %u), bins",
                 v.id, why, v.stream ? "stream" : "buffer", v.format.tag, v.format.channels,
                 v.format.samples_per_second, v.spatial ? " 3d" : "",
                 static_cast<unsigned long long>(v.traced_frames), v.traced_source_peak, v.traced_peak,
                 v.traced_unreadable, v.traced_packets, v.traced_zero_submit, v.traced_zero_play);
    for (std::uint32_t k = 0; k < v.mixbin_count; ++k)
        std::fprintf(stderr, " %u@%d", v.mixbins[k].bin, v.mixbins[k].volume);
    std::fprintf(stderr, " vol %d\n", v.volume);
    v.traced_frames = 0;
    v.traced_peak = v.traced_source_peak = 0.0f;
    v.traced_unreadable = 0;
    v.traced_zero_submit = v.traced_zero_play = v.traced_packets = 0;
}

Mixer::Voice* Mixer::find(std::uint32_t id) {
    const auto it = voices_.find(id);
    return it == voices_.end() ? nullptr : it->second.get();
}

void Mixer::command(const h2_audio_command& command) {
    std::lock_guard<std::mutex> lock(mutex_);
    ++stats_.commands;
    apply(command);
}

void Mixer::apply(const h2_audio_command& c) {
    if (c.kind == H2_AUDIO_SET_LISTENER) {
        std::memcpy(listener_.position, c.position, sizeof(listener_.position));
        std::memcpy(listener_.front, c.front, sizeof(listener_.front));
        std::memcpy(listener_.top, c.top, sizeof(listener_.top));
        listener_.rolloff = c.rolloff;
        listener_.distance_factor = c.distance_factor;
        return;
    }
    if (c.kind == H2_AUDIO_SET_MIXBIN_HEADROOM) {
        /* Xbox mix-bin headroom attenuates the bin; the game sets 0 */
        if (c.offset < H2_AUDIO_MIXBIN_COUNT)
            mixbin_gain_[c.offset] = millibels_to_gain(-std::abs(c.headroom));
        return;
    }
    if (c.kind == H2_AUDIO_CREATE) {
        auto voice = std::make_unique<Voice>();
        Voice& v = *voice;
        v.id = c.voice;
        v.flags = c.flags;
        v.format = c.format;
        v.input_mixbin = c.input_mixbin;
        v.stream = (c.flags & H2_AUDIO_VOICE_STREAM) != 0;
        v.bus = !v.stream && (c.flags & H2_AUDIO_VOICE_MIXIN) != 0;
        v.spatial = (c.flags & H2_AUDIO_VOICE_3D) != 0;
        v.frequency = c.frequency > 0.0f ? c.frequency : static_cast<float>(c.format.samples_per_second);
        v.mixbin_count = std::min<std::uint32_t>(c.mixbin_count, H2_AUDIO_MAX_MIXBINS);
        std::memcpy(v.mixbins, c.mixbins, sizeof(v.mixbins));
        v.reset_gains();
        const std::uint32_t channels = c.format.channels;
        if (c.format.tag == H2_AUDIO_FORMAT_XBOX_ADPCM && (channels == 1 || channels == 2)) {
            v.unit_bytes = k_xbox_adpcm_block_bytes * channels;
            v.unit_frames = k_xbox_adpcm_block_samples;
            v.decodable = true;
        } else if (c.format.tag == H2_AUDIO_FORMAT_PCM && (channels == 1 || channels == 2) &&
                   (c.format.bits_per_sample == 8 || c.format.bits_per_sample == 16)) {
            v.unit_bytes = k_unit_frames * channels * (c.format.bits_per_sample / 8);
            v.unit_frames = k_unit_frames;
            v.decodable = true;
        } else if (c.format.block_align && c.format.block_align <= k_max_unit_bytes) {
            /* unknown format: silence that still consumes at the format's
               byte rate, so packets complete */
            v.unit_bytes = c.format.block_align;
            v.unit_frames = std::clamp<std::uint32_t>(c.format.samples_per_block, 1, k_unit_frames);
        }
        if (v.bus)
            v.bus_input.assign(k_block, 0.0f);
        /* H2_TRACE=audio: each distinct voice format once, and whether it
           decodes (one that doesn't plays as silence) */
        static const bool trace_formats = [] {
            const char* trace = std::getenv("H2_TRACE");
            return trace && std::strstr(trace, "audio");
        }();
        if (trace_formats && !v.bus) {
            const std::uint64_t key = (static_cast<std::uint64_t>(c.format.tag) << 48) ^
                                      (static_cast<std::uint64_t>(channels) << 40) ^
                                      (static_cast<std::uint64_t>(c.format.bits_per_sample) << 32) ^
                                      c.format.samples_per_second ^ (v.stream ? 1ull << 63 : 0);
            if (traced_formats_.insert(key).second)
                std::fprintf(stderr, "audio: %s voice format 0x%04x, %u ch, %u Hz, %u bits, block %u: %s\n",
                             v.stream ? "stream" : "buffer", c.format.tag, channels,
                             c.format.samples_per_second, c.format.bits_per_sample, c.format.block_align,
                             v.decodable ? "decoded" : "SILENT (not decodable)");
        }
        voices_[c.voice] = std::move(voice);
        order_dirty_ = true;
        return;
    }

    Voice* v = find(c.voice);
    if (!v)
        return;
    switch (c.kind) {
    case H2_AUDIO_RELEASE:
        trace_voice_end(*v, "release");
        for (auto& [id, other] : voices_)
            if (other->output == c.voice)
                other->output = 0;
        stats_.packets_completed += v->packets.size();
        voices_.erase(c.voice);
        order_dirty_ = true;
        break;
    case H2_AUDIO_SET_DATA:
        v->data = c.address && c.size ? resolver_(c.address, c.size) : nullptr;
        v->data_size = v->data ? c.size : 0;
        v->cursor = 0;
        v->reset_decoder();
        break;
    case H2_AUDIO_SET_LOOP_REGION:
        v->loop_start = c.offset;
        v->loop_size = c.size;
        break;
    case H2_AUDIO_SET_CURSOR:
        v->cursor = c.offset < v->data_size ? c.offset : 0;
        v->stage_size = 0;
        v->decoded_count = v->decoded_index = 0;
        break;
    case H2_AUDIO_PLAY:
        if (!v->playing) {
            v->reset_gains();
            v->primed = false;
            v->phase = 0.0;
        }
        v->playing = true;
        v->looping = (c.flags & H2_AUDIO_PLAY_LOOPING) != 0;
        break;
    case H2_AUDIO_STOP:
        trace_voice_end(*v, "stop");
        v->playing = false;
        break;
    case H2_AUDIO_PAUSE:
        v->paused = c.flags != 0;
        break;
    case H2_AUDIO_SUBMIT:
        if (v->stream) {
            const std::uint8_t* data = c.address && c.size ? resolver_(c.address, c.size) : nullptr;
            if (!data && c.size) {
                ++v->traced_unreadable;
                /* H2_TRACE=audio: packets whose guest memory cannot be read
                   play as silence; say where they pointed */
                static const bool trace = [] {
                    const char* t = std::getenv("H2_TRACE");
                    return t && std::strstr(t, "audio");
                }();
                static int reported = 0;
                if (trace && reported < 20) {
                    ++reported;
                    std::fprintf(stderr, "audio: voice %08x packet at 0x%08x (%u bytes) is not readable guest memory\n",
                                 v->id, c.address, c.size);
                }
            }
            if (trace_voices() && data) {
                bool zero = true;
                for (std::uint32_t i = 0; i < c.size && zero; ++i) zero = data[i] == 0;
                v->traced_zero_submit += zero;
                ++v->traced_packets;
            }
            v->packets.push_back({data, c.size});
            ++v->submitted;
            ++stats_.packets_submitted;
            if (v->format.tag == H2_AUDIO_FORMAT_XBOX_ADPCM)
                ++stats_.packets_adpcm;
            else if (v->format.tag == H2_AUDIO_FORMAT_PCM)
                ++stats_.packets_pcm;
        }
        break;
    case H2_AUDIO_FLUSH:
        stats_.packets_completed += v->packets.size();
        v->packets.clear();
        v->packet_offset = 0;
        v->completed = v->submitted;
        v->reset_decoder();
        break;
    case H2_AUDIO_SET_VOLUME:
        v->volume = c.volume;
        v->headroom = c.headroom;
        break;
    case H2_AUDIO_SET_FREQUENCY:
        if (c.frequency > 0.0f)
            v->frequency = c.frequency;
        break;
    case H2_AUDIO_SET_MIXBINS: {
        const std::uint32_t count = std::min<std::uint32_t>(c.mixbin_count, H2_AUDIO_MAX_MIXBINS);
        for (std::uint32_t i = 0; i < count; ++i)
            if (i >= v->mixbin_count || v->mixbins[i].bin != c.mixbins[i].bin)
                v->last_gain[i] = k_jump;
        v->mixbin_count = count;
        std::memcpy(v->mixbins, c.mixbins, sizeof(v->mixbins));
        break;
    }
    case H2_AUDIO_SET_OUTPUT:
        v->output = c.offset;
        v->reset_gains();
        break;
    case H2_AUDIO_SET_3D:
        std::memcpy(v->position, c.position, sizeof(v->position));
        v->min_distance = c.min_distance;
        v->max_distance = c.max_distance;
        v->rolloff = c.rolloff;
        break;
    default:
        break;
    }
}

bool Mixer::status(std::uint32_t voice, h2_audio_status& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::memset(&status, 0, sizeof(status));
    Voice* v = find(voice);
    if (!v)
        return false;
    if (v->playing)
        status.flags |= H2_AUDIO_STATUS_PLAYING;
    if (v->paused)
        status.flags |= H2_AUDIO_STATUS_PAUSED;
    status.cursor = v->cursor;
    status.packets_submitted = v->submitted;
    status.packets_completed = v->completed;
    status.frames_low = static_cast<std::uint32_t>(v->frames_played);
    status.frames_high = static_cast<std::uint32_t>(v->frames_played >> 32);
    return true;
}

MixerStats Mixer::stats() {
    std::lock_guard<std::mutex> lock(mutex_);
    MixerStats result = stats_;
    result.voices = static_cast<std::uint32_t>(voices_.size());
    return result;
}

std::uint32_t Mixer::read_bytes(Voice& v, std::uint8_t* target, std::uint32_t count) {
    std::uint32_t got = 0;
    if (v.stream) {
        while (got < count && !v.packets.empty()) {
            Voice::Packet& packet = v.packets.front();
            const std::uint32_t take = std::min(count - got, packet.size - v.packet_offset);
            if (packet.data)
                std::memcpy(target + got, packet.data + v.packet_offset, take);
            else
                std::memset(target + got, 0, take);
            got += take;
            v.packet_offset += take;
            if (v.packet_offset >= packet.size) {
                if (trace_voices() && packet.data) {
                    bool zero = true;
                    for (std::uint32_t i = 0; i < packet.size && zero; ++i) zero = packet.data[i] == 0;
                    v.traced_zero_play += zero;
                }
                /* the packet's data is never read again: it is complete */
                v.packets.pop_front();
                v.packet_offset = 0;
                ++v.completed;
                ++stats_.packets_completed;
            }
        }
        return got;
    }
    if (!v.data || !v.data_size)
        return 0;
    std::uint32_t loop_start = v.loop_start < v.data_size ? v.loop_start : 0;
    std::uint32_t loop_end = v.loop_size ? std::min(v.data_size, loop_start + v.loop_size) : v.data_size;
    if (loop_end <= loop_start) {
        loop_start = 0;
        loop_end = v.data_size;
    }
    while (got < count) {
        const std::uint32_t end = v.looping ? loop_end : v.data_size;
        if (v.cursor >= end) {
            if (!v.looping)
                break;
            v.cursor = loop_start;
            continue;
        }
        const std::uint32_t take = std::min(count - got, end - v.cursor);
        std::memcpy(target + got, v.data + v.cursor, take);
        got += take;
        v.cursor += take;
    }
    return got;
}

bool Mixer::refill(Voice& v) {
    if (!v.unit_bytes)
        return false;
    v.stage_size += read_bytes(v, v.stage + v.stage_size, v.unit_bytes - v.stage_size);
    std::uint32_t frames = v.unit_frames;
    if (v.stage_size < v.unit_bytes) {
        /* a stream waits for its next packet with the partial unit kept; a
           buffer has reached its end: PCM plays its whole frames */
        if (v.stream)
            return false;
        const std::uint32_t frame_bytes = v.unit_bytes / v.unit_frames;
        frames = v.format.tag == H2_AUDIO_FORMAT_PCM && frame_bytes ? v.stage_size / frame_bytes : 0;
        if (!frames) {
            v.stage_size = 0;
            return false;
        }
    }
    const unsigned channels = v.format.channels == 2 ? 2 : 1;
    if (!v.decodable) {
        std::fill(v.decoded, v.decoded + frames * 2, 0.0f);
    } else if (v.format.tag == H2_AUDIO_FORMAT_XBOX_ADPCM) {
        std::int16_t samples[k_xbox_adpcm_block_samples * 2];
        xbox_adpcm_decode_block(v.stage, channels, samples);
        for (std::uint32_t i = 0; i < frames; ++i) {
            const float left = samples[i * channels] / 32768.0f;
            v.decoded[i * 2] = left;
            v.decoded[i * 2 + 1] = channels == 2 ? samples[i * 2 + 1] / 32768.0f : left;
        }
    } else if (v.format.bits_per_sample == 16) {
        for (std::uint32_t i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < channels; ++c) {
                const std::uint8_t* p = v.stage + (i * channels + c) * 2;
                v.decoded[i * 2 + c] = static_cast<std::int16_t>(p[0] | (p[1] << 8)) / 32768.0f;
            }
            if (channels == 1)
                v.decoded[i * 2 + 1] = v.decoded[i * 2];
        }
    } else {
        /* 8-bit PCM is unsigned, 128 = silence */
        for (std::uint32_t i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < channels; ++c)
                v.decoded[i * 2 + c] = (static_cast<int>(v.stage[i * channels + c]) - 128) / 128.0f;
            if (channels == 1)
                v.decoded[i * 2 + 1] = v.decoded[i * 2];
        }
    }
    v.stage_size = 0;
    v.decoded_count = frames;
    v.decoded_index = 0;
    return true;
}

bool Mixer::fetch_frame(Voice& v, float frame[2]) {
    if (v.decoded_index >= v.decoded_count && !refill(v))
        return false;
    frame[0] = v.decoded[v.decoded_index * 2];
    frame[1] = v.decoded[v.decoded_index * 2 + 1];
    ++v.decoded_index;
    ++v.frames_played;
    return true;
}

/* Linear-interpolating resampler from the voice's frequency to k_rate. A
 * stream that runs dry outputs silence and resumes with its next packet; a
 * buffer that reaches its end stops. */
void Mixer::render_voice_source(Voice& v, float* left, float* right, std::size_t frames) {
    double rate = v.frequency > 0.0f ? v.frequency : static_cast<double>(v.format.samples_per_second);
    if (rate <= 0.0)
        rate = k_rate;
    const double step = rate / static_cast<double>(k_rate);
    bool dry = false;
    if (!v.primed) {
        v.s0[0] = v.s0[1] = 0.0f;
        if (!fetch_frame(v, v.s1)) {
            v.s1[0] = v.s1[1] = 0.0f;
            dry = true;
        } else {
            v.primed = true;
        }
        v.phase = 0.0;
        if (dry) {
            std::fill(left, left + frames, 0.0f);
            std::fill(right, right + frames, 0.0f);
            if (!v.stream) {
                trace_voice_end(v, "end");
        v.playing = false;
                v.cursor = 0;
            }
            return;
        }
    }
    for (std::size_t i = 0; i < frames; ++i) {
        const float t = static_cast<float>(v.phase);
        left[i] = v.s0[0] + (v.s1[0] - v.s0[0]) * t;
        right[i] = v.s0[1] + (v.s1[1] - v.s0[1]) * t;
        v.phase += step;
        while (v.phase >= 1.0) {
            v.phase -= 1.0;
            v.s0[0] = v.s1[0];
            v.s0[1] = v.s1[1];
            if (dry || !fetch_frame(v, v.s1)) {
                v.s1[0] = v.s1[1] = 0.0f;
                if (!dry) {
                    dry = true;
                    /* the next packet restarts the interpolation cleanly */
                    v.primed = false;
                }
            }
        }
    }
    if (dry && v.stream)
        trace_voice_end(v, "drained");
    if (dry && !v.stream) {
        trace_voice_end(v, "end");
        v.playing = false;
        v.cursor = 0;
        v.reset_decoder();
    }
}

void Mixer::spatialize(const Voice& v, Pan& pan) const {
    pan = Pan{};
    float relative[3];
    for (int i = 0; i < 3; ++i)
        relative[i] = v.position[i] - listener_.position[i];
    const float distance = length3(relative);
    /* DirectSound 3D inverse distance law */
    const float min_distance = v.min_distance > 0.0f ? v.min_distance : 1.0f;
    const float max_distance = std::max(v.max_distance, min_distance);
    const float rolloff = std::max(0.0f, v.rolloff * listener_.rolloff);
    if (distance > min_distance && rolloff > 0.0f) {
        const float clamped = std::min(distance, max_distance);
        pan.gain = min_distance / (min_distance + rolloff * (clamped - min_distance));
    }
    if (distance < 1e-4f)
        return;
    /* right = top x front (left-handed, y up) */
    const float* f = listener_.front;
    const float* u = listener_.top;
    float axis[3] = {u[1] * f[2] - u[2] * f[1], u[2] * f[0] - u[0] * f[2], u[0] * f[1] - u[1] * f[0]};
    const float axis_length = length3(axis);
    if (axis_length < 1e-6f)
        return;
    const float side = std::clamp(
        (relative[0] * axis[0] + relative[1] * axis[1] + relative[2] * axis[2]) / (distance * axis_length),
        -1.0f, 1.0f);
    const float angle = (side + 1.0f) * 0.25f * 3.14159265f;
    pan.left = std::min(1.0f, std::cos(angle) * 1.41421356f);
    pan.right = std::min(1.0f, std::sin(angle) * 1.41421356f);
    /* front and back bins share the source linearly, so the stereo fold
       (which sums them) keeps its level wherever it is */
    const float front_length = length3(f);
    if (front_length > 1e-6f) {
        const float ahead = std::clamp(
            (relative[0] * f[0] + relative[1] * f[1] + relative[2] * f[2]) / (distance * front_length),
            -1.0f, 1.0f);
        pan.front = 0.5f * (1.0f + ahead);
        pan.back = 1.0f - pan.front;
    }
}

/* adds a voice's block to its bus or its mix bins, ramping each level from
 * the previous block's to avoid zipper noise */
void Mixer::route(Voice& v, const float* left, const float* right, std::size_t frames,
                  bool allow_fx) {
    float gain = millibels_to_gain(v.volume) * millibels_to_gain(-std::abs(v.headroom));
    Pan pan;
    if (v.spatial) {
        spatialize(v, pan);
        gain *= pan.gain;
    }
    const float inverse = 1.0f / static_cast<float>(frames);

    if (!v.bus && v.output) {
        Voice* target = find(v.output);
        if (target && target->bus) {
            const float from = v.last_bus_gain == k_jump ? gain : v.last_bus_gain;
            const unsigned channels = v.channels();
            for (std::size_t i = 0; i < frames; ++i) {
                const float g = from + (gain - from) * (i * inverse);
                const float sample = channels == 2 ? 0.5f * (left[i] + right[i]) : left[i];
                target->bus_input[i] += sample * g;
            }
            v.last_bus_gain = gain;
            return;
        }
    }

    const unsigned channels = v.channels();
    for (std::uint32_t k = 0; k < v.mixbin_count; ++k) {
        const std::uint32_t bin = v.mixbins[k].bin;
        if (bin >= H2_AUDIO_MIXBIN_COUNT || bin == H2_AUDIO_I3DL2_MIXBIN || bin == 3)
            continue;
        if (bin >= H2_AUDIO_FIRST_FX_MIXBIN && !allow_fx)
            continue;
        float g = gain * millibels_to_gain(v.mixbins[k].volume);
        if (v.spatial && bin <= 9 && bin != 2) {
            const bool left_side = bin == 0 || bin == 4 || bin == 6 || bin == 8;
            const bool back = bin == 4 || bin == 5 || bin == 8 || bin == 9;
            g *= (left_side ? pan.left : pan.right) * (back ? pan.back : pan.front);
        }
        const float from = v.last_gain[k] == k_jump ? g : v.last_gain[k];
        v.last_gain[k] = g;
        if (from == 0.0f && g == 0.0f)
            continue;
        const float* source = (k % channels) == 1 ? right : left;
        float* row = &bins_[bin * k_block];
        for (std::size_t i = 0; i < frames; ++i)
            row[i] += source[i] * (from + (g - from) * (i * inverse));
        if (trace_voices())
            for (std::size_t i = 0; i < frames; ++i) {
                v.traced_source_peak = std::max(v.traced_source_peak, std::fabs(source[i]));
                v.traced_peak = std::max(v.traced_peak, std::fabs(source[i] * g));
            }
    }
}

void Mixer::render_block(float* out, std::size_t frames) {
    if (order_dirty_) {
        order_.clear();
        for (auto& [id, voice] : voices_)
            if (!voice->bus)
                order_.push_back(voice.get());
        /* buses fed only by routed voices, then the effect-send readers */
        for (auto& [id, voice] : voices_)
            if (voice->bus && voice->input_mixbin < H2_AUDIO_FIRST_FX_MIXBIN)
                order_.push_back(voice.get());
        for (auto& [id, voice] : voices_)
            if (voice->bus && voice->input_mixbin >= H2_AUDIO_FIRST_FX_MIXBIN)
                order_.push_back(voice.get());
        order_dirty_ = false;
    }
    std::fill(bins_.begin(), bins_.end(), 0.0f);
    for (Voice* voice : order_)
        if (voice->bus)
            std::fill(voice->bus_input.begin(), voice->bus_input.begin() + frames, 0.0f);

    float* left = scratch_.data();
    float* right = scratch_.data() + k_block;
    std::uint32_t audible = 0;
    for (Voice* voice : order_) {
        Voice& v = *voice;
        if (v.paused)
            continue;
        if (v.bus) {
            /* a submix passes what is routed into it whether or not it was
               played: the game never plays its dialogue submix, whose lines
               retail is heard speaking (the bus has no data of its own) */
            for (std::size_t i = 0; i < frames; ++i)
                left[i] = v.bus_input[i];
            if (v.input_mixbin >= H2_AUDIO_FIRST_FX_MIXBIN && v.input_mixbin < H2_AUDIO_MIXBIN_COUNT) {
                const float* row = &bins_[v.input_mixbin * k_block];
                for (std::size_t i = 0; i < frames; ++i)
                    left[i] += row[i];
            }
            std::copy(left, left + frames, right);
            /* a bus reading an effect send never writes effect sends, so
               the routing has no cycles */
            route(v, left, right, frames, v.input_mixbin < H2_AUDIO_FIRST_FX_MIXBIN);
            ++audible;
            continue;
        }
        if (v.stream) {
            if (v.packets.empty() && v.decoded_index >= v.decoded_count && !v.primed)
                continue;
        } else if (!v.playing || !v.data) {
            continue;
        }
        render_voice_source(v, left, right, frames);
        v.traced_frames += frames;
        route(v, left, right, frames, true);
        ++audible;
    }
    stats_.voices_audible = audible;

    constexpr float k_center = 0.70710678f;
    const float* b = bins_.data();
    for (std::size_t i = 0; i < frames; ++i) {
        const auto at = [&](unsigned bin) { return b[bin * k_block + i] * mixbin_gain_[bin]; };
        const float center = at(2) * k_center;
        float l = at(0) + at(4) + at(6) + at(8) + center;
        float r = at(1) + at(5) + at(7) + at(9) + center;
        stats_.peak = std::max(stats_.peak, std::max(std::fabs(l), std::fabs(r)));
        out[i * 2] = std::clamp(l, -1.0f, 1.0f);
        out[i * 2 + 1] = std::clamp(r, -1.0f, 1.0f);
    }
    stats_.frames_rendered += frames;
    if (record_) {
        std::fwrite(out, sizeof(float), frames * 2, record_);
        record_bytes_ += frames * 2 * sizeof(float);
        /* keep the header true: the game exits without unwinding */
        if (++record_blocks_ % 256 == 0)
            record_patch();
    }
}

void Mixer::render(float* out, std::size_t frames) {
    std::lock_guard<std::mutex> lock(mutex_);
    while (frames) {
        const std::size_t count = std::min(frames, k_block);
        render_block(out, count);
        out += count * 2;
        frames -= count;
    }
}

void Mixer::advance_clock(std::uint64_t now_ns) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!clock_started_) {
        clock_started_ = true;
        clock_start_ns_ = now_ns;
        clock_frames_ = 0;
        return;
    }
    const std::uint64_t elapsed = now_ns > clock_start_ns_ ? now_ns - clock_start_ns_ : 0;
    const std::uint64_t due = elapsed * k_rate / 1000000000ull;
    if (due <= clock_frames_)
        return;
    std::uint64_t frames = due - clock_frames_;
    const std::uint64_t limit = k_max_step_ns * k_rate / 1000000000ull;
    if (frames > limit) {
        /* a stall: the time beyond the limit is skipped, not played */
        clock_frames_ = due - limit;
        frames = limit;
    }
    clock_frames_ += frames;
    float discard[k_block * 2];
    while (frames) {
        const std::size_t count = static_cast<std::size_t>(std::min<std::uint64_t>(frames, k_block));
        render_block(discard, count);
        frames -= count;
    }
}

}  // namespace h2::audio
