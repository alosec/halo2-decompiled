#ifndef H2_PORT_TEXTURE_DECODE_HPP
#define H2_PORT_TEXTURE_DECODE_HPP

#include <cstdint>
#include <span>
#include <vector>

namespace h2::port {

bool decode_dxt5(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output);
/* DXT1 (BC1, 1-bit alpha when colour0 <= colour1) and DXT3 (BC2, explicit
   4-bit alpha) blocks in row-major block order to RGBA8. */
bool decode_dxt1(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output);
bool decode_dxt3(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output);
bool decode_dxt5_linear(std::span<const std::uint8_t> source, int width, int height,
                        std::vector<std::uint8_t>& output);
bool decode_argb8_linear(std::span<const std::uint8_t> source, int width, int height,
                         bool has_alpha, std::vector<std::uint8_t>& output);
bool decode_argb8_swizzled(std::span<const std::uint8_t> source, int width, int height,
                           std::vector<std::uint8_t>& output);

}  // namespace h2::port

#endif
