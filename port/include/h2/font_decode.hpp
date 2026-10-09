#ifndef H2_PORT_FONT_DECODE_HPP
#define H2_PORT_FONT_DECODE_HPP

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace h2::port {

struct FontMetrics {
    std::int16_t ascending_height = 0;
    std::int16_t descending_height = 0;
    std::int16_t leading_height = 0;
    std::int16_t initial_offset = 0;
};

struct FontGlyph {
    std::int16_t advance = 0;
    std::int16_t width = 0;
    std::int16_t height = 0;
    std::int16_t x_offset = 0;
    std::int16_t y_offset = 0;
    std::vector<std::uint8_t> rgba;
};

bool decode_font_metrics(std::span<const std::uint8_t> file, FontMetrics& metrics,
                         std::string& error);
bool decode_font_glyph(std::span<const std::uint8_t> file, std::uint32_t codepoint,
                       FontGlyph& glyph, std::string& error);

}  // namespace h2::port

#endif
