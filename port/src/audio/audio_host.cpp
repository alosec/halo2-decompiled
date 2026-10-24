/* The session mixer and its guest imports; see audio_host.hpp. */
#include "audio/audio_host.hpp"

#include "audio/mixer.hpp"
#include "h2/audio_protocol.h"
#include "h2/guest_arena.hpp"
#include "h2/host_abi.h"

#include <SDL3/SDL.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

namespace h2::audio {
namespace {

struct Session {
    std::unique_ptr<Mixer> mixer;
    SDL_AudioStream* stream = nullptr;
    bool device = false;
    bool trace = false;
    std::vector<float> device_frames;
    std::atomic<std::uint64_t> last_trace_ns{0};

    /* std::exit (the session's last frame) runs this while the device
       thread may still be pulling: stop it before the mixer goes */
    ~Session() {
        if (stream)
            SDL_DestroyAudioStream(stream);
    }
};

Session& session() {
    static Session instance;
    return instance;
}

std::uint64_t now_ns() {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count());
}

void SDLCALL device_callback(void*, SDL_AudioStream* stream, int additional, int) {
    Session& s = session();
    if (!s.mixer || additional <= 0)
        return;
    const std::size_t frames = static_cast<std::size_t>(additional) / (2 * sizeof(float));
    if (!frames)
        return;
    /* only this thread touches device_frames */
    if (s.device_frames.size() < frames * 2)
        s.device_frames.resize(frames * 2);
    s.mixer->render(s.device_frames.data(), frames);
    SDL_PutAudioStreamData(stream, s.device_frames.data(),
                           static_cast<int>(frames * 2 * sizeof(float)));
}

/* the clock without a device, and the trace either way */
void tick() {
    Session& s = session();
    const std::uint64_t now = now_ns();
    if (!s.device)
        s.mixer->advance_clock(now);
    if (s.trace) {
        std::uint64_t last = s.last_trace_ns.load(std::memory_order_relaxed);
        if (now - last > 5000000000ull &&
            s.last_trace_ns.compare_exchange_strong(last, now, std::memory_order_relaxed)) {
            const MixerStats stats = s.mixer->stats();
            std::fprintf(stderr,
                         "audio: %u voices (%u audible), %llu commands, packets %llu/%llu "
                         "(adpcm %llu, pcm %llu), %.1f s rendered, peak %.3f\n",
                         stats.voices, stats.voices_audible,
                         static_cast<unsigned long long>(stats.commands),
                         static_cast<unsigned long long>(stats.packets_completed),
                         static_cast<unsigned long long>(stats.packets_submitted),
                         static_cast<unsigned long long>(stats.packets_adpcm),
                         static_cast<unsigned long long>(stats.packets_pcm),
                         static_cast<double>(stats.frames_rendered) / Mixer::k_rate, stats.peak);
        }
    }
}

}  // namespace

bool open_session(const h2::port::GuestArena& arena, bool device, std::string& error) {
    Session& s = session();
    close_session();
    const char* trace = std::getenv("H2_TRACE");
    s.trace = trace && std::strstr(trace, "audio");
    const h2::port::GuestArena* guest = &arena;
    s.mixer = std::make_unique<Mixer>([guest](std::uint32_t address, std::uint32_t size) {
        const void* pointer = guest->pointer_const(address);
        return guest->host_range_accessible(pointer, size, false)
                   ? static_cast<const std::uint8_t*>(pointer)
                   : nullptr;
    });
    const char* setting = std::getenv("H2_AUDIO");
    if (setting && (!std::strcmp(setting, "off") || !std::strcmp(setting, "0")))
        device = false;
    if (device) {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            std::fprintf(stderr, "audio: no audio subsystem (%s); playing silently\n", SDL_GetError());
        } else {
            const SDL_AudioSpec spec{SDL_AUDIO_F32, 2, Mixer::k_rate};
            s.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                                 device_callback, nullptr);
            if (!s.stream) {
                std::fprintf(stderr, "audio: no playback device (%s); playing silently\n",
                             SDL_GetError());
                SDL_QuitSubSystem(SDL_INIT_AUDIO);
            } else {
                s.device = true;
                SDL_ResumeAudioStreamDevice(s.stream);
            }
        }
    }
    (void)error;
    if (s.trace)
        std::fprintf(stderr, "audio: mixer open, %s\n", s.device ? "playback device" : "silent clock");
    return true;
}

void close_session() {
    Session& s = session();
    if (s.stream) {
        SDL_DestroyAudioStream(s.stream);
        s.stream = nullptr;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    s.device = false;
    s.mixer.reset();
}

}  // namespace h2::audio

extern "C" void host_h2_audio_command(const void* command) {
    auto& s = h2::audio::session();
    if (!s.mixer || !command)
        return;
    h2::audio::tick();
    h2_audio_command copy;
    std::memcpy(&copy, command, sizeof(copy));
    s.mixer->command(copy);
}

extern "C" int host_h2_audio_status(unsigned int voice, void* status) {
    auto& s = h2::audio::session();
    if (!s.mixer || !status)
        return 0;
    h2::audio::tick();
    h2_audio_status result;
    const bool known = s.mixer->status(voice, result);
    std::memcpy(status, &result, sizeof(result));
    return known ? 1 : 0;
}
