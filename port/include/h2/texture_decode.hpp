#ifndef H2_PORT_TEXTURE_DECODE_HPP
#define H2_PORT_TEXTURE_DECODE_HPP

#include <cstdint>
#include <span>
#include <vector>

namespace h2::port {

bool decode_dxt5(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output);
bool decode_dxt5_linear(std::span<const std::uint8_t> source, int width, int height,
                        std::vector<std::uint8_t>& output);
bool decode_argb8_linear(std::span<const std::uint8_t> source, int width, int height,
                         bool has_alpha, std::vector<std::uint8_t>& output);
bool decode_argb8_swizzled(std::span<const std::uint8_t> source, int width, int height,
                           std::vector<std::uint8_t>& output);

}  // namespace h2::port

#endif
