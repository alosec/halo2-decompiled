#ifndef H2_PORT_TEXTURE_DECODE_HPP
#define H2_PORT_TEXTURE_DECODE_HPP

#include <cstdint>
#include <span>
#include <vector>

namespace h2::port {

bool decode_dxt5(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output);

}  // namespace h2::port

#endif
