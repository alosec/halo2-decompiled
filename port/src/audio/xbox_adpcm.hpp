/* Xbox ADPCM (WAVE_FORMAT 0x0069) block decoding.
 *
 * Xbox ADPCM is IMA ADPCM in fixed blocks of 36 bytes per channel. Each block
 * holds, per channel, a 4-byte header (the starting predictor as a signed
 * 16-bit little-endian sample, the starting step-table index, one reserved
 * byte), then 32 bytes of 4-bit codes, interleaved between channels in 4-byte
 * groups (8 codes) as in the Microsoft IMA ADPCM WAVE layout, low nibble
 * first. A block decodes to 64 samples per channel (the wSamplesPerBlock the
 * game's stream formats declare, src/sound_driver_stream_format_native.cpp);
 * the header sample seeds the predictor.
 *
 * Sources (public format descriptions only):
 *  - IMA Digital Audio Focus and Technical Working Groups, "Recommended
 *    Practices for Enhancing Digital Audio Compatibility in Multimedia
 *    Systems", rev. 3.00, 1992: the step and index tables and the nibble
 *    reconstruction.
 *  - Microsoft, "Multimedia Standards Update" (1994), IMA ADPCM WAVE type:
 *    the block header and channel interleave.
 *  - MultimediaWiki, "Xbox ADPCM": 36-byte blocks per channel, 64 samples. */
#pragma once

#include <cstddef>
#include <cstdint>

namespace h2::audio {

constexpr std::uint32_t k_xbox_adpcm_block_bytes = 36;   /* per channel */
constexpr std::uint32_t k_xbox_adpcm_block_samples = 64; /* per channel */

/* Decodes one block of channels * 36 bytes into 64 frames of channels
 * interleaved 16-bit samples. channels is 1 or 2 (any count up to 8 works). */
void xbox_adpcm_decode_block(const std::uint8_t* block, unsigned channels,
                             std::int16_t* frames);

/* One IMA ADPCM code applied to a predictor and step index (exposed for tests). */
std::int16_t ima_adpcm_step(std::uint8_t code, int& predictor, int& index);

}  // namespace h2::audio
