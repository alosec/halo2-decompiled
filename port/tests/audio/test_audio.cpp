/* Standalone tests for the host mixer and the Xbox ADPCM decoder
 * (port/src/audio/).
 *
 *   clang++ -std=c++20 -I port/src -I port/include \
 *     port/src/audio/xbox_adpcm.cpp port/src/audio/mixer.cpp \
 *     port/tests/audio/test_audio.cpp -o /tmp/test_audio && /tmp/test_audio
 *
 * The ADPCM checks use hand-computed IMA steps, a block whose codes make each
 * sample move by one (to pin the nibble order and channel interleave) and a
 * round trip through the standard IMA encoder. The mixer checks feed it
 * commands as the guest runtime would, with "guest memory" in host vectors. */
#include "audio/mixer.hpp"
#include "audio/xbox_adpcm.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <map>
#include <thread>
#include <vector>

using namespace h2::audio;

namespace {

int failures = 0;
int checks = 0;

void check(bool condition, const char* what) {
    ++checks;
    if (!condition) {
        ++failures;
        std::printf("FAIL: %s\n", what);
    }
}

bool near(float a, float b, float tolerance) {
    return std::fabs(a - b) <= tolerance;
}

/* ---- ADPCM ---- */

void test_ima_steps() {
    int predictor = 0, index = 0;
    /* step 7: 7>>3 + 7>>2 + 7>>1 + 7 = 0 + 1 + 3 + 7 */
    check(ima_adpcm_step(0x7, predictor, index) == 11 && index == 8, "ima code 7 at index 0");
    /* step 16: 16>>3 = 2, index 8 - 1 */
    check(ima_adpcm_step(0x0, predictor, index) == 13 && index == 7, "ima code 0 at index 8");
    /* step 14: -(14>>3) */
    check(ima_adpcm_step(0x8, predictor, index) == 12 && index == 6, "ima code 8 at index 7");
    predictor = 32760;
    index = 88;
    check(ima_adpcm_step(0x7, predictor, index) == 32767 && index == 88, "ima clamps high");
    predictor = -32760;
    check(ima_adpcm_step(0xf, predictor, index) == -32768, "ima clamps low");
}

/* code 1 at index 0 adds step 7 >> 2 = 1 (index stays 0); code 9 subtracts 1 */
void test_block_layout() {
    std::uint8_t block[72] = {};
    const std::int16_t left_start = 1000, right_start = -500;
    block[0] = left_start & 0xff;
    block[1] = (left_start >> 8) & 0xff;
    block[2] = 0;
    block[4] = right_start & 0xff;
    block[5] = (right_start >> 8) & 0xff;
    block[6] = 0;
    for (int group = 0; group < 8; ++group) {
        for (int b = 0; b < 4; ++b) {
            block[8 + (group * 2 + 0) * 4 + b] = 0x11;
            block[8 + (group * 2 + 1) * 4 + b] = 0x99;
        }
    }
    std::int16_t frames[128];
    xbox_adpcm_decode_block(block, 2, frames);
    bool ok = true;
    for (int i = 0; i < 64; ++i)
        ok &= frames[i * 2] == left_start + i + 1 && frames[i * 2 + 1] == right_start - i - 1;
    check(ok, "stereo block: 64 frames, low nibble first, 4-byte channel interleave");

    /* mono: one header, 32 data bytes; codes alternate +1 (low) and -1 (high) */
    std::uint8_t mono[36] = {};
    mono[0] = 0x10;  /* 16 */
    for (int i = 0; i < 32; ++i)
        mono[4 + i] = 0x91;
    std::int16_t samples[64];
    xbox_adpcm_decode_block(mono, 1, samples);
    ok = true;
    for (int i = 0; i < 64; ++i)
        ok &= samples[i] == (i % 2 == 0 ? 17 : 16);
    check(ok, "mono block nibble order");
}

/* the standard IMA encoder (the inverse of the recommended-practice decoder) */
std::uint8_t ima_encode(int sample, int& predictor, int& index) {
    static const int steps[89] = {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60,
        66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371,
        408, 449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707,
        1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
        7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385,
        24623, 27086, 29794, 32767};
    int step = steps[index];
    int difference = sample - predictor;
    std::uint8_t code = 0;
    if (difference < 0) {
        code = 8;
        difference = -difference;
    }
    if (difference >= step) { code |= 4; difference -= step; }
    step >>= 1;
    if (difference >= step) { code |= 2; difference -= step; }
    step >>= 1;
    if (difference >= step) code |= 1;
    ima_adpcm_step(code, predictor, index);  /* track the decoder's state */
    return code;
}

void test_round_trip() {
    /* 4 stereo blocks: left 440 Hz, right 1 kHz at 44.1 kHz */
    const int blocks = 4;
    std::vector<std::uint8_t> data(blocks * 72);
    std::vector<float> reference(blocks * 64 * 2);
    int predictor[2] = {0, 0}, index[2] = {0, 0};
    for (int k = 0; k < blocks; ++k) {
        std::uint8_t* block = &data[k * 72];
        for (int c = 0; c < 2; ++c) {
            block[c * 4] = predictor[c] & 0xff;
            block[c * 4 + 1] = (predictor[c] >> 8) & 0xff;
            block[c * 4 + 2] = static_cast<std::uint8_t>(index[c]);
        }
        for (int n = 0; n < 64; ++n) {
            const int t = k * 64 + n;
            const int value[2] = {
                static_cast<int>(12000 * std::sin(2 * M_PI * 440 * t / 44100.0)),
                static_cast<int>(8000 * std::sin(2 * M_PI * 1000 * t / 44100.0))};
            for (int c = 0; c < 2; ++c) {
                reference[t * 2 + c] = static_cast<float>(value[c]);
                const std::uint8_t code = ima_encode(value[c], predictor[c], index[c]);
                const int group = n / 8, byte = (n % 8) / 2;
                std::uint8_t& target = block[8 + (group * 2 + c) * 4 + byte];
                target |= (n % 2) ? static_cast<std::uint8_t>(code << 4) : code;
            }
        }
    }
    double signal = 0, noise = 0;
    for (int k = 0; k < blocks; ++k) {
        std::int16_t frames[128];
        xbox_adpcm_decode_block(&data[k * 72], 2, frames);
        /* skip the first block: the encoder starts from silence */
        for (int i = 0; i < 128 && k > 0; ++i) {
            const double s = reference[k * 128 + i];
            signal += s * s;
            noise += (frames[i] - s) * (frames[i] - s);
        }
    }
    const double snr = 10 * std::log10(signal / std::max(noise, 1e-9));
    std::printf("adpcm round trip SNR %.1f dB\n", snr);
    check(snr > 25.0, "adpcm round trip SNR above 25 dB");
}

/* ---- mixer ---- */

struct Memory {
    std::map<std::uint32_t, std::vector<std::uint8_t>> blocks;
    std::uint32_t next = 0x10000;

    std::uint32_t add(std::vector<std::uint8_t> bytes) {
        const std::uint32_t address = next;
        next += static_cast<std::uint32_t>(bytes.size() + 0x1000);
        blocks[address] = std::move(bytes);
        return address;
    }
    MemoryResolver resolver() {
        return [this](std::uint32_t address, std::uint32_t size) -> const std::uint8_t* {
            const auto it = blocks.find(address);
            if (it == blocks.end() || it->second.size() < size)
                return nullptr;
            return it->second.data();
        };
    }
};

std::vector<std::uint8_t> pcm16(const std::vector<std::int16_t>& samples) {
    std::vector<std::uint8_t> bytes(samples.size() * 2);
    std::memcpy(bytes.data(), samples.data(), bytes.size());
    return bytes;
}

h2_audio_command make(std::uint32_t kind, std::uint32_t voice) {
    h2_audio_command c;
    std::memset(&c, 0, sizeof(c));
    c.kind = kind;
    c.voice = voice;
    return c;
}

h2_audio_command create(std::uint32_t voice, std::uint32_t flags, std::uint32_t tag,
                        std::uint32_t channels, std::uint32_t rate, std::uint32_t bits,
                        std::initializer_list<h2_audio_mixbin> bins, std::uint32_t input = 0) {
    h2_audio_command c = make(H2_AUDIO_CREATE, voice);
    c.flags = flags;
    c.format.tag = tag;
    c.format.channels = channels;
    c.format.samples_per_second = rate;
    c.format.bits_per_sample = bits;
    c.format.block_align = tag == H2_AUDIO_FORMAT_XBOX_ADPCM ? 36 * channels : channels * bits / 8;
    c.format.samples_per_block = tag == H2_AUDIO_FORMAT_XBOX_ADPCM ? 64 : 1;
    c.frequency = static_cast<float>(rate);
    c.input_mixbin = input;
    for (const auto& bin : bins)
        c.mixbins[c.mixbin_count++] = bin;
    return c;
}

void set_data(Mixer& mixer, std::uint32_t voice, std::uint32_t address, std::uint32_t size) {
    h2_audio_command c = make(H2_AUDIO_SET_DATA, voice);
    c.address = address;
    c.size = size;
    mixer.command(c);
}

void play(Mixer& mixer, std::uint32_t voice, bool looping = false) {
    h2_audio_command c = make(H2_AUDIO_PLAY, voice);
    c.flags = looping ? H2_AUDIO_PLAY_LOOPING : 0;
    mixer.command(c);
}

std::vector<float> render(Mixer& mixer, std::size_t frames) {
    std::vector<float> out(frames * 2);
    mixer.render(out.data(), frames);
    return out;
}

void test_pcm_unity() {
    Memory memory;
    Mixer mixer(memory.resolver());
    std::vector<std::int16_t> samples(1000);
    for (int i = 0; i < 1000; ++i)
        samples[i] = static_cast<std::int16_t>((i % 100) * 300 - 15000);
    const std::uint32_t address = memory.add(pcm16(samples));
    mixer.command(create(1, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 48000, 16, {{0, 0}, {1, 0}}));
    set_data(mixer, 1, address, 2000);
    play(mixer, 1);
    const auto out = render(mixer, 1100);
    bool ok = true;
    for (int i = 1; i <= 1000; ++i)
        ok &= near(out[i * 2], samples[i - 1] / 32768.0f, 1e-5f) &&
              near(out[i * 2 + 1], samples[i - 1] / 32768.0f, 1e-5f);
    check(ok, "pcm16 mono at 48 kHz plays sample-exact (one frame of latency) to both bins");
    bool silent_after = true;
    for (int i = 1002; i < 1100; ++i)
        silent_after &= out[i * 2] == 0.0f;
    check(silent_after, "a non-looping buffer is silent after its end");
    h2_audio_status status;
    check(mixer.status(1, status) && !(status.flags & H2_AUDIO_STATUS_PLAYING),
          "a non-looping buffer stops at its end");
    check(!mixer.status(99, status), "an unknown voice has no status");
}

void test_volume_and_bins() {
    Memory memory;
    Mixer mixer(memory.resolver());
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(4800, 16384)));
    /* bin 6 (3D front left) at -600 mB, center (2), LFE (3) and reverb (10) */
    mixer.command(create(1, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 48000, 16,
                         {{6, -600}, {2, 0}, {3, 0}, {10, 0}}));
    set_data(mixer, 1, address, 9600);
    h2_audio_command volume = make(H2_AUDIO_SET_VOLUME, 1);
    volume.volume = -600;
    mixer.command(volume);
    play(mixer, 1, true);
    const auto out = render(mixer, 1024);
    const float g6 = std::pow(10.0f, -0.3f);
    const float l = 0.5f * g6 * g6 + 0.5f * g6 * 0.70710678f;
    const float r = 0.5f * g6 * 0.70710678f;
    check(near(out[600 * 2], l, 1e-4f) && near(out[600 * 2 + 1], r, 1e-4f),
          "volume and mix-bin levels in millibels; center folds at -3 dB; LFE and reverb dropped");
    check(near(millibels_to_gain(-2000), 0.1f, 1e-6f) && millibels_to_gain(-10000) == 0.0f,
          "millibel conversion");
}

void test_resample_and_loop() {
    Memory memory;
    Mixer mixer(memory.resolver());
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(2400, 8192)));
    mixer.command(create(1, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 24000, 16, {{0, 0}}));
    set_data(mixer, 1, address, 4800);
    play(mixer, 1);
    h2_audio_status status;
    render(mixer, 4700);
    check(mixer.status(1, status) && (status.flags & H2_AUDIO_STATUS_PLAYING),
          "24 kHz buffer of 0.1 s still plays at 4700 output frames");
    render(mixer, 200);
    check(mixer.status(1, status) && !(status.flags & H2_AUDIO_STATUS_PLAYING),
          "24 kHz buffer of 0.1 s ends after 4800 output frames");
    check(status.frames_low == 2400, "frames played counts source frames");

    /* looping: still playing after three lengths */
    play(mixer, 1, true);
    render(mixer, 3 * 4800);
    check(mixer.status(1, status) && (status.flags & H2_AUDIO_STATUS_PLAYING), "a looping buffer keeps playing");

    /* a pitch change: frequency 96 kHz plays 2 source frames per output frame */
    Mixer fast(memory.resolver());
    fast.command(create(2, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 24000, 16, {{0, 0}}));
    set_data(fast, 2, address, 4800);
    h2_audio_command frequency = make(H2_AUDIO_SET_FREQUENCY, 2);
    frequency.frequency = 96000.0f;
    fast.command(frequency);
    play(fast, 2);
    render(fast, 1210);
    check(fast.status(2, status) && !(status.flags & H2_AUDIO_STATUS_PLAYING),
          "SET_FREQUENCY changes the playback rate");
}

void test_8bit() {
    Memory memory;
    Mixer mixer(memory.resolver());
    std::vector<std::uint8_t> bytes(256, 128);
    bytes[10] = 255;
    bytes[11] = 0;
    const std::uint32_t address = memory.add(bytes);
    mixer.command(create(1, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 48000, 8, {{0, 0}}));
    set_data(mixer, 1, address, 256);
    play(mixer, 1);
    const auto out = render(mixer, 64);
    check(out[2] == 0.0f && near(out[11 * 2], 127 / 128.0f, 1e-6f) && out[12 * 2] == -1.0f,
          "8-bit PCM is unsigned around 128");
}

void test_stream_packets() {
    Memory memory;
    Mixer mixer(memory.resolver());
    /* stereo 16-bit at 48 kHz: left constant, right constant */
    std::vector<std::int16_t> samples(4800 * 2);
    for (int i = 0; i < 4800; ++i) {
        samples[i * 2] = 8192;
        samples[i * 2 + 1] = -8192;
    }
    const std::uint32_t a = memory.add(pcm16(samples));
    const std::uint32_t b = memory.add(pcm16(samples));
    mixer.command(create(0x1000, H2_AUDIO_VOICE_STREAM, H2_AUDIO_FORMAT_PCM, 2, 48000, 16, {{0, 0}, {1, 0}}));
    h2_audio_command submit = make(H2_AUDIO_SUBMIT, 0x1000);
    submit.address = a;
    submit.size = 4800 * 4;
    mixer.command(submit);
    submit.address = b;
    mixer.command(submit);
    h2_audio_status status;
    auto out = render(mixer, 4700);
    check(mixer.status(0x1000, status) && status.packets_submitted == 2 && status.packets_completed == 0,
          "stream: no packet completes before its data is read");
    check(near(out[1000 * 2], 0.25f, 1e-6f) && near(out[1000 * 2 + 1], -0.25f, 1e-6f),
          "stereo stream: mix-bin entries take channels in turn");
    render(mixer, 200);
    check(mixer.status(0x1000, status) && status.packets_completed == 1,
          "stream: the first packet completes after its 4800 frames");
    out = render(mixer, 4800);
    check(mixer.status(0x1000, status) && status.packets_completed == 2, "stream: second packet completes");
    bool continuous = true;
    for (int i = 0; i < 4700; ++i)
        continuous &= near(out[i * 2], 0.25f, 1e-6f);
    check(continuous, "stream: packets play back to back without a gap");
    out = render(mixer, 256);
    check(out[200 * 2] == 0.0f, "stream: silence once starved");

    /* a packet submitted after starving plays and completes */
    submit.address = a;
    mixer.command(submit);
    mixer.command(submit);
    render(mixer, 100);
    h2_audio_command flush = make(H2_AUDIO_FLUSH, 0x1000);
    mixer.command(flush);
    check(mixer.status(0x1000, status) && status.packets_submitted == 4 && status.packets_completed == 4,
          "stream: flush counts every submitted packet as completed");
    out = render(mixer, 256);
    check(out[100 * 2] == 0.0f, "stream: flushed packets are not played");

    /* pause holds the packet */
    mixer.command(submit);
    h2_audio_command pause = make(H2_AUDIO_PAUSE, 0x1000);
    pause.flags = 1;
    mixer.command(pause);
    render(mixer, 9600);
    check(mixer.status(0x1000, status) && status.packets_completed == 4 && (status.flags & H2_AUDIO_STATUS_PAUSED),
          "stream: a paused stream does not consume");
    pause.flags = 0;
    mixer.command(pause);
    render(mixer, 4900);
    check(mixer.status(0x1000, status) && status.packets_completed == 5, "stream: resumes after pause");
}

void test_adpcm_stream_split_packets() {
    Memory memory;
    Mixer mixer(memory.resolver());
    /* one mono block that ramps by 1 per sample, split 10 + 26 bytes */
    std::vector<std::uint8_t> block(36, 0x11);
    block[0] = 0;
    block[1] = 0;
    block[2] = 0;
    block[3] = 0;
    std::vector<std::uint8_t> first(block.begin(), block.begin() + 10);
    std::vector<std::uint8_t> second(block.begin() + 10, block.end());
    const std::uint32_t a = memory.add(first), b = memory.add(second);
    mixer.command(create(0x1001, H2_AUDIO_VOICE_STREAM, H2_AUDIO_FORMAT_XBOX_ADPCM, 1, 48000, 4, {{0, 0}}));
    h2_audio_command submit = make(H2_AUDIO_SUBMIT, 0x1001);
    submit.address = a;
    submit.size = 10;
    mixer.command(submit);
    render(mixer, 32);
    h2_audio_status status;
    check(mixer.status(0x1001, status) && status.packets_completed == 1,
          "adpcm stream: a partial block's packet completes once read");
    submit.address = b;
    submit.size = 26;
    mixer.command(submit);
    const auto out = render(mixer, 80);
    bool ok = true;
    for (int i = 1; i <= 64; ++i)
        ok &= near(out[i * 2], i / 32768.0f, 1e-7f);
    check(ok, "adpcm stream: a block split across packets decodes whole");
}

void test_3d_and_bus() {
    Memory memory;
    Mixer mixer(memory.resolver());
    /* listener at the origin facing +z, up +y: +x is to the right */
    h2_audio_command listener = make(H2_AUDIO_SET_LISTENER, 0);
    listener.front[2] = 1.0f;
    listener.top[1] = 1.0f;
    listener.rolloff = 1.0f;
    listener.distance_factor = 1.0f;
    mixer.command(listener);
    /* a 3D bus on the 3D bins, fed by a stream routed to it */
    mixer.command(create(10, H2_AUDIO_VOICE_BUFFER | H2_AUDIO_VOICE_3D | H2_AUDIO_VOICE_MIXIN, 0, 0, 0, 0,
                         {{6, 0}, {8, 0}, {7, 0}, {9, 0}, {10, 0}}));
    play(mixer, 10);
    h2_audio_command spatial = make(H2_AUDIO_SET_3D, 10);
    spatial.position[0] = 4.0f;
    spatial.min_distance = 1.0f;
    spatial.max_distance = 100.0f;
    spatial.rolloff = 1.0f;
    mixer.command(spatial);
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(9600, 16384)));
    mixer.command(create(0x1002, H2_AUDIO_VOICE_STREAM, H2_AUDIO_FORMAT_PCM, 1, 48000, 16, {{0, 0}, {1, 0}}));
    h2_audio_command output = make(H2_AUDIO_SET_OUTPUT, 0x1002);
    output.offset = 10;
    mixer.command(output);
    h2_audio_command submit = make(H2_AUDIO_SUBMIT, 0x1002);
    submit.address = address;
    submit.size = 19200;
    mixer.command(submit);
    auto out = render(mixer, 512);
    /* at 4 units with min 1, rolloff 1: 1 / (1 + 3) */
    check(near(out[300 * 2 + 1], 0.5f * 0.25f, 1e-4f) && near(out[300 * 2], 0.0f, 1e-4f),
          "3D: a source on the right at distance 4 is attenuated by 1/4 and panned right");
    spatial.position[0] = -1.0f;
    spatial.position[2] = 1.0f;
    mixer.command(spatial);
    out = render(mixer, 512);
    check(out[400 * 2] > out[400 * 2 + 1] && out[400 * 2 + 1] > 0.0f, "3D: front-left source favours the left");
    spatial.position[0] = 0.0f;
    spatial.position[2] = -1.0f;
    mixer.command(spatial);
    out = render(mixer, 512);
    check(near(out[400 * 2], 0.5f, 1e-4f) && near(out[400 * 2 + 1], 0.5f, 1e-4f),
          "3D: a source behind folds its back bins without doubling");

    /* releasing the bus sends the stream back to its own mix bins */
    mixer.command(make(H2_AUDIO_RELEASE, 10));
    out = render(mixer, 512);
    check(near(out[400 * 2], 0.5f, 1e-4f) && near(out[400 * 2 + 1], 0.5f, 1e-4f),
          "release: routed voices fall back to their mix bins");
}

void test_effect_send_bus() {
    Memory memory;
    Mixer mixer(memory.resolver());
    /* an effect-input bus reading send 23, out on bin 7 (right) */
    mixer.command(create(20, H2_AUDIO_VOICE_BUFFER | H2_AUDIO_VOICE_MIXIN, 0, 0, 0, 0, {{7, 0}}, 23));
    play(mixer, 20);
    /* an unread send (14) is dropped */
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(4800, 16384)));
    mixer.command(create(1, H2_AUDIO_VOICE_BUFFER, H2_AUDIO_FORMAT_PCM, 1, 48000, 16, {{23, -600}, {14, 0}}));
    set_data(mixer, 1, address, 9600);
    play(mixer, 1, true);
    const auto out = render(mixer, 512);
    check(near(out[300 * 2], 0.0f, 1e-6f) && near(out[300 * 2 + 1], 0.5f * std::pow(10.0f, -0.3f), 1e-4f),
          "effect sends reach the speakers through the bus reading them");
}

/* the game routes dialogue into a submix it never plays: the bus still
   passes its input, and pausing it silences it */
void test_unplayed_bus() {
    Memory memory;
    Mixer mixer(memory.resolver());
    mixer.command(create(30, H2_AUDIO_VOICE_BUFFER | H2_AUDIO_VOICE_MIXIN, 0, 0, 0, 0, {{0, 0}, {1, 0}}));
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(9600, 16384)));
    mixer.command(create(0x1003, H2_AUDIO_VOICE_STREAM, H2_AUDIO_FORMAT_PCM, 1, 48000, 16, {{0, 0}, {1, 0}}));
    h2_audio_command output = make(H2_AUDIO_SET_OUTPUT, 0x1003);
    output.offset = 30;
    mixer.command(output);
    h2_audio_command submit = make(H2_AUDIO_SUBMIT, 0x1003);
    submit.address = address;
    submit.size = 19200;
    mixer.command(submit);
    auto out = render(mixer, 512);
    check(near(out[300 * 2], 0.5f, 1e-4f) && near(out[300 * 2 + 1], 0.5f, 1e-4f),
          "submix: a bus never played still passes what is routed into it");
    h2_audio_command pause = make(H2_AUDIO_PAUSE, 30);
    pause.flags = 1;
    mixer.command(pause);
    out = render(mixer, 512);
    check(near(out[300 * 2], 0.0f, 1e-6f) && near(out[300 * 2 + 1], 0.0f, 1e-6f),
          "submix: a paused bus is silent");
}

void test_clock() {
    Memory memory;
    Mixer mixer(memory.resolver());
    mixer.advance_clock(1000000000ull);
    mixer.advance_clock(1200000000ull);
    check(mixer.stats().frames_rendered == 9600, "clock: 0.2 s renders 9600 frames");
    mixer.advance_clock(11200000000ull);
    check(mixer.stats().frames_rendered == 9600 + 12000, "clock: a 10 s stall renders only 250 ms");
    mixer.advance_clock(11300000000ull);
    check(mixer.stats().frames_rendered == 9600 + 12000 + 4800, "clock: resumes in real time after a stall");
}

/* a device thread rendering while a game thread submits, polls and flushes */
void test_threads() {
    Memory memory;
    const std::uint32_t address = memory.add(pcm16(std::vector<std::int16_t>(4800, 1000)));
    Mixer mixer(memory.resolver());
    mixer.command(create(0x1000, H2_AUDIO_VOICE_STREAM, H2_AUDIO_FORMAT_PCM, 1, 48000, 16, {{0, 0}}));
    std::atomic<bool> done{false};
    std::thread device([&] {
        std::vector<float> out(512 * 2);
        while (!done.load())
            mixer.render(out.data(), 512);
    });
    h2_audio_command submit = make(H2_AUDIO_SUBMIT, 0x1000);
    submit.address = address;
    submit.size = 9600;
    std::uint32_t submitted = 0;
    h2_audio_status status;
    for (int i = 0; i < 2000; ++i) {
        mixer.command(submit);
        ++submitted;
        if (i % 100 == 99)
            mixer.command(make(H2_AUDIO_FLUSH, 0x1000));
        mixer.status(0x1000, status);
    }
    mixer.command(make(H2_AUDIO_FLUSH, 0x1000));
    done = true;
    device.join();
    check(mixer.status(0x1000, status) && status.packets_submitted == submitted &&
              status.packets_completed == submitted,
          "threads: counts stay consistent under concurrent render");
}

}  // namespace

int main() {
    test_ima_steps();
    test_block_layout();
    test_round_trip();
    test_pcm_unity();
    test_volume_and_bins();
    test_resample_and_loop();
    test_8bit();
    test_stream_packets();
    test_adpcm_stream_split_packets();
    test_3d_and_bus();
    test_effect_send_bus();
    test_unplayed_bus();
    test_clock();
    test_threads();
    std::printf("%d/%d checks passed\n", checks - failures, checks);
    return failures ? 1 : 0;
}
