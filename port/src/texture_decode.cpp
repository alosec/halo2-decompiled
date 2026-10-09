#include "h2/texture_decode.hpp"

#include <array>
#include <cstring>

namespace {

std::uint16_t read_u16(const std::uint8_t* data) {
    std::uint16_t value = 0;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

std::uint32_t read_u32(const std::uint8_t* data) {
    std::uint32_t value = 0;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

std::array<std::uint8_t, 4> unpack_565(std::uint16_t value) {
    return {
        static_cast<std::uint8_t>(((value >> 11) & 31u) * 255u / 31u),
        static_cast<std::uint8_t>(((value >> 5) & 63u) * 255u / 63u),
        static_cast<std::uint8_t>((value & 31u) * 255u / 31u),
        255,
    };
}

}  // namespace

namespace h2::port {

bool decode_dxt5(std::span<const std::uint8_t> source, int width, int height,
                 std::vector<std::uint8_t>& output) {
    if (width <= 0 || height <= 0) {
        return false;
    }
    const int blocks_wide = (width + 3) / 4;
    const int blocks_high = (height + 3) / 4;
    const std::size_t required =
        static_cast<std::size_t>(blocks_wide) * blocks_high * 16u;
    if (source.size() < required) {
        return false;
    }
    output.assign(static_cast<std::size_t>(width) * height * 4u, 0);
    const std::uint8_t* block = source.data();
    for (int by = 0; by < blocks_high; ++by) {
        for (int bx = 0; bx < blocks_wide; ++bx, block += 16) {
            std::uint8_t alpha[8] = {block[0], block[1]};
            if (alpha[0] > alpha[1]) {
                for (int i = 1; i <= 6; ++i) {
                    alpha[i + 1] = static_cast<std::uint8_t>(
                        ((7 - i) * alpha[0] + i * alpha[1]) / 7);
                }
            } else {
                for (int i = 1; i <= 4; ++i) {
                    alpha[i + 1] = static_cast<std::uint8_t>(
                        ((5 - i) * alpha[0] + i * alpha[1]) / 5);
                }
                alpha[6] = 0;
                alpha[7] = 255;
            }
            std::uint64_t alpha_bits = 0;
            for (int i = 0; i < 6; ++i) {
                alpha_bits |= static_cast<std::uint64_t>(block[2 + i]) << (8 * i);
            }
            std::array<std::array<std::uint8_t, 4>, 4> colors{};
            colors[0] = unpack_565(read_u16(block + 8));
            colors[1] = unpack_565(read_u16(block + 10));
            for (int channel = 0; channel < 3; ++channel) {
                colors[2][channel] = static_cast<std::uint8_t>(
                    (2u * colors[0][channel] + colors[1][channel]) / 3u);
                colors[3][channel] = static_cast<std::uint8_t>(
                    (colors[0][channel] + 2u * colors[1][channel]) / 3u);
            }
            colors[2][3] = colors[3][3] = 255;
            const std::uint32_t color_bits = read_u32(block + 12);
            for (int pixel = 0; pixel < 16; ++pixel) {
                const int x = bx * 4 + pixel % 4;
                const int y = by * 4 + pixel / 4;
                if (x >= width || y >= height) {
                    continue;
                }
                const auto& color = colors[(color_bits >> (2 * pixel)) & 3u];
                const std::size_t destination =
                    (static_cast<std::size_t>(y) * width + x) * 4u;
                output[destination] = color[0];
                output[destination + 1] = color[1];
                output[destination + 2] = color[2];
                output[destination + 3] =
                    alpha[(alpha_bits >> (3 * pixel)) & 7u];
            }
        }
    }
    return true;
}

}  // namespace h2::port
