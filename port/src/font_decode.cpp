#include "h2/font_decode.hpp"

#include <algorithm>
#include <limits>

namespace h2::port {
namespace {

constexpr std::size_t kFontHeaderOffset = 0x200;
constexpr std::uint32_t kFontHeaderVersion = 0xf0000001u;
constexpr std::size_t kCharacterOffsetTable = 0x400;
constexpr std::size_t kCharacterHeaderBias = 0x4040;
constexpr std::size_t kCharacterHeaderSize = 16;

std::uint16_t read_u16(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8);
}

std::int16_t read_i16(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::int16_t>(read_u16(bytes, offset));
}

std::uint32_t read_u32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

bool add_pixel(std::uint16_t pixel, std::size_t maximum,
               std::vector<std::uint16_t>& output, std::string& error) {
    if (output.size() >= maximum) {
        error = "font glyph RLE exceeds declared dimensions";
        return false;
    }
    output.push_back(pixel);
    return true;
}

bool decode_pixels(std::span<const std::uint8_t> source, std::size_t expected,
                   std::vector<std::uint16_t>& output, std::string& error) {
    output.clear();
    output.reserve(expected);
    std::uint16_t color = 0x0fff;
    for (std::size_t input = 0; input < source.size();) {
        const std::uint8_t code = source[input];
        const std::uint8_t type = code >> 6;
        if (type <= 1) {
            const unsigned run = code & 0x3f;
            if (run == 0 && type == 0) {
                if (source.size() - input < 3) {
                    error = "truncated font glyph color literal";
                    return false;
                }
                const std::uint16_t value = static_cast<std::uint16_t>(
                    (static_cast<std::uint16_t>(source[input + 1]) << 8) |
                    source[input + 2]);
                color = value & 0x0fff;
                if (!add_pixel(value, expected, output, error)) return false;
                input += 3;
                continue;
            }
            const std::uint16_t alpha = type == 0 ? 0 : 0xf000;
            for (unsigned i = 0; i < run; ++i) {
                if (!add_pixel(alpha | color, expected, output, error)) return false;
            }
            ++input;
            continue;
        }

        const auto alpha_pixel = [&](unsigned alpha) {
            return static_cast<std::uint16_t>(((alpha << 1) | (alpha & 1)) << 12) |
                   color;
        };
        if (type == 3) {
            if (!add_pixel(alpha_pixel((code >> 3) & 7), expected, output, error) ||
                !add_pixel(alpha_pixel(code & 7), expected, output, error)) {
                return false;
            }
            ++input;
            continue;
        }

        if (!add_pixel(alpha_pixel((code >> 3) & 7), expected, output, error)) return false;
        unsigned run = 0;
        std::uint16_t fill = color;
        switch (code & 7) {
            case 0: run = 0; break;
            case 1: run = 4; fill |= 0xf000; break;
            case 2: run = 3; fill |= 0xf000; break;
            case 3: run = 2; fill |= 0xf000; break;
            case 4: run = 5; break;
            case 5: run = 4; break;
            case 6: run = 3; break;
            case 7: run = 2; break;
        }
        for (unsigned i = 0; i < run; ++i) {
            if (!add_pixel(fill, expected, output, error)) return false;
        }
        ++input;
    }
    if (output.size() != expected) {
        error = "font glyph RLE pixel count does not match declared dimensions";
        return false;
    }
    return true;
}

}  // namespace

bool decode_font_metrics(std::span<const std::uint8_t> file, FontMetrics& metrics,
                         std::string& error) {
    if (file.size() < kFontHeaderOffset + 0x0c) {
        error = "font file is too small for its header";
        return false;
    }
    if (read_u32(file, kFontHeaderOffset) != kFontHeaderVersion) {
        error = "unsupported retail font header version";
        return false;
    }
    metrics.ascending_height = read_i16(file, kFontHeaderOffset + 4);
    metrics.descending_height = read_i16(file, kFontHeaderOffset + 6);
    metrics.leading_height = read_i16(file, kFontHeaderOffset + 8);
    metrics.initial_offset = read_i16(file, kFontHeaderOffset + 0x0a);
    return true;
}

bool decode_font_glyph(std::span<const std::uint8_t> file, std::uint32_t codepoint,
                       FontGlyph& glyph, std::string& error) {
    if (codepoint > 0xffff) {
        error = "font glyph codepoint is outside the retail offset table";
        return false;
    }
    const std::size_t table_offset = kCharacterOffsetTable + codepoint * sizeof(std::uint32_t);
    if (table_offset > file.size() || file.size() - table_offset < sizeof(std::uint32_t)) {
        error = "font file is too small for its character offset table";
        return false;
    }
    const std::int32_t header_index = static_cast<std::int32_t>(read_u32(file, table_offset));
    if (header_index < 0) {
        error = "font glyph is absent";
        return false;
    }
    const std::uint64_t header_offset_64 =
        (static_cast<std::uint64_t>(header_index) + kCharacterHeaderBias) *
        kCharacterHeaderSize;
    if (header_offset_64 > std::numeric_limits<std::size_t>::max()) {
        error = "font glyph header offset overflows the host address space";
        return false;
    }
    const std::size_t header_offset = static_cast<std::size_t>(header_offset_64);
    if (header_offset > file.size() || file.size() - header_offset < kCharacterHeaderSize) {
        error = "font glyph header lies outside the file";
        return false;
    }

    const std::uint16_t pixels_size = read_u16(file, header_offset + 2);
    glyph.advance = read_i16(file, header_offset);
    glyph.width = read_i16(file, header_offset + 4);
    glyph.height = read_i16(file, header_offset + 6);
    glyph.x_offset = read_i16(file, header_offset + 8);
    glyph.y_offset = read_i16(file, header_offset + 10);
    const std::uint32_t pixels_offset = read_u32(file, header_offset + 12);
    if (glyph.width < 0 || glyph.height < 0 || glyph.width > 4096 || glyph.height > 4096) {
        error = "font glyph has invalid dimensions";
        return false;
    }
    const std::size_t pixel_count = static_cast<std::size_t>(glyph.width) * glyph.height;
    if (pixels_offset > file.size() || file.size() - pixels_offset < pixels_size) {
        error = "font glyph pixels lie outside the file";
        return false;
    }
    std::vector<std::uint16_t> pixels;
    if (!decode_pixels(file.subspan(pixels_offset, pixels_size), pixel_count, pixels, error)) {
        return false;
    }
    glyph.rgba.resize(pixel_count * 4);
    for (std::size_t i = 0; i < pixels.size(); ++i) {
        const std::uint16_t pixel = pixels[i];
        glyph.rgba[i * 4 + 0] = static_cast<std::uint8_t>(((pixel >> 8) & 0x0f) * 17);
        glyph.rgba[i * 4 + 1] = static_cast<std::uint8_t>(((pixel >> 4) & 0x0f) * 17);
        glyph.rgba[i * 4 + 2] = static_cast<std::uint8_t>((pixel & 0x0f) * 17);
        glyph.rgba[i * 4 + 3] = static_cast<std::uint8_t>(((pixel >> 12) & 0x0f) * 17);
    }
    return true;
}

}  // namespace h2::port
