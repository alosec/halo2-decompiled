/* The game session's mixer and the host_h2_audio_* imports behind it.
 *
 * open_session makes the mixer that host_h2_audio_command and
 * host_h2_audio_status (h2/host_abi.h) serve. With a device it plays through
 * SDL3's default playback device (48 kHz stereo float, the device's thread
 * pulling from the mixer); without one (headless, H2_AUDIO=off, or when no
 * device opens) the mixer runs on a real-time clock that every import call
 * advances, so the game still sees playback progress and nothing is heard.
 * H2_TRACE=audio logs mixer statistics every few seconds. */
#pragma once

#include "h2/host_abi.h"  /* host_h2_audio_command, host_h2_audio_status */

#include <string>

namespace h2::port {
class GuestArena;
}

namespace h2::audio {

bool open_session(const h2::port::GuestArena& arena, bool device, std::string& error);
void close_session();

}  // namespace h2::audio
