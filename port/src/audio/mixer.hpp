/* The host mixer behind the guest DirectSound runtime (h2/audio_protocol.h).
 *
 * It keeps one voice per guest buffer or stream, decodes PCM (8/16-bit,
 * mono/stereo) and Xbox ADPCM straight from guest memory, resamples each
 * voice to 48 kHz by its frequency (linear interpolation), applies volume,
 * headroom, mix-bin levels and simple 3D distance attenuation and panning,
 * and mixes to interleaved stereo float.
 *
 * Every entry point takes one lock, so the game's threads (commands, status)
 * and the audio device's thread (render) can call it concurrently. Without a
 * device, advance_clock renders (and discards) as many frames as real time
 * has passed, so positions and packet completions still move in real time. */
#pragma once

#include "h2/audio_protocol.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace h2::audio {

/* a guest address and byte count to a host pointer, or nullptr */
using MemoryResolver = std::function<const std::uint8_t*(std::uint32_t address, std::uint32_t size)>;

struct MixerStats {
    std::uint64_t commands = 0;
    std::uint64_t frames_rendered = 0;
    std::uint64_t packets_submitted = 0;
    std::uint64_t packets_completed = 0;
    std::uint64_t packets_adpcm = 0;   /* submitted, by format */
    std::uint64_t packets_pcm = 0;
    std::uint32_t voices = 0;
    std::uint32_t voices_audible = 0;  /* in the last render block */
    float peak = 0.0f;                 /* largest output sample so far */
};

class Mixer {
public:
    static constexpr int k_rate = 48000;
    /* the longest stretch one advance_clock call renders: a host stall (map
       loading) does not complete a burst of packets at once */
    static constexpr std::uint64_t k_max_step_ns = 250000000ull;

    explicit Mixer(MemoryResolver resolver);
    ~Mixer();

    void command(const h2_audio_command& command);
    bool status(std::uint32_t voice, h2_audio_status& status);

    /* interleaved stereo float frames at k_rate */
    void render(float* out, std::size_t frames);

    /* no-device clock: renders the frames due by now_ns (any monotonic
       nanosecond clock; the first call only starts it) */
    void advance_clock(std::uint64_t now_ns);

    MixerStats stats();

private:
    struct Voice;
    struct Listener {
        float position[3] = {0, 0, 0};
        float front[3] = {0, 0, 1};
        float top[3] = {0, 1, 0};
        float rolloff = 1.0f;
        float distance_factor = 1.0f;
    };

    void apply(const h2_audio_command& command);
    void render_block(float* out, std::size_t frames);
    void render_voice_source(Voice& voice, float* left, float* right, std::size_t frames);
    bool fetch_frame(Voice& voice, float frame[2]);
    bool refill(Voice& voice);
    std::uint32_t read_bytes(Voice& voice, std::uint8_t* target, std::uint32_t count);
    void route(Voice& voice, const float* left, const float* right, std::size_t frames,
               bool allow_fx);
    /* a 3D voice's distance gain and its per-side and front/back weights */
    struct Pan {
        float gain = 1.0f;
        float left = 1.0f;
        float right = 1.0f;
        float front = 1.0f;
        float back = 0.0f;
    };
    void spatialize(const Voice& voice, Pan& pan) const;
    Voice* find(std::uint32_t id);

    MemoryResolver resolver_;
    std::mutex mutex_;
    std::unordered_map<std::uint32_t, std::unique_ptr<Voice>> voices_;
    std::vector<Voice*> order_;   /* sources first, then buses */
    bool order_dirty_ = true;
    Listener listener_;
    std::array<float, H2_AUDIO_MIXBIN_COUNT> mixbin_gain_{};
    /* per-block accumulators: one row per mix bin */
    std::vector<float> bins_;
    std::vector<float> scratch_;
    std::uint64_t clock_start_ns_ = 0;
    std::uint64_t clock_frames_ = 0;
    bool clock_started_ = false;
    MixerStats stats_;
};

/* millibels (hundredths of a decibel) to a linear gain; <= -9600 is silence */
float millibels_to_gain(std::int32_t millibels);

}  // namespace h2::audio
