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

bool decode_dxt5_linear(std::span<const std::uint8_t> source, int width, int height,
                        std::vector<std::uint8_t>& output) {
    if (width <= 0 || height <= 0) {
        return false;
    }
    const std::size_t blocks_wide = static_cast<std::size_t>((width + 3) / 4);
    const std::size_t blocks_high = static_cast<std::size_t>((height + 3) / 4);
    const std::size_t row_bytes = blocks_wide * 16u;
    const std::size_t pitch = (row_bytes + 63u) & ~std::size_t{63u};
    if (blocks_high > source.size() / pitch) {
        return false;
    }
    std::vector<std::uint8_t> compact(row_bytes * blocks_high);
    for (std::size_t row = 0; row < blocks_high; ++row) {
        std::memcpy(compact.data() + row * row_bytes,
                    source.data() + row * pitch, row_bytes);
    }
    return decode_dxt5(compact, width, height, output);
}

bool decode_argb8_linear(std::span<const std::uint8_t> source, int width, int height,
                         bool has_alpha, std::vector<std::uint8_t>& output) {
    if (width <= 0 || height <= 0) {
        return false;
    }
    const std::size_t row_bytes = static_cast<std::size_t>(width) * 4u;
    const std::size_t pitch = (row_bytes + 63u) & ~std::size_t{63u};
    if (static_cast<std::size_t>(height) > source.size() / pitch) {
        return false;
    }
    output.assign(row_bytes * static_cast<std::size_t>(height), 0);
    for (int y = 0; y < height; ++y) {
        const std::uint8_t* row = source.data() + static_cast<std::size_t>(y) * pitch;
        std::uint8_t* destination = output.data() + static_cast<std::size_t>(y) * row_bytes;
        for (int x = 0; x < width; ++x) {
            destination[x * 4] = row[x * 4 + 2];
            destination[x * 4 + 1] = row[x * 4 + 1];
            destination[x * 4 + 2] = row[x * 4];
            destination[x * 4 + 3] = has_alpha ? row[x * 4 + 3] : 255;
        }
    }
    return true;
}

bool decode_argb8_swizzled(std::span<const std::uint8_t> source, int width, int height,
                           std::vector<std::uint8_t>& output) {
    if (width <= 0 || height <= 0 || (width & (width - 1)) != 0 ||
        (height & (height - 1)) != 0) {
        return false;
    }
    const std::size_t pixel_count = static_cast<std::size_t>(width) * height;
    if (pixel_count > source.size() / 4u) {
        return false;
    }
    output.assign(pixel_count * 4u, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::size_t swizzled = 0;
            unsigned int output_bit = 0;
            for (unsigned int bit = 0;
                 (1u << bit) < static_cast<unsigned int>(width) ||
                 (1u << bit) < static_cast<unsigned int>(height);
                 ++bit) {
                if ((1u << bit) < static_cast<unsigned int>(width)) {
                    swizzled |= static_cast<std::size_t>((x >> bit) & 1) << output_bit++;
                }
                if ((1u << bit) < static_cast<unsigned int>(height)) {
                    swizzled |= static_cast<std::size_t>((y >> bit) & 1) << output_bit++;
                }
            }
            const std::size_t source_offset = swizzled * 4u;
            const std::size_t destination =
                (static_cast<std::size_t>(y) * width + x) * 4u;
            output[destination] = source[source_offset + 2];
            output[destination + 1] = source[source_offset + 1];
            output[destination + 2] = source[source_offset];
            output[destination + 3] = source[source_offset + 3];
        }
    }
    return true;
}

}  // namespace h2::port
