#include "h2/font_decode.hpp"
#include "h2/guest_arena.hpp"
#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/render_protocol.h"
#include "h2/texture_decode.hpp"

struct color4f { float alpha, red, green, blue; };
struct short_rectangle2d { short top, left, bottom, right; };

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

extern "C" void D3DDevice_Begin(unsigned int primitive_type);
extern "C" void D3DDevice_End();
extern "C" void D3DDevice_SetVertexData2f(unsigned int vertex_register, float x, float y);
extern "C" void D3DDevice_SetVertexData4f(unsigned int vertex_register, float x, float y,
                                            float z, float w);
extern "C" void D3DDevice_SetVertexDataColor(unsigned int vertex_register,
                                               unsigned int color);
extern "C" void h2_render_preview_quad(long bitmap_tag, long bitmap_index,
                                         long layer, const void* vertices);
void function_52040(const void* request, const void* vertices);
void function_235d69(short_rectangle2d const* rectangle, float depth,
                     short_rectangle2d const* screen, color4f const* color);

namespace {

struct PreviewVertex {
    float x;
    float y;
    float z;
    float w;
    float texture_x;
    float texture_y;
    unsigned int color;
};
static_assert(sizeof(PreviewVertex) == 28);

struct InterfaceRequest {
    unsigned int flags;
    unsigned char unknown04[8];
    unsigned int bitmap_address;
};
static_assert(sizeof(InterfaceRequest) == 16);

struct InterfaceVertex {
    float position_x;
    float position_y;
    float texture_x;
    float texture_y;
    unsigned int color;
};
static_assert(sizeof(InterfaceVertex) == 20);

struct CallbackCapture {
    int command = 0;
    const void* request = nullptr;
    const void* vertices = nullptr;
    const std::uint32_t* text = nullptr;
    const void* bounds = nullptr;
    int line_gap = 0;
    float scale = 0.0f;
    std::uint32_t color = 0;
    const void* text_metadata = nullptr;
    int video_calls = 0;
    int render_calls = 0;
    int text_calls = 0;
    bool decode_immediate = false;
    h2_render_immediate_request immediate{};
    h2_render_immediate_vertex immediate_vertices[H2_RENDER_IMMEDIATE_MAX_VERTICES]{};
};

void capture_video(int command, void* context) {
    auto& capture = *static_cast<CallbackCapture*>(context);
    capture.command = command;
    ++capture.video_calls;
}

void capture_render(const void* request, const void* vertices, void* context) {
    auto& capture = *static_cast<CallbackCapture*>(context);
    capture.request = request;
    capture.vertices = vertices;
    ++capture.render_calls;
    if (capture.decode_immediate) {
        capture.immediate = *static_cast<const h2_render_immediate_request*>(request);
        const auto* source = static_cast<const h2_render_immediate_vertex*>(vertices);
        for (unsigned int i = 0; i < capture.immediate.vertex_count &&
                                 i < H2_RENDER_IMMEDIATE_MAX_VERTICES; ++i) {
            capture.immediate_vertices[i] = source[i];
        }
    }
}

void capture_text(const std::uint32_t* text, const void* bounds, const void*,
                  const void*, int line_gap, float scale, std::uint32_t color,
                  const void* metadata, void* context) {
    auto& capture = *static_cast<CallbackCapture*>(context);
    capture.text = text;
    capture.bounds = bounds;
    capture.line_gap = line_gap;
    capture.scale = scale;
    capture.color = color;
    capture.text_metadata = metadata;
    ++capture.text_calls;
}

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
    }
    return condition;
}

}  // namespace

int main() {
    bool ok = true;
    ok &= check(host_h2_abi_version() == H2_HOST_ABI_VERSION,
                "host ABI version mismatch");
    ok &= check(std::abs(host_h2_cos(0.5) - std::cos(0.5)) < 1.0e-12,
                "host cosine adapter mismatch");

    const std::vector<std::uint8_t> red_dxt5 = {
        255, 0, 0, 0, 0, 0, 0, 0,
        0x00, 0xf8, 0x00, 0x00, 0, 0, 0, 0,
    };
    std::vector<std::uint8_t> decoded;
    ok &= check(h2::port::decode_dxt5(red_dxt5, 4, 4, decoded) &&
                    decoded.size() == 64 && decoded[0] == 255 && decoded[1] == 0 &&
                    decoded[2] == 0 && decoded[3] == 255 && decoded[63] == 255,
                "DXT5 decoder did not produce the expected opaque red block");
    ok &= check(!h2::port::decode_dxt5(
                    std::span<const std::uint8_t>(red_dxt5.data(), 15), 4, 4, decoded),
                "DXT5 decoder accepted a truncated block");
    std::vector<std::uint8_t> linear_dxt5(2 * 128);
    for (std::size_t row = 0; row < 2; ++row) {
        for (std::size_t block = 0; block < 5; ++block) {
            std::copy(red_dxt5.begin(), red_dxt5.end(),
                      linear_dxt5.begin() + static_cast<std::ptrdiff_t>(
                          row * 128 + block * 16));
        }
    }
    std::vector<std::uint8_t> blue_dxt5 = red_dxt5;
    blue_dxt5[8] = 0x1f;
    blue_dxt5[9] = 0x00;
    std::copy(blue_dxt5.begin(), blue_dxt5.end(), linear_dxt5.begin() + 128);
    ok &= check(h2::port::decode_dxt5_linear(linear_dxt5, 20, 8, decoded) &&
                    decoded[(4 * 20) * 4] == 0 && decoded[(4 * 20) * 4 + 2] == 255 &&
                    decoded[(16 * 4)] == 255 && decoded[(16 * 4) + 2] == 0,
                "linear DXT5 decoder did not remove 64-byte row padding");
    ok &= check(!h2::port::decode_dxt5_linear(
                    std::span<const std::uint8_t>(linear_dxt5.data(), 255),
                    20, 8, decoded),
                "linear DXT5 decoder accepted a truncated padded surface");

    std::vector<std::uint8_t> linear_argb(128, 0);
    linear_argb[64] = 3;
    linear_argb[65] = 2;
    linear_argb[66] = 1;
    linear_argb[67] = 4;
    ok &= check(h2::port::decode_argb8_linear(linear_argb, 3, 2, true, decoded) &&
                    decoded[12] == 1 && decoded[13] == 2 &&
                    decoded[14] == 3 && decoded[15] == 4,
                "linear ARGB8 decoder did not remove 64-byte BGRA row padding");
    ok &= check(h2::port::decode_argb8_linear(linear_argb, 3, 2, false, decoded) &&
                    decoded[15] == 255,
                "linear XRGB8 decoder did not force opaque alpha");
    ok &= check(!h2::port::decode_argb8_linear(
                    std::span<const std::uint8_t>(linear_argb.data(), 127),
                    3, 2, true, decoded),
                "linear ARGB8 decoder accepted a truncated padded surface");

    std::vector<std::uint8_t> swizzled_argb(64, 0);
    const std::size_t swizzled_offset = 6 * 4;
    swizzled_argb[swizzled_offset] = 3;
    swizzled_argb[swizzled_offset + 1] = 2;
    swizzled_argb[swizzled_offset + 2] = 1;
    swizzled_argb[swizzled_offset + 3] = 4;
    ok &= check(h2::port::decode_argb8_swizzled(swizzled_argb, 4, 4, decoded) &&
                    decoded[24] == 1 && decoded[25] == 2 &&
                    decoded[26] == 3 && decoded[27] == 4,
                "swizzled ARGB8 decoder did not restore Morton-order BGRA pixels");
    ok &= check(!h2::port::decode_argb8_swizzled(
                    std::span<const std::uint8_t>(swizzled_argb.data(), 63),
                    4, 4, decoded),
                "swizzled ARGB8 decoder accepted a truncated surface");
    ok &= check(!h2::port::decode_argb8_swizzled(swizzled_argb, 3, 4, decoded),
                "swizzled ARGB8 decoder accepted a non-power-of-two surface");

    std::vector<std::uint8_t> font_file(0x40411, 0);
    std::fill(font_file.begin() + 0x400, font_file.begin() + 0x40400, 0xff);
    font_file[0x200] = 1;
    font_file[0x203] = 0xf0;
    font_file[0x204] = 9;
    font_file[0x206] = 3;
    font_file[0x208] = 1;
    const std::size_t a_offset = 0x400 + static_cast<std::size_t>('A') * 4;
    font_file[a_offset + 0] = 0;
    font_file[a_offset + 1] = 0;
    font_file[a_offset + 2] = 0;
    font_file[a_offset + 3] = 0;
    font_file[0x40400] = 3;
    font_file[0x40402] = 1;
    font_file[0x40404] = 2;
    font_file[0x40406] = 2;
    font_file[0x40408] = 1;
    font_file[0x4040a] = 2;
    font_file[0x4040c] = 0x10;
    font_file[0x4040d] = 0x04;
    font_file[0x4040e] = 0x04;
    font_file[0x40410] = 0x44;
    h2::port::FontMetrics font_metrics;
    h2::port::FontGlyph font_glyph;
    std::string font_error;
    ok &= check(h2::port::decode_font_metrics(font_file, font_metrics, font_error) &&
                    font_metrics.ascending_height == 9 &&
                    font_metrics.descending_height == 3 && font_metrics.leading_height == 1,
                "retail font metrics decoder mismatch");
    ok &= check(h2::port::decode_font_glyph(font_file, 'A', font_glyph, font_error) &&
                    font_glyph.advance == 3 && font_glyph.width == 2 &&
                    font_glyph.height == 2 && font_glyph.x_offset == 1 &&
                    font_glyph.y_offset == 2 && font_glyph.rgba.size() == 16 &&
                    font_glyph.rgba[0] == 255 && font_glyph.rgba[3] == 255,
                "retail font glyph decoder mismatch");
    font_error.clear();
    ok &= check(!h2::port::decode_font_glyph(font_file, 'B', font_glyph, font_error),
                "retail font decoder accepted an absent glyph");
    font_file[0x200] = 2;
    font_error.clear();
    ok &= check(!h2::port::decode_font_metrics(font_file, font_metrics, font_error),
                "retail font decoder accepted an unsupported header version");
    font_file[0x200] = 1;
    font_file[0x40410] = 0x45;
    font_error.clear();
    ok &= check(!h2::port::decode_font_glyph(font_file, 'A', font_glyph, font_error),
                "retail font decoder accepted an overflowing RLE run");

    const unsigned int before = host_h2_get_milliseconds();
    host_h2_sleep(2);
    const unsigned int after = host_h2_get_milliseconds();
    ok &= check(after - before >= 1, "host monotonic clock did not advance");

    std::string arena_error;
    auto arena = h2::port::GuestArena::create(arena_error);
    ok &= check(arena != nullptr, "could not create guest arena for cache services");
    if (arena) {
        std::vector<unsigned char> cache_bytes(0x1000);
        for (std::size_t i = 0; i < cache_bytes.size(); ++i) {
            cache_bytes[i] = static_cast<unsigned char>((i * 29u + 7u) & 0xffu);
        }
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto cache_root = std::filesystem::temp_directory_path() /
            ("h2-host-cache-" + std::to_string(unique));
        const auto cache_path = cache_root / "mainmenu.map";
        const auto campaign_path = cache_root / "00a_introduction.map";
        std::filesystem::create_directory(cache_root);
        {
            std::ofstream output(cache_path, std::ios::binary);
            output.write(reinterpret_cast<const char*>(cache_bytes.data()),
                         static_cast<std::streamsize>(cache_bytes.size()));
        }
        auto campaign_bytes = cache_bytes;
        const auto write_u32 = [&](std::size_t offset, std::uint32_t value) {
            campaign_bytes[offset] = static_cast<unsigned char>(value);
            campaign_bytes[offset + 1] = static_cast<unsigned char>(value >> 8);
            campaign_bytes[offset + 2] = static_cast<unsigned char>(value >> 16);
            campaign_bytes[offset + 3] = static_cast<unsigned char>(value >> 24);
        };
        write_u32(0, 0x68656164u);
        write_u32(4, 8u);
        write_u32(8, static_cast<std::uint32_t>(campaign_bytes.size()));
        write_u32(0x140, 0u);
        write_u32(0x7fc, 0x666f6f74u);
        std::memset(campaign_bytes.data() + 0x120, 0, 0x20);
        std::memcpy(campaign_bytes.data() + 0x120, "02.09.27.09809", 14);
        std::memset(campaign_bytes.data() + 0x198, 0, 0x20);
        std::memcpy(campaign_bytes.data() + 0x198, "00a_introduction", 16);
        {
            std::ofstream output(campaign_path, std::ios::binary);
            output.write(reinterpret_cast<const char*>(campaign_bytes.data()),
                         static_cast<std::streamsize>(campaign_bytes.size()));
        }
        const bool configured =
            h2::port::configure_cache_services(*arena, cache_path, arena_error);
        ok &= check(configured, "could not configure native cache services");
        if (configured) {
            ok &= check(*static_cast<const unsigned char*>(
                            arena->pointer_const(0x00557c94u)) == cache_bytes[0],
                        "cache header was not staged at the recovered guest address");
            void* const allocation = host_h2_physical_memory_malloc_low(0x1000, 0);
            ok &= check(reinterpret_cast<std::uintptr_t>(allocation) == 0x80061000u,
                        "physical cache allocation did not return the expected guest address");
            void* const native_buffer = arena->pointer(0x80061000u);
            ok &= check(host_h2_cache_read(0x800u, 16, native_buffer) == 1,
                        "native cache read rejected a valid guest destination");
            ok &= check(*static_cast<const unsigned char*>(native_buffer) ==
                            cache_bytes[0x800],
                        "native cache read copied incorrect data");
            ok &= check(host_h2_cache_read(0xff8u, 16, native_buffer) == 0,
                        "native cache read accepted an out-of-range request");
            unsigned char host_copy[16]{};
            ok &= check(h2::port::copy_cache_bytes(0x800u, host_copy,
                                                   sizeof(host_copy)) &&
                            host_copy[0] == cache_bytes[0x800],
                        "native cache ownership did not expose bounded host bytes");
            std::strcpy(static_cast<char*>(native_buffer),
                        "scenarios\\solo\\00a_introduction\\00a_introduction");
            ok &= check(host_h2_map_preflight(static_cast<char*>(native_buffer)) == 1,
                        "campaign cache preflight rejected a valid recovered path");
            write_u32(0x140, 1u);
            {
                std::ofstream output(campaign_path, std::ios::binary | std::ios::trunc);
                output.write(reinterpret_cast<const char*>(campaign_bytes.data()),
                             static_cast<std::streamsize>(campaign_bytes.size()));
            }
            ok &= check(host_h2_map_preflight(static_cast<char*>(native_buffer)) == 0,
                        "campaign cache preflight accepted a non-Campaign header");
            std::strcpy(static_cast<char*>(native_buffer), "../00a_introduction");
            ok &= check(host_h2_map_preflight(static_cast<char*>(native_buffer)) == 0,
                        "campaign cache preflight accepted path traversal");
            std::strcpy(static_cast<char*>(native_buffer), "missing_map");
            ok &= check(host_h2_map_preflight(static_cast<char*>(native_buffer)) == 0,
                        "campaign cache preflight accepted an absent map");
            std::memset(native_buffer, 'a', 0x100);
            ok &= check(host_h2_map_preflight(static_cast<char*>(native_buffer)) == 0,
                        "campaign cache preflight accepted an unterminated name");
            ok &= check(host_h2_map_preflight("00a_introduction") == 0,
                        "campaign cache preflight accepted a host pointer");
            h2::port::reset_cache_services();
        }
        std::filesystem::remove_all(cache_root);
    }

    CallbackCapture capture;
    h2::port::reset_host_service_stats();
    h2::port::set_video_control_handler(capture_video, &capture);
    h2::port::set_render_request_handler(capture_render, &capture);
    h2::port::set_text_request_handler(capture_text, &capture);
    const auto* request = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x1234));
    const auto* vertices = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x5678));
    const std::uint32_t text[] = {'H', '2', 0};
    const auto* bounds = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x9abc));
    const h2_text_request_metadata text_metadata{
        H2_TEXT_PROTOCOL_VERSION, 2, 1, 2, 3, 0x80112233u};
    host_h2_video_control(7);
    host_h2_render_request(request, vertices);
    host_h2_text_request(text, bounds, nullptr, nullptr, 3, 1.5f, 0xff00ff00u,
                         &text_metadata);
    const auto stats = h2::port::host_service_stats();
    ok &= check(capture.command == 7 && capture.video_calls == 1,
                "video-control callback did not receive the guest command");
    ok &= check(capture.request == request && capture.vertices == vertices &&
                    capture.render_calls == 1,
                "render callback did not receive the guest pointers");
    ok &= check(capture.text == text && capture.bounds == bounds &&
                    capture.line_gap == 3 && capture.scale == 1.5f &&
                    capture.color == 0xff00ff00u &&
                    capture.text_metadata == &text_metadata && capture.text_calls == 1,
                "text callback did not receive the guest text request");
    ok &= check(stats.video_control_calls == 1 && stats.render_request_calls == 1 &&
                    stats.text_request_calls == 1,
                "host service counters are incorrect");

    capture.decode_immediate = true;
    D3DDevice_Begin(7);
    D3DDevice_SetVertexDataColor(9, 0xff336699u);
    D3DDevice_SetVertexData2f(3, 0.25f, 0.5f);
    D3DDevice_SetVertexData4f(0, -1.0f, 2.0f, 3.0f, 1.0f);
    D3DDevice_SetVertexData2f(3, 0.75f, 1.0f);
    D3DDevice_SetVertexData4f(0, 4.0f, 5.0f, 6.0f, 1.0f);
    D3DDevice_End();
    const auto immediate_stats = h2::port::host_service_stats();
    ok &= check(capture.immediate.kind == H2_RENDER_REQUEST_IMMEDIATE &&
                    capture.immediate.version == H2_RENDER_PROTOCOL_VERSION &&
                    capture.immediate.primitive_type == 7 &&
                    capture.immediate.vertex_count == 2 &&
                    capture.immediate.color == 0xff336699u,
                "immediate render request state is incorrect");
    ok &= check(capture.immediate_vertices[0].texture_x == 0.25f &&
                    capture.immediate_vertices[0].texture_y == 0.5f &&
                    capture.immediate_vertices[0].x == -1.0f &&
                    capture.immediate_vertices[0].w == 1.0f &&
                    capture.immediate_vertices[1].texture_x == 0.75f &&
                    capture.immediate_vertices[1].texture_y == 1.0f &&
                    capture.immediate_vertices[1].x == 4.0f &&
                    capture.immediate_vertices[1].z == 6.0f,
                "immediate render vertices are incorrect");
    ok &= check(capture.render_calls == 2 && immediate_stats.render_request_calls == 2,
                "immediate render request did not cross the host boundary once");

    const PreviewVertex preview[] = {
        {-2.0f, -1.0f, 7.0f, 1.0f, 0.0f, 0.0f, 0xffffffffu},
        { 2.0f, -1.0f, 7.0f, 1.0f, 1.0f, 0.0f, 0xffffffffu},
        { 2.0f,  1.0f, 7.0f, 1.0f, 1.0f, 1.0f, 0xffffffffu},
        {-2.0f,  1.0f, 7.0f, 1.0f, 0.0f, 1.0f, 0xffffffffu},
    };
    h2_render_preview_quad(0x1234, 3, 20, preview);
    const auto preview_stats = h2::port::host_service_stats();
    ok &= check(capture.immediate.kind == H2_RENDER_REQUEST_PREVIEW_QUAD &&
                    capture.immediate.primitive_type == 7 &&
                    capture.immediate.vertex_count == 4 &&
                    capture.immediate.texture_tag == 0x1234 &&
                    capture.immediate.texture_index == 3 &&
                    capture.immediate.layer == 20,
                "preview render request metadata is incorrect");
    ok &= check(capture.immediate_vertices[0].x == -2.0f &&
                    capture.immediate_vertices[0].texture_x == 0.0f &&
                    capture.immediate_vertices[2].y == 1.0f &&
                    capture.immediate_vertices[2].texture_y == 1.0f,
                "preview render vertices are incorrect");
    ok &= check(capture.render_calls == 3 && preview_stats.render_request_calls == 3,
                "preview render request did not cross the host boundary once");

    const InterfaceVertex interface_quad[] = {
        {10.0f, 20.0f, 0.0f, 0.0f, 0xff112233u},
        {30.0f, 20.0f, 1.0f, 0.0f, 0xff112233u},
        {30.0f, 40.0f, 1.0f, 1.0f, 0xff112233u},
        {10.0f, 40.0f, 0.0f, 1.0f, 0xff112233u},
    };
    const InterfaceRequest interface_request{0, {}, 0x12345678u};
    function_52040(&interface_request, interface_quad);
    const auto interface_stats = h2::port::host_service_stats();
    ok &= check(capture.immediate.kind == H2_RENDER_REQUEST_INTERFACE_QUAD &&
                    capture.immediate.vertex_count == 4 &&
                    capture.immediate.color == 0xff112233u &&
                    capture.immediate.texture_tag == 0x12345678,
                "interface quad request metadata is incorrect");
    ok &= check(capture.immediate_vertices[0].x == 10.0f &&
                    capture.immediate_vertices[0].y == 20.0f &&
                    capture.immediate_vertices[2].texture_x == 1.0f &&
                    capture.immediate_vertices[2].texture_y == 1.0f,
                "interface quad vertices are incorrect");
    ok &= check(capture.render_calls == 4 && interface_stats.render_request_calls == 4,
                "interface quad did not cross the host boundary once");

    const short_rectangle2d solid_rect{10, 20, 30, 40};
    const short_rectangle2d solid_screen{0, 0, 480, 640};
    const color4f solid_color{0.5f, 1.0f, 0.25f, 0.0f};
    function_235d69(&solid_rect, 0.75f, &solid_screen, &solid_color);
    const auto solid_stats = h2::port::host_service_stats();
    ok &= check(capture.immediate.kind == H2_RENDER_REQUEST_IMMEDIATE &&
                    capture.immediate.primitive_type == 7 &&
                    capture.immediate.vertex_count == 4 &&
                    capture.immediate.color == 0x80ff4000u,
                "solid rectangle request metadata is incorrect");
    ok &= check(capture.immediate_vertices[0].x == 20.0f &&
                    capture.immediate_vertices[0].y == 10.0f &&
                    capture.immediate_vertices[1].x == 20.0f &&
                    capture.immediate_vertices[1].y == 30.0f &&
                    capture.immediate_vertices[2].x == 40.0f &&
                    capture.immediate_vertices[3].y == 10.0f,
                "solid rectangle vertices are incorrect");
    ok &= check(capture.render_calls == 5 && solid_stats.render_request_calls == 5,
                "solid rectangle did not cross the host boundary once");

    h2::port::set_video_control_handler(nullptr);
    h2::port::set_render_request_handler(nullptr);
    h2::port::set_text_request_handler(nullptr);
    if (ok) {
        std::cout << "native host ABI services passed\n";
    }
    return ok ? 0 : 1;
}
