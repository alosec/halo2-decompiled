#include <unistd.h>
#include <sys/ucontext.h>
#include <signal.h>
#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <atomic>
#include <sstream>
#include <csignal>
#include <unistd.h>
#include "h2/font_decode.hpp"
#include "h2/xbox_system.hpp"
#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"
#include "h2/render_protocol.h"
#include "h2/retail_data.hpp"
#include "h2/texture_decode.hpp"
#include "gpu/metal_renderer.hpp"
#include "audio/audio_host.hpp"

#include <array>
#include <charconv>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

/* the "trace" control line: the renderer traces the next frame */
extern std::atomic<bool> g_trace_next_frame;

namespace {

constexpr std::uint32_t kGuestStackAddress = 0x20000000u;
constexpr std::uint32_t kGuestStackSize = 0x00100000u;
constexpr std::uint32_t kAsyncControllerTimeoutMilliseconds = 5000u;

using GuestEntry = int (*)();

class AsyncTextureControllerGuard {
public:
    explicit AsyncTextureControllerGuard(
        h2::port::AsyncTextureGuestEntries entries)
        : entries_(entries) {}

    ~AsyncTextureControllerGuard() {
        if (!active_)
            return;
        std::string error;
        if (!h2::port::stop_async_texture_controller(
                entries_, kAsyncControllerTimeoutMilliseconds, error)) {
            std::cerr << "error: async texture controller teardown failed: "
                      << error << '\n';
        }
    }

    void activate() { active_ = true; }

    bool stop(std::string& error) {
        if (!active_)
            return true;
        if (!h2::port::stop_async_texture_controller(
                entries_, kAsyncControllerTimeoutMilliseconds, error)) {
            return false;
        }
        active_ = false;
        return true;
    }

private:
    h2::port::AsyncTextureGuestEntries entries_{};
    bool active_ = false;
};

struct CachedFontGlyph {
    h2::port::FontGlyph metrics;
    SDL_Texture* texture = nullptr;
};

struct CachedFont {
    bool attempted = false;
    std::vector<std::uint8_t> file;
    h2::port::FontMetrics metrics;
    std::unordered_map<std::uint32_t, CachedFontGlyph> glyphs;
};

struct NativeRenderContext {
    SDL_Renderer* renderer = nullptr;
    const h2::port::GuestArena* arena = nullptr;
    std::filesystem::path font_root;
    std::unordered_map<std::uint32_t, SDL_Texture*> textures;
    std::array<CachedFont, 11> fonts;
    bool failed = false;
};

std::uint16_t read_u16(const unsigned char* data) {
    std::uint16_t value = 0;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

std::uint32_t read_u32(const unsigned char* data) {
    std::uint32_t value = 0;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

bool get_interface_texture_size(const NativeRenderContext& context,
                                std::uint32_t guest_address, int& width, int& height) {
    if (!context.arena || guest_address < 0x80060000u || guest_address > 0x80233f8cu) {
        return false;
    }
    const auto* descriptor = static_cast<const unsigned char*>(
        context.arena->pointer_const(guest_address));
    width = read_u16(descriptor + 4);
    height = read_u16(descriptor + 6);
    return width > 0 && height > 0 && width <= 4096 && height <= 4096;
}

SDL_Texture* get_interface_texture(NativeRenderContext& context,
                                   std::uint32_t guest_address) {
    if (!guest_address) {
        return nullptr;
    }
    if (const auto found = context.textures.find(guest_address);
        found != context.textures.end()) {
        return found->second;
    }
    SDL_Texture* texture = nullptr;
    if (context.arena && guest_address >= 0x80060000u && guest_address <= 0x80233f8cu) {
        const auto* descriptor = static_cast<const unsigned char*>(
            context.arena->pointer_const(guest_address));
        const int width = read_u16(descriptor + 4);
        const int height = read_u16(descriptor + 6);
        const unsigned int source_format = read_u16(descriptor + 0x0c);
        const unsigned int cache_format = descriptor[0x17];
        const std::uint32_t data_offset = read_u32(descriptor + 0x1c);
        const std::uint32_t data_size = read_u32(descriptor + 0x34);
        const std::uint16_t flags = read_u16(descriptor + 0x0e);
        const bool linear_argb = (flags & 0x10u) &&
            (source_format == 10 || source_format == 11);
        const bool supported = linear_argb || cache_format == 16 ||
            (cache_format == 19 && source_format == 10 && !(flags & 0x10u));
        if (supported && width > 0 && height > 0 && width <= 4096 &&
            height <= 4096 && data_offset != 0xffffffffu && data_size != 0 &&
            data_size <= 64u * 1024u * 1024u) {
            std::vector<unsigned char> encoded(data_size);
            std::vector<unsigned char> pixels;
            const bool copied = h2::port::copy_cache_bytes(
                data_offset, encoded.data(), encoded.size());
            bool decoded = false;
            if (copied && linear_argb) {
                decoded = h2::port::decode_argb8_linear(
                    encoded, width, height, source_format == 11, pixels);
            } else if (copied && cache_format == 16) {
                decoded = h2::port::decode_dxt5(encoded, width, height, pixels);
            } else if (copied) {
                decoded = h2::port::decode_argb8_swizzled(
                    encoded, width, height, pixels);
            }
            if (decoded) {
                texture = SDL_CreateTexture(context.renderer, SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_STATIC, width, height);
                if (texture &&
                    (!SDL_UpdateTexture(texture, nullptr, pixels.data(), width * 4) ||
                     !SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND))) {
                    SDL_DestroyTexture(texture);
                    texture = nullptr;
                }
            }
        }
    }
    context.textures.emplace(guest_address, texture);
    return texture;
}

SDL_FColor unpack_color(std::uint32_t color) {
    constexpr float scale = 1.0f / 255.0f;
    return {
        static_cast<float>((color >> 16) & 0xffu) * scale,
        static_cast<float>((color >> 8) & 0xffu) * scale,
        static_cast<float>(color & 0xffu) * scale,
        static_cast<float>((color >> 24) & 0xffu) * scale,
    };
}

struct GuestRectangle {
    short top;
    short left;
    short bottom;
    short right;
};

CachedFont* get_font(NativeRenderContext& context, int index) {
    static constexpr std::array<std::string_view, 11> names = {
        "fixedsys-9", "conduit-12", "handel_gothic-11", "handel_gothic-24",
        "conduit-13", "conduit-12", "conduit-13", "conduit-12", "MSLCD-14",
        "conduit-16", "handel_gothic-13"};
    if (index < 0 || index >= static_cast<int>(names.size())) {
        return nullptr;
    }
    CachedFont& font = context.fonts[static_cast<std::size_t>(index)];
    if (!font.attempted) {
        font.attempted = true;
        const auto path = context.font_root / names[static_cast<std::size_t>(index)];
        std::ifstream input(path, std::ios::binary | std::ios::ate);
        if (input) {
            const std::streamsize size = input.tellg();
            if (size > 0) {
                font.file.resize(static_cast<std::size_t>(size));
                input.seekg(0);
                input.read(reinterpret_cast<char*>(font.file.data()), size);
                std::string error;
                if (!input || !h2::port::decode_font_metrics(font.file, font.metrics, error)) {
                    font.file.clear();
                }
            }
        }
    }
    return font.file.empty() ? nullptr : &font;
}

CachedFontGlyph* get_font_glyph(NativeRenderContext& context, CachedFont& font,
                                std::uint32_t character) {
    if (const auto found = font.glyphs.find(character); found != font.glyphs.end()) {
        return found->second.texture ? &found->second : nullptr;
    }
    CachedFontGlyph cached;
    std::string error;
    if (h2::port::decode_font_glyph(font.file, character, cached.metrics, error) &&
        cached.metrics.width > 0 && cached.metrics.height > 0) {
        cached.texture = SDL_CreateTexture(context.renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STATIC,
                                           cached.metrics.width, cached.metrics.height);
        if (cached.texture &&
            (!SDL_UpdateTexture(cached.texture, nullptr, cached.metrics.rgba.data(),
                                cached.metrics.width * 4) ||
             !SDL_SetTextureBlendMode(cached.texture, SDL_BLENDMODE_BLEND))) {
            SDL_DestroyTexture(cached.texture);
            cached.texture = nullptr;
        }
    }
    auto [entry, inserted] = font.glyphs.emplace(character, std::move(cached));
    (void)inserted;
    return entry->second.texture ? &entry->second : nullptr;
}

bool draw_retail_text(NativeRenderContext& context, const std::uint32_t* text,
                      const GuestRectangle& bounds, int line_gap, float scale,
                      std::uint32_t color,
                      const h2_text_request_metadata& metadata) {
    CachedFont* font = get_font(context, metadata.font);
    if (!font) {
        return false;
    }
    struct TextLine {
        std::size_t begin;
        std::size_t end;
        float width;
    };
    const float glyph_scale = scale > 0.0f ? scale : 1.0f;
    const float maximum_width = static_cast<float>(bounds.right - bounds.left);
    std::vector<TextLine> lines;
    std::size_t index = 0;
    while (index < 2048 && text[index]) {
        const std::size_t begin = index;
        std::size_t last_space = static_cast<std::size_t>(-1);
        float width = 0.0f;
        float width_before_space = 0.0f;
        bool emitted = false;
        while (index < 2048 && text[index] && text[index] != '\r') {
            CachedFontGlyph* glyph = get_font_glyph(context, *font, text[index]);
            if (!glyph) {
                return false;
            }
            const float advance = glyph->metrics.advance * glyph_scale;
            if ((metadata.flags & 1u) && index > begin &&
                width + advance > maximum_width) {
                if (last_space != static_cast<std::size_t>(-1)) {
                    lines.push_back({begin, last_space, width_before_space});
                    index = last_space + 1;
                    while (index < 2048 && text[index] == ' ') {
                        ++index;
                    }
                } else {
                    lines.push_back({begin, index, width});
                }
                emitted = true;
                break;
            }
            if (text[index] == ' ') {
                last_space = index;
                width_before_space = width;
            }
            width += advance;
            ++index;
        }
        if (!emitted) {
            lines.push_back({begin, index, width});
            if (index < 2048 && text[index] == '\r') {
                ++index;
            }
        }
    }
    if (lines.empty()) {
        return true;
    }
    const float line_height =
        (font->metrics.ascending_height + font->metrics.descending_height +
         font->metrics.leading_height + line_gap) * glyph_scale;
    const auto draw_pass = [&](std::uint32_t pass_color, float offset) {
        const SDL_FColor modulation = unpack_color(pass_color);
        for (std::size_t line_index = 0; line_index < lines.size(); ++line_index) {
            const TextLine& line = lines[line_index];
            float cursor = static_cast<float>(bounds.left) +
                font->metrics.initial_offset * glyph_scale;
            if (metadata.justification == 1) {
                cursor = static_cast<float>(bounds.right) - line.width -
                    font->metrics.initial_offset * glyph_scale;
            } else if (metadata.justification == 2) {
                cursor = (static_cast<float>(bounds.left) + bounds.right -
                          line.width) * 0.5f;
            }
            const float baseline = bounds.top +
                font->metrics.ascending_height * glyph_scale +
                line_index * line_height;
            for (std::size_t i = line.begin; i < line.end; ++i) {
                CachedFontGlyph* glyph = get_font_glyph(context, *font, text[i]);
                SDL_FRect destination{
                    cursor + glyph->metrics.x_offset * glyph_scale + offset,
                    baseline - glyph->metrics.y_offset * glyph_scale + offset,
                    glyph->metrics.width * glyph_scale,
                    glyph->metrics.height * glyph_scale};
                if (!SDL_SetTextureColorModFloat(glyph->texture, modulation.r,
                                                 modulation.g, modulation.b) ||
                    !SDL_SetTextureAlphaModFloat(glyph->texture, modulation.a) ||
                    !SDL_RenderTexture(context.renderer, glyph->texture, nullptr,
                                       &destination)) {
                    return false;
                }
                cursor += glyph->metrics.advance * glyph_scale;
            }
        }
        return true;
    };
    if (metadata.shadow_color && !draw_pass(metadata.shadow_color, glyph_scale)) {
        context.failed = true;
        return true;
    }
    if (!draw_pass(color, 0.0f)) {
        context.failed = true;
    }
    return true;
}

void draw_guest_text(const std::uint32_t* text, const void* bounds_pointer,
                     const void*, const void*, int line_gap, float scale,
                     std::uint32_t color, const void* metadata_pointer,
                     void* context_pointer) {
    auto& context = *static_cast<NativeRenderContext*>(context_pointer);
    if (!text || !bounds_pointer || context.failed) {
        return;
    }
    const auto& bounds = *static_cast<const GuestRectangle*>(bounds_pointer);
    if (metadata_pointer) {
        const auto& metadata =
            *static_cast<const h2_text_request_metadata*>(metadata_pointer);
        if (metadata.version == H2_TEXT_PROTOCOL_VERSION &&
            draw_retail_text(context, text, bounds, line_gap, scale, color, metadata)) {
            return;
        }
    }
    std::string fallback;
    for (std::size_t i = 0; i < 2048 && text[i]; ++i) {
        fallback.push_back(text[i] >= 0x20 && text[i] <= 0x7e
                               ? static_cast<char>(text[i])
                               : '?');
    }
    const float width = static_cast<float>(fallback.size()) *
                        SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    const float x = (static_cast<float>(bounds.left) + bounds.right - width) * 0.5f;
    const float y = (static_cast<float>(bounds.top) + bounds.bottom -
                     SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.5f;
    const SDL_FColor unpacked = unpack_color(color);
    if (!SDL_SetRenderDrawColorFloat(context.renderer, unpacked.r, unpacked.g,
                                     unpacked.b, unpacked.a) ||
        !SDL_RenderDebugText(context.renderer, x, y, fallback.c_str())) {
        context.failed = true;
    }
}

void draw_guest_request(const void* request_pointer, const void* vertices_pointer,
                        void* context_pointer) {
    auto& context = *static_cast<NativeRenderContext*>(context_pointer);
    if (!request_pointer || !vertices_pointer || context.failed) {
        return;
    }
    const auto& request =
        *static_cast<const h2_render_immediate_request*>(request_pointer);
    if (request.kind != H2_RENDER_REQUEST_IMMEDIATE &&
        request.kind != H2_RENDER_REQUEST_PREVIEW_QUAD &&
        request.kind != H2_RENDER_REQUEST_INTERFACE_QUAD) {
        return;
    }
    if (request.version != H2_RENDER_PROTOCOL_VERSION || request.vertex_count < 3 ||
        request.vertex_count > H2_RENDER_IMMEDIATE_MAX_VERTICES) {
        context.failed = true;
        return;
    }
    const auto* source =
        static_cast<const h2_render_immediate_vertex*>(vertices_pointer);
    SDL_Texture* texture = nullptr;
    int texture_width = 1;
    int texture_height = 1;
    bool pixel_coordinates = false;
    if (request.kind == H2_RENDER_REQUEST_INTERFACE_QUAD &&
        request.texture_tag != -1) {
        const auto texture_address = static_cast<std::uint32_t>(request.texture_tag);
        texture = get_interface_texture(context, texture_address);
        if (texture && get_interface_texture_size(
                           context, texture_address, texture_width, texture_height)) {
            for (unsigned int i = 0; i < request.vertex_count; ++i) {
                pixel_coordinates |= source[i].texture_x < -1.0f ||
                    source[i].texture_x > 1.0f || source[i].texture_y < -1.0f ||
                    source[i].texture_y > 1.0f;
            }
        }
    }
    SDL_Vertex vertices[H2_RENDER_IMMEDIATE_MAX_VERTICES]{};
    const SDL_FColor color = unpack_color(request.color);
    for (unsigned int i = 0; i < request.vertex_count; ++i) {
        vertices[i].position = {source[i].x, source[i].y};
        vertices[i].color = color;
        vertices[i].tex_coord = {
            pixel_coordinates ? source[i].texture_x / texture_width : source[i].texture_x,
            pixel_coordinates ? source[i].texture_y / texture_height : source[i].texture_y};
    }
    static constexpr int indices[] = {0, 1, 2, 0, 2, 3};
    const int index_count = request.vertex_count == 3 ? 3 : 6;
    if (!SDL_RenderGeometry(context.renderer, texture, vertices,
                            static_cast<int>(request.vertex_count), indices,
                            index_count)) {
        context.failed = true;
    }
}

struct Options {
    std::filesystem::path data_root;
    std::filesystem::path profile_root;
    std::filesystem::path guest_image;
    std::string guest_entry;
    std::filesystem::path screenshot;
    int frame_count = 0;
    int press_start_frame = 0;
    int accept_frame = 0;
    int select_frame = 0;
    int finish_frame = 0;
    int menu_accept_frame = 0;
    int campaign_accept_frame = 0;
    int level_accept_frame = 0;
    int campaign_load_frame = 0;
    int down_frame = 0;
    int up_frame = 0;
    int cancel_frame = 0;
    bool headless = false;
};

void usage(const char* program) {
    std::cerr << "Usage: " << program
              << " [--headless] [--data-root PATH] [--profile-root PATH]"
                 " [--guest-image PATH] [--guest-entry SYMBOL] [--frames COUNT]"
                 " [--press-start-frame COUNT]"
                 " [--accept-frame COUNT] [--select-frame COUNT] [--finish-frame COUNT]"
                 " [--menu-accept-frame COUNT] [--campaign-accept-frame COUNT] [--level-accept-frame COUNT]"
                 " [--campaign-load-frame COUNT] [--down-frame COUNT] [--up-frame COUNT] [--cancel-frame COUNT]"
                 " [--screenshot BMP]\n";
}

bool parse_options(int argc, char** argv, Options& options) {
    if (const char* root = std::getenv("H2_DATA_ROOT")) {
        options.data_root = root;
    }
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--headless") {
            options.headless = true;
        } else if (argument == "--data-root" && i + 1 < argc) {
            options.data_root = argv[++i];
        } else if (argument == "--profile-root" && i + 1 < argc) {
            options.profile_root = argv[++i];
        } else if (argument == "--guest-image" && i + 1 < argc) {
            options.guest_image = argv[++i];
        } else if (argument == "--guest-entry" && i + 1 < argc) {
            options.guest_entry = argv[++i];
        } else if (argument == "--frames" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.frame_count);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.frame_count <= 0) {
                return false;
            }
        } else if (argument == "--press-start-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.press_start_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.press_start_frame <= 0) {
                return false;
            }
        } else if (argument == "--accept-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.accept_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.accept_frame <= 0) {
                return false;
            }
        } else if (argument == "--select-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.select_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.select_frame <= 0) {
                return false;
            }
        } else if (argument == "--finish-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.finish_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.finish_frame <= 0) {
                return false;
            }
        } else if (argument == "--menu-accept-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.menu_accept_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.menu_accept_frame <= 0) {
                return false;
            }
        } else if (argument == "--campaign-accept-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.campaign_accept_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.campaign_accept_frame <= 0) {
                return false;
            }
        } else if (argument == "--level-accept-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.level_accept_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.level_accept_frame <= 0) {
                return false;
            }
        } else if (argument == "--campaign-load-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.campaign_load_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.campaign_load_frame <= 0) {
                return false;
            }
        } else if (argument == "--down-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.down_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.down_frame <= 0) {
                return false;
            }
        } else if (argument == "--up-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.up_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.up_frame <= 0) {
                return false;
            }
        } else if (argument == "--cancel-frame" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            const auto parsed = std::from_chars(value.data(), value.data() + value.size(),
                                                options.cancel_frame);
            if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size() ||
                options.cancel_frame <= 0) {
                return false;
            }
        } else if (argument == "--screenshot" && i + 1 < argc) {
            options.screenshot = argv[++i];
        } else {
            return false;
        }
    }
    if (options.profile_root.empty() && !options.data_root.empty()) {
        options.profile_root = options.data_root.parent_path() / "profiles";
    }
    return !options.data_root.empty() && !options.profile_root.empty() &&
        (options.screenshot.empty() || options.frame_count > 0) &&
        (options.press_start_frame == 0 ||
         (options.frame_count >= options.press_start_frame && !options.headless)) &&
        (options.accept_frame == 0 ||
         (options.press_start_frame > 0 &&
          options.accept_frame > options.press_start_frame &&
          options.frame_count >= options.accept_frame && !options.headless)) &&
        (options.select_frame == 0 ||
         (options.accept_frame > 0 &&
          options.select_frame > options.accept_frame &&
          options.frame_count >= options.select_frame && !options.headless)) &&
        (options.finish_frame == 0 ||
         (options.select_frame > 0 &&
          options.finish_frame > options.select_frame &&
          options.frame_count >= options.finish_frame && !options.headless)) &&
        (options.menu_accept_frame == 0 ||
         (options.finish_frame > 0 &&
          options.menu_accept_frame > options.finish_frame &&
          options.frame_count >= options.menu_accept_frame && !options.headless)) &&
        (options.campaign_accept_frame == 0 ||
         (options.menu_accept_frame > 0 &&
          options.campaign_accept_frame > options.menu_accept_frame &&
          options.frame_count >= options.campaign_accept_frame && !options.headless)) &&
        (options.level_accept_frame == 0 ||
         (options.campaign_accept_frame > 0 &&
          options.level_accept_frame > options.campaign_accept_frame &&
          options.frame_count >= options.level_accept_frame && !options.headless)) &&
        (options.campaign_load_frame == 0 ||
         (options.level_accept_frame > 0 &&
          options.campaign_load_frame > options.level_accept_frame &&
          options.frame_count >= options.campaign_load_frame && !options.headless)) &&
        (options.down_frame == 0 ||
         (options.finish_frame > 0 &&
          options.down_frame > options.finish_frame &&
          options.frame_count >= options.down_frame && !options.headless)) &&
        (options.up_frame == 0 ||
         (options.down_frame > 0 &&
          options.up_frame > options.down_frame &&
          options.frame_count >= options.up_frame && !options.headless)) &&
        (options.cancel_frame == 0 ||
         (options.press_start_frame > 0 &&
          options.cancel_frame > options.press_start_frame &&
          options.frame_count >= options.cancel_frame && !options.headless));
}

int run_window(GuestEntry guest_frame, GuestEntry guest_press_start,
               GuestEntry guest_accept, GuestEntry guest_campaign_load,
               GuestEntry guest_up, GuestEntry guest_down,
               GuestEntry guest_cancel, void* guest_stack_top,
               int frame_count, int press_start_frame, int accept_frame, int select_frame,
               int finish_frame, int menu_accept_frame, int campaign_accept_frame,
               int level_accept_frame, int campaign_load_frame,
               int down_frame, int up_frame,
               int cancel_frame,
               const std::filesystem::path& screenshot,
               const h2::port::GuestArena* arena,
               const std::filesystem::path& data_root) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Halo 2 native port - recovered menu geometry preview", 960, 540,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        std::cerr << "window creation failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "renderer creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!SDL_SetRenderLogicalPresentation(
            renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        std::cerr << "logical presentation setup failed: " << SDL_GetError() << '\n';
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    NativeRenderContext render_context;
    render_context.renderer = renderer;
    render_context.arena = arena;
    render_context.font_root = data_root / "maps" / "fonts";
    h2::port::set_render_request_handler(draw_guest_request, &render_context);
    h2::port::set_text_request_handler(draw_guest_text, &render_context);
    bool running = true;
    int result = 0;
    int rendered_frames = 0;
    const auto dispatch_press_start = [&]() {
        if (!guest_press_start) {
            return;
        }
        const int input_result = h2_call_guest_int_0(guest_press_start, guest_stack_top);
        if (input_result != 0) {
            std::cerr << "guest press-start dispatch failed with result "
                      << input_result << '\n';
            result = 1;
            running = false;
        }
    };
    const auto dispatch_accept = [&]() {
        if (!guest_accept) {
            return false;
        }
        const int input_result = h2_call_guest_int_0(guest_accept, guest_stack_top);
        if (input_result == 2) {
            return false;
        }
        if (input_result != 0) {
            std::cerr << "guest accept dispatch failed with result "
                      << input_result << '\n';
            result = 1;
            running = false;
        }
        return true;
    };
    const auto dispatch_campaign_load = [&]() {
        if (!guest_campaign_load) {
            std::cerr << "guest Campaign cache-load entry is unavailable\n";
            result = 1;
            running = false;
            return false;
        }
        const int load_result = h2_call_guest_int_0(guest_campaign_load, guest_stack_top);
        if (load_result != 0) {
            std::cerr << "guest Campaign cache load failed with result "
                      << load_result << '\n';
            result = 1;
        } else {
            std::cout << "Recovered loader accepted the selected Campaign cache; "
                         "ending menu frames before tag ownership changes.\n";
        }
        running = false;
        return load_result == 0;
    };
    const auto dispatch_move = [&](GuestEntry entry, const char* direction,
                                   bool require_movement = false) {
        if (!entry) {
            return false;
        }
        const int input_result = h2_call_guest_int_0(entry, guest_stack_top);
        if (input_result == 2) {
            return false;
        }
        if (input_result == 4 && !require_movement) {
            return true;
        }
        if (input_result != 0) {
            std::cerr << "guest " << direction << " dispatch failed with result "
                      << input_result << '\n';
            result = 1;
            running = false;
        }
        return true;
    };
    const auto dispatch_cancel = [&]() {
        if (!guest_cancel) {
            return false;
        }
        const int input_result = h2_call_guest_int_0(guest_cancel, guest_stack_top);
        if (input_result == 2) {
            return false;
        }
        if (input_result != 0) {
            std::cerr << "guest cancel dispatch failed with result "
                      << input_result << '\n';
            result = 1;
            running = false;
        }
        return true;
    };
    while (running) {
        if (press_start_frame > 0 && rendered_frames + 1 == press_start_frame) {
            dispatch_press_start();
        }
        if (accept_frame > 0 && rendered_frames + 1 == accept_frame) {
            dispatch_accept();
        }
        if (select_frame > 0 && rendered_frames + 1 == select_frame) {
            dispatch_accept();
        }
        if (finish_frame > 0 && rendered_frames + 1 == finish_frame) {
            dispatch_accept();
        }
        if (menu_accept_frame > 0 && rendered_frames + 1 == menu_accept_frame) {
            dispatch_accept();
        }
        if (campaign_accept_frame > 0 && rendered_frames + 1 == campaign_accept_frame) {
            dispatch_accept();
        }
        if (level_accept_frame > 0 && rendered_frames + 1 == level_accept_frame) {
            dispatch_accept();
        }
        if (campaign_load_frame > 0 && rendered_frames + 1 == campaign_load_frame) {
            dispatch_campaign_load();
            break;
        }
        if (down_frame > 0 && rendered_frames + 1 == down_frame) {
            dispatch_move(guest_down, "down", true);
        }
        if (up_frame > 0 && rendered_frames + 1 == up_frame) {
            dispatch_move(guest_up, "up", true);
        }
        if (cancel_frame > 0 && rendered_frames + 1 == cancel_frame) {
            dispatch_cancel();
        }
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
                       event.key.key == SDLK_ESCAPE) {
                if (!dispatch_cancel()) {
                    running = false;
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
                       event.key.key == SDLK_UP) {
                dispatch_move(guest_up, "up");
            } else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
                       event.key.key == SDLK_DOWN) {
                dispatch_move(guest_down, "down");
            } else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
                       (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE)) {
                if (!dispatch_accept()) {
                    dispatch_press_start();
                }
            } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
                       event.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_UP) {
                dispatch_move(guest_up, "up");
            } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
                       event.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_DOWN) {
                dispatch_move(guest_down, "down");
            } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
                       (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH ||
                        event.gbutton.button == SDL_GAMEPAD_BUTTON_START)) {
                if (!dispatch_accept()) {
                    dispatch_press_start();
                }
            } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
                       event.gbutton.button == SDL_GAMEPAD_BUTTON_EAST) {
                dispatch_cancel();
            }
        }
        if (!running) {
            break;
        }
        SDL_SetRenderDrawColor(renderer, 8, 18, 24, 255);
        SDL_RenderClear(renderer);
        if (guest_frame) {
            const int frame_result = h2_call_guest_int_0(guest_frame, guest_stack_top);
            if (frame_result != 0 || render_context.failed) {
                std::cerr << "guest frame rendering failed";
                if (frame_result != 0) {
                    std::cerr << " with result " << frame_result;
                }
                if (render_context.failed && *SDL_GetError()) {
                    std::cerr << ": " << SDL_GetError();
                }
                std::cerr << '\n';
                result = 1;
                running = false;
            }
        }
        if (!screenshot.empty() && frame_count > 0 &&
            rendered_frames + 1 >= frame_count) {
            SDL_Surface* surface = SDL_RenderReadPixels(renderer, nullptr);
            if (!surface || !SDL_SaveBMP(surface, screenshot.c_str())) {
                std::cerr << "screenshot capture failed: " << SDL_GetError() << '\n';
                result = 1;
                running = false;
            } else {
                std::cout << "Saved final rendered frame to " << screenshot << '\n';
            }
            if (surface) {
                SDL_DestroySurface(surface);
            }
        }
        SDL_RenderPresent(renderer);
        ++rendered_frames;
        if (frame_count > 0 && rendered_frames >= frame_count) {
            running = false;
        }
        SDL_Delay(8);
    }

    h2::port::set_render_request_handler(nullptr);
    h2::port::set_text_request_handler(nullptr);
    if (guest_frame) {
        const auto stats = h2::port::host_service_stats();
        std::size_t decoded_textures = 0;
        for (const auto& [address, texture] : render_context.textures) {
            (void)address;
            decoded_textures += texture != nullptr;
        }
        std::size_t decoded_glyphs = 0;
        for (const auto& font : render_context.fonts) {
            for (const auto& [character, glyph] : font.glyphs) {
                (void)character;
                decoded_glyphs += glyph.texture != nullptr;
            }
        }
        std::cout << "Rendered " << rendered_frames << " host frames; guest requests: render "
                  << stats.render_request_calls << ", text " << stats.text_request_calls
                  << ", video " << stats.video_control_calls << "; decoded textures "
                  << decoded_textures << '/' << render_context.textures.size()
                  << ", retail glyphs " << decoded_glyphs << '\n';
    }
    for (const auto& [address, texture] : render_context.textures) {
        (void)address;
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    for (const auto& font : render_context.fonts) {
        for (const auto& [character, glyph] : font.glyphs) {
            (void)character;
            if (glyph.texture) {
                SDL_DestroyTexture(glyph.texture);
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}

}  // namespace

namespace {

/* Report guest faults: the program counter, link register and frame-pointer
   chain, which tools/guest_symbolize.py maps to guest symbols. */
void report_fault(int signal_number, siginfo_t* info, void* context) {
    auto* const state = static_cast<ucontext_t*>(context);
    char line[256];
    int length = std::snprintf(line, sizeof(line),
                               "h2-port: fault signal %d address 0x%llx pc 0x%llx lr 0x%llx\n",
                               signal_number,
                               static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(info->si_addr)),
                               static_cast<unsigned long long>(state->uc_mcontext->__ss.__pc),
                               static_cast<unsigned long long>(state->uc_mcontext->__ss.__lr));
    write(2, line, static_cast<size_t>(length));
    /* a jump into the XBE's own x86 code: a retail code pointer (usually a
       vtable slot) whose function no recovered unit defines yet */
    const auto pc = static_cast<std::uintptr_t>(state->uc_mcontext->__ss.__pc);
    if (pc >= h2::port::kGuestBias + 0x12000 && pc < h2::port::kGuestBias + 0x379c4c) {
        length = std::snprintf(line, sizeof(line),
                               "h2-port: called unrecovered retail function 0x%llx from 0x%llx\n",
                               static_cast<unsigned long long>(pc - h2::port::kGuestBias),
                               static_cast<unsigned long long>(state->uc_mcontext->__ss.__lr));
        write(2, line, static_cast<size_t>(length));
    }
    auto frame = static_cast<std::uintptr_t>(state->uc_mcontext->__ss.__fp);
    for (int depth = 0; depth < 32 && frame && (frame & 7) == 0; ++depth) {
        const auto* const record = reinterpret_cast<const std::uintptr_t*>(frame);
        length = std::snprintf(line, sizeof(line), "h2-port:   frame 0x%llx\n",
                               static_cast<unsigned long long>(record[1]));
        write(2, line, static_cast<size_t>(length));
        if (record[0] <= frame)
            break;
        frame = record[0];
    }
    /* the registers, and for those holding a guest pointer the words it
       points at and (as a vtable) that word's first slots: names the object
       a crash went through (guest_symbolize.py names the slot targets) */
    /* write() from an unreadable address fails with EFAULT instead of
       faulting (mincore succeeds on reserved, unreadable pages) */
    static int probe[2] = {-1, -1};
    if (probe[0] < 0 && pipe(probe) != 0)
        probe[0] = probe[1] = -1;
    const auto readable = [](std::uintptr_t address) {
        if (probe[1] < 0 || write(probe[1], reinterpret_cast<const void*>(address), 4) != 4)
            return false;
        char sink[4];
        return read(probe[0], sink, sizeof(sink)) == 4;
    };
    for (int index = 0; index < 29; ++index) {
        const auto value = static_cast<std::uint32_t>(state->uc_mcontext->__ss.__x[index]);
        length = std::snprintf(line, sizeof(line), "h2-port:   x%d = 0x%08x", index, value);
        const auto address = h2::port::kGuestBias + value;
        if (value >= 0x10000 && (value & 3) == 0 && readable(address)) {
            const auto first = *reinterpret_cast<const std::uint32_t*>(address);
            length += std::snprintf(line + length, sizeof(line) - length, " -> [0x%08x]", first);
            const auto table = h2::port::kGuestBias + first;
            if (first >= 0x10000 && (first & 3) == 0 && readable(table) && readable(table + 12))
                for (int slot = 0; slot < 4; ++slot)
                    length += std::snprintf(line + length, sizeof(line) - length, " 0x%08x",
                                            reinterpret_cast<const std::uint32_t*>(table)[slot]);
        }
        line[length++] = '\n';
        write(2, line, static_cast<size_t>(length));
    }
    /* H2_FAULT_REGDUMP="x21:0x200,x23:0x40": that many bytes behind a
       register's guest pointer, eight words a line */
    if (const char* dump = std::getenv("H2_FAULT_REGDUMP")) {
        const char* cursor = dump;
        while (*cursor == 'x') {
            char* end = nullptr;
            const long reg = std::strtol(cursor + 1, &end, 10);
            if (end == cursor + 1 || *end != ':' || reg < 0 || reg > 28)
                break;
            const unsigned long bytes = std::strtoul(end + 1, &end, 16);
            const auto value = static_cast<std::uint32_t>(state->uc_mcontext->__ss.__x[reg]);
            for (unsigned long offset = 0; offset < bytes; offset += 32) {
                length = std::snprintf(line, sizeof(line), "h2-port:   x%ld+0x%03lx:", reg, offset);
                for (unsigned long word = 0; word < 32 && offset + word < bytes; word += 4) {
                    const auto address = h2::port::kGuestBias + value + offset + word;
                    if (!readable(address))
                        break;
                    length += std::snprintf(line + length, sizeof(line) - length, " %08x",
                                            *reinterpret_cast<const std::uint32_t*>(address));
                }
                line[length++] = '\n';
                write(2, line, static_cast<size_t>(length));
            }
            cursor = *end ? end + 1 : end;
        }
    }
    /* H2_FAULT_DUMP="0x510c98,0x5023f4": guest words to print at the fault */
    if (const char* dump = std::getenv("H2_FAULT_DUMP")) {
        const char* cursor = dump;
        while (*cursor) {
            char* end = nullptr;
            const unsigned long address = std::strtoul(cursor, &end, 16);
            if (end == cursor)
                break;
            const auto* word = reinterpret_cast<const std::uint32_t*>(h2::port::kGuestBias + address);
            length = std::snprintf(line, sizeof(line), "h2-port:   [0x%08lx] = 0x%08x\n", address, *word);
            write(2, line, static_cast<size_t>(length));
            cursor = *end ? end + 1 : end;
        }
    }
    signal(signal_number, SIG_DFL);
    raise(signal_number);
}

void install_fault_reporter() {
    struct sigaction action {};
    action.sa_sigaction = report_fault;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    sigaction(SIGSEGV, &action, nullptr);
    sigaction(SIGBUS, &action, nullptr);
    sigaction(SIGILL, &action, nullptr);
    sigaction(SIGTRAP, &action, nullptr);
}

/* The recovered game's own main loop drives frames in a whole image: its
   Direct3D swap calls host_h2_frame, which presents what the frame drew,
   pumps events and refreshes the controller the game polls. */
struct GameSession {
    SDL_Window* window = nullptr;
    SDL_MetalView metal_view = nullptr;
    std::unique_ptr<h2::gpu::Renderer> gpu;
    int frame_count = 0;
    int frames = 0;
    std::filesystem::path screenshot;
    std::chrono::steady_clock::time_point last_frame;
    /* XINPUT_GAMEPAD for port 0 */
    std::uint16_t buttons = 0;
    std::uint8_t analog[8] = {};
    std::int16_t thumbs[4] = {};
    std::uint32_t packet = 0;
    float mouse_x = 0;
    float mouse_y = 0;
    struct ScriptedPress {
        int first;
        int last;
        std::string button;
    };
    std::vector<ScriptedPress> script;
};

GameSession& game_session() {
    static GameSession session;
    return session;
}

void parse_input_script(GameSession& session) {
    /* H2_INPUT_SCRIPT="frame:button[:frames],..." e.g. "400:start:4,700:a:4" */
    const char* script = std::getenv("H2_INPUT_SCRIPT");
    if (!script)
        return;
    std::string text(script);
    std::size_t start = 0;
    while (start < text.size()) {
        const std::size_t end = text.find(',', start);
        const std::string item = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
        const std::size_t first_colon = item.find(':');
        if (first_colon != std::string::npos) {
            const std::size_t second_colon = item.find(':', first_colon + 1);
            const int frame = std::atoi(item.substr(0, first_colon).c_str());
            const std::string button = item.substr(first_colon + 1, second_colon == std::string::npos
                                                                        ? std::string::npos
                                                                        : second_colon - first_colon - 1);
            const int length = second_colon == std::string::npos ? 4 : std::atoi(item.c_str() + second_colon + 1);
            session.script.push_back({frame, frame + std::max(1, length) - 1, button});
        }
        if (end == std::string::npos)
            break;
        start = end + 1;
    }
}

void request_snapshot(int);
extern std::atomic<bool> g_snapshot_requested;

/* the "guest ..." control lines, for the guest's test hooks to take
   (host_h2_guest_command) */
std::deque<std::string> g_guest_commands;

/* H2_CONTROL=file: a test driver appends lines while the game runs, read at
   each frame: "press BUTTON FRAMES" (any input-script button, e.g. a, rt,
   ly+, rx-25), "mouse DX DY" (mouse motion in points), "snapshot" (as
   SIGUSR1), "trace" (the next frame, as H2_TRACE_FRAME), "quit", "guest COMMAND..." (handed
   to the guest's test hooks, port/runtime/guest_test_state.cpp) */
void poll_control(GameSession& session) {
    static const char* path = std::getenv("H2_CONTROL");
    static std::streamoff offset = 0;
    if (!path)
        return;
    std::ifstream file(path);
    if (!file)
        return;
    file.seekg(offset);
    std::string line;
    while (std::getline(file, line)) {
        if (file.eof())
            break;   /* a partly written last line waits for the next frame */
        offset = file.tellg();
        std::istringstream words(line);
        std::string command;
        words >> command;
        if (command == "press") {
            std::string button;
            int frames = 1;
            words >> button >> frames;
            session.script.push_back({static_cast<int>(session.frames) + 1,
                                      static_cast<int>(session.frames) + std::max(1, frames), button});
        } else if (command == "mouse") {
            float dx = 0, dy = 0;
            words >> dx >> dy;
            session.mouse_x += dx;
            session.mouse_y += dy;
        } else if (command == "snapshot") {
            g_snapshot_requested = true;
        } else if (command == "trace") {
            g_trace_next_frame = true;
        } else if (command == "quit") {
            std::exit(0);
        } else if (command == "guest") {
            std::string rest;
            std::getline(words, rest);
            g_guest_commands.push_back(rest.size() && rest[0] == ' ' ? rest.substr(1) : rest);
        }
    }
}

void apply_button(GameSession& session, const std::string& name, bool down) {
    /* digital: dpad up/down/left/right, start, back, left/right thumb;
       analog: a, b, x, y, black, white, left/right trigger; sticks: see below */
    static const std::pair<const char*, std::uint16_t> digital[] = {
        {"up", 0x01}, {"down", 0x02}, {"left", 0x04}, {"right", 0x08},
        {"start", 0x10}, {"back", 0x20}, {"ls", 0x40}, {"rs", 0x80}};
    static const char* analog[] = {"a", "b", "x", "y", "black", "white", "lt", "rt"};
    for (const auto& [label, bit] : digital)
        if (name == label)
            session.buttons = down ? (session.buttons | bit) : (session.buttons & ~bit);
    for (int index = 0; index < 8; ++index)
        if (name == analog[index])
            session.analog[index] = down ? 0xff : 0;
    /* thumbsticks: lx+/lx- ly+/ly- rx+/rx- ry+/ry- fully over (y+ is up), or
       part way with a percentage after the sign (rx+25) */
    static const char* sticks[] = {"lx", "ly", "rx", "ry"};
    for (int index = 0; index < 4; ++index)
        if (name.size() >= 3 && name.compare(0, 2, sticks[index]) == 0 && (name[2] == '+' || name[2] == '-')) {
            const int percent = name.size() > 3 ? std::clamp(std::atoi(name.c_str() + 3), 0, 100) : 100;
            const int value = 32767 * percent / 100;
            session.thumbs[index] = down ? static_cast<std::int16_t>(name[2] == '+' ? value : -value) : 0;
        }
}

void update_input(GameSession& session) {
    session.buttons = 0;
    std::memset(session.analog, 0, sizeof(session.analog));
    std::memset(session.thumbs, 0, sizeof(session.thumbs));
    if (session.window) {
        const bool* keys = SDL_GetKeyboardState(nullptr);
        const auto key = [&](SDL_Scancode code) { return keys[code]; };
        /* Halo 2 defaults: A jump, B melee, X action/reload, Y weapon swap,
           left trigger grenade, right trigger fire, left stick crouch,
           right stick zoom, white flashlight, black grenade swap */
        struct KeyMap { SDL_Scancode code; const char* button; };
        static const KeyMap keymap[] = {
            {SDL_SCANCODE_SPACE, "a"}, {SDL_SCANCODE_F, "b"}, {SDL_SCANCODE_E, "x"},
            {SDL_SCANCODE_Q, "y"}, {SDL_SCANCODE_G, "lt"}, {SDL_SCANCODE_LCTRL, "ls"},
            {SDL_SCANCODE_Z, "rs"}, {SDL_SCANCODE_T, "white"}, {SDL_SCANCODE_TAB, "black"},
            {SDL_SCANCODE_RETURN, "start"}, {SDL_SCANCODE_ESCAPE, "back"},
            {SDL_SCANCODE_UP, "up"}, {SDL_SCANCODE_DOWN, "down"},
            {SDL_SCANCODE_LEFT, "left"}, {SDL_SCANCODE_RIGHT, "right"}};
        for (const auto& entry : keymap)
            if (key(entry.code))
                apply_button(session, entry.button, true);
        float mouse_dx = 0, mouse_dy = 0;
        const SDL_MouseButtonFlags mouse = SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
        if (mouse & SDL_BUTTON_LMASK)
            apply_button(session, "rt", true);
        if (mouse & SDL_BUTTON_RMASK)
            apply_button(session, "lt", true);
        const auto axis = [](bool negative, bool positive) {
            return static_cast<std::int16_t>((positive ? 32767 : 0) - (negative ? 32767 : 0));
        };
        session.thumbs[0] = axis(key(SDL_SCANCODE_A), key(SDL_SCANCODE_D));
        session.thumbs[1] = axis(key(SDL_SCANCODE_S), key(SDL_SCANCODE_W));
        /* the mouse turns the view directly (host_h2_mouse_take), not
           through the right stick's dead zone and top turn rate */
        session.mouse_x += mouse_dx;
        session.mouse_y += mouse_dy;
        /* a gamepad, when present, adds to the keyboard */
        int count = 0;
        SDL_JoystickID* ids = SDL_GetGamepads(&count);
        if (ids && count > 0) {
            static SDL_Gamepad* pad = nullptr;
            if (!pad)
                pad = SDL_OpenGamepad(ids[0]);
            if (pad) {
                const auto pressed = [&](SDL_GamepadButton button) { return SDL_GetGamepadButton(pad, button); };
                struct PadMap { SDL_GamepadButton button; const char* name; };
                static const PadMap padmap[] = {
                    {SDL_GAMEPAD_BUTTON_SOUTH, "a"}, {SDL_GAMEPAD_BUTTON_EAST, "b"},
                    {SDL_GAMEPAD_BUTTON_WEST, "x"}, {SDL_GAMEPAD_BUTTON_NORTH, "y"},
                    {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "white"}, {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "black"},
                    {SDL_GAMEPAD_BUTTON_START, "start"}, {SDL_GAMEPAD_BUTTON_BACK, "back"},
                    {SDL_GAMEPAD_BUTTON_LEFT_STICK, "ls"}, {SDL_GAMEPAD_BUTTON_RIGHT_STICK, "rs"},
                    {SDL_GAMEPAD_BUTTON_DPAD_UP, "up"}, {SDL_GAMEPAD_BUTTON_DPAD_DOWN, "down"},
                    {SDL_GAMEPAD_BUTTON_DPAD_LEFT, "left"}, {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "right"}};
                for (const auto& entry : padmap)
                    if (pressed(entry.button))
                        apply_button(session, entry.name, true);
                if (SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 8000)
                    apply_button(session, "lt", true);
                if (SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 8000)
                    apply_button(session, "rt", true);
                const auto stick = [&](SDL_GamepadAxis a, bool invert, std::int16_t keyboard) {
                    const int value = SDL_GetGamepadAxis(pad, a);
                    const int signed_value = invert ? -std::max(value, -32767) : value;
                    return keyboard ? keyboard : static_cast<std::int16_t>(std::clamp(signed_value, -32767, 32767));
                };
                session.thumbs[0] = stick(SDL_GAMEPAD_AXIS_LEFTX, false, session.thumbs[0]);
                session.thumbs[1] = stick(SDL_GAMEPAD_AXIS_LEFTY, true, session.thumbs[1]);
                session.thumbs[2] = stick(SDL_GAMEPAD_AXIS_RIGHTX, false, session.thumbs[2]);
                session.thumbs[3] = stick(SDL_GAMEPAD_AXIS_RIGHTY, true, session.thumbs[3]);
            }
        }
        SDL_free(ids);
    }
    poll_control(session);
    for (const auto& press : session.script)
        if (session.frames >= press.first && session.frames <= press.last)
            apply_button(session, press.button, true);
    ++session.packet;
}

bool open_game_session(bool headless, int frame_count, const std::filesystem::path& screenshot,
                       const h2::port::GuestArena* arena, const std::filesystem::path& data_root) {
    auto& session = game_session();
    session.frame_count = frame_count;
    session.screenshot = screenshot;
    session.last_frame = std::chrono::steady_clock::now();
    parse_input_script(session);
    std::signal(SIGUSR1, request_snapshot);
    (void)data_root;
    /* the game renders offscreen even headless, so screenshots work */
    std::string error;
    const char* scale = std::getenv("H2_RENDER_SCALE");
    session.gpu = h2::gpu::Renderer::create(*arena, scale ? std::atoi(scale) : 2, error);
    if (!session.gpu) {
        std::cerr << "renderer setup failed: " << error << '\n';
        return false;
    }
    /* headless: a silent mixer clock, no audio device */
    if (!h2::audio::open_session(*arena, !headless, error)) {
        std::cerr << "audio setup failed: " << error << '\n';
        return false;
    }
    if (headless)
        return true;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
        return false;
    }
    /* H2_WINDOW_HIDDEN=1: a real window and presentation, never shown */
    const bool hidden = std::getenv("H2_WINDOW_HIDDEN") != nullptr;
    session.window = SDL_CreateWindow("Halo 2", 960, 720,
                                      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_METAL |
                                          (hidden ? SDL_WINDOW_HIDDEN : 0));
    /* H2_WINDOW_SIZE=WxH: resize after creation (as going full screen does),
       to check presentation at other sizes without showing a window */
    if (const char* size = std::getenv("H2_WINDOW_SIZE"); size && session.window) {
        int width = 0, height = 0;
        if (std::sscanf(size, "%dx%d", &width, &height) == 2 && width > 0 && height > 0)
            SDL_SetWindowSize(session.window, width, height);
    }
    session.metal_view = session.window ? SDL_Metal_CreateView(session.window) : nullptr;
    if (!session.metal_view) {
        std::cerr << "game window setup failed: " << SDL_GetError() << '\n';
        return false;
    }
    session.gpu->attach_layer(SDL_Metal_GetLayer(session.metal_view));
    if (!hidden)
        SDL_SetWindowRelativeMouseMode(session.window, true);
    return true;
}

/* SIGUSR1: save the next frame (game frame and window) to H2_SNAPSHOT_DIR
   (default $TMPDIR/h2-snapshots), so a running game can be looked at */
std::atomic<bool> g_snapshot_requested{false};

void request_snapshot(int) { g_snapshot_requested = true; }

std::filesystem::path snapshot_dir() {
    if (const char* dir = std::getenv("H2_SNAPSHOT_DIR"))
        return dir;
    const char* tmp = std::getenv("TMPDIR");
    return std::filesystem::path(tmp ? tmp : "/tmp") / "h2-snapshots";
}

bool save_bgra_bmp(const std::filesystem::path& path, std::vector<std::uint8_t>& pixels, int width, int height) {
    SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_XRGB8888, pixels.data(), width * 4);
    const bool saved = surface && SDL_SaveBMP(surface, path.c_str());
    if (surface)
        SDL_DestroySurface(surface);
    return saved;
}

/* H2_RECORD=file.mp4: every presented game frame goes to ffmpeg (H.264 via
   VideoToolbox) on a writer thread. The file is fragmented, so it stays
   playable when the game is killed. Frames the encoder can't keep up with
   are dropped, never waited for. */
class FrameRecorder {
public:
    static FrameRecorder* get() {
        static FrameRecorder* recorder = [] () -> FrameRecorder* {
            const char* path = std::getenv("H2_RECORD");
            return path && *path ? new FrameRecorder(path) : nullptr;
        }();
        return recorder;
    }

    void add(std::vector<std::uint8_t>&& bgra, int width, int height) {
        std::unique_lock lock(mutex_);
        if (!pipe_) {
            if (failed_)
                return;
            width_ = width;
            height_ = height;
            const char* fps = std::getenv("H2_RECORD_FPS");
            const std::string command =
                "ffmpeg -loglevel error -y -f rawvideo -pix_fmt bgra -s " + std::to_string(width) + "x" +
                std::to_string(height) + " -framerate " + (fps ? fps : "60") +
                " -i - -c:v h264_videotoolbox -b:v 8M -pix_fmt yuv420p"
                " -movflags frag_keyframe+empty_moov '" + path_ + "'";
            pipe_ = popen(command.c_str(), "w");
            if (!pipe_) {
                failed_ = true;
                std::cerr << "h2-port: H2_RECORD: could not start ffmpeg\n";
                return;
            }
            writer_ = std::thread([this] { write_loop(); });
            std::atexit([] { FrameRecorder::get()->finish(); });
            std::cerr << "h2-port: recording " << width << "x" << height << " to " << path_ << '\n';
        }
        if (width != width_ || height != height_)
            return;
        if (queue_.size() >= 8) {
            ++dropped_;
            return;
        }
        queue_.push_back(std::move(bgra));
        ready_.notify_one();
    }

    void finish() {
        {
            std::lock_guard lock(mutex_);
            if (!pipe_ || done_)
                return;
            done_ = true;
        }
        ready_.notify_one();
        writer_.join();
        pclose(pipe_);
        std::cerr << "h2-port: recording finished (" << dropped_ << " frames dropped)\n";
    }

private:
    explicit FrameRecorder(std::string path) : path_(std::move(path)) {}

    void write_loop() {
        for (;;) {
            std::vector<std::uint8_t> frame;
            {
                std::unique_lock lock(mutex_);
                ready_.wait(lock, [this] { return done_ || !queue_.empty(); });
                if (queue_.empty())
                    return;
                frame = std::move(queue_.front());
                queue_.pop_front();
            }
            if (std::fwrite(frame.data(), 1, frame.size(), pipe_) != frame.size())
                return;
        }
    }

    std::string path_;
    std::mutex mutex_;
    std::condition_variable ready_;
    std::deque<std::vector<std::uint8_t>> queue_;
    std::thread writer_;
    FILE* pipe_ = nullptr;
    int width_ = 0, height_ = 0;
    unsigned long dropped_ = 0;
    bool failed_ = false;
    bool done_ = false;
};

bool save_frame_bmp(const std::filesystem::path& path) {
    auto& session = game_session();
    std::vector<std::uint8_t> pixels;
    int width = 0, height = 0;
    if (!session.gpu || !session.gpu->read_pixels(pixels, width, height))
        return false;
    SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_XRGB8888, pixels.data(), width * 4);
    const bool saved = surface && SDL_SaveBMP(surface, path.c_str());
    if (surface)
        SDL_DestroySurface(surface);
    return saved;
}

} // namespace

extern "C" void host_h2_gpu_draw(const void* packet) {
    auto& session = game_session();
    if (session.gpu && packet)
        session.gpu->draw(*static_cast<const h2_gpu_draw*>(packet));
}

extern "C" void host_h2_gpu_clear(const void* packet) {
    auto& session = game_session();
    if (session.gpu && packet)
        session.gpu->clear(*static_cast<const h2_gpu_clear*>(packet));
}

extern "C" void host_h2_gpu_visibility_end(unsigned int index) {
    auto& session = game_session();
    if (session.gpu)
        session.gpu->end_visibility_test(index);
}

extern "C" unsigned int host_h2_gpu_visibility_result(unsigned int index) {
    auto& session = game_session();
    return session.gpu ? session.gpu->visibility_result(index) : 0;
}

extern "C" void host_h2_frame(void) {
    auto& session = game_session();
    ++session.frames;
    const bool last = session.frame_count > 0 && session.frames >= session.frame_count;
    /* H2_SCREENSHOT_EVERY=n: also save every n-th frame next to --screenshot */
    static const int every = [] {
        const char* value = std::getenv("H2_SCREENSHOT_EVERY");
        return value ? std::atoi(value) : 0;
    }();
    const bool periodic = every > 0 && !session.screenshot.empty() && session.frames % every == 0;
    const bool snapshot = g_snapshot_requested.exchange(false);
    FrameRecorder* const recorder = FrameRecorder::get();
    const bool keep = ((last || periodic) && !session.screenshot.empty()) || snapshot || recorder;
    if (session.gpu) {
        if (snapshot)
            session.gpu->capture_window();
        session.gpu->present(keep);
        if (recorder) {
            std::vector<std::uint8_t> pixels;
            int width = 0, height = 0;
            if (session.gpu->read_pixels(pixels, width, height))
                recorder->add(std::move(pixels), width, height);
        }
        if (snapshot) {
            const auto dir = snapshot_dir();
            std::error_code ignored;
            std::filesystem::create_directories(dir, ignored);
            const std::string stem = "frame-" + std::to_string(session.frames);
            std::vector<std::uint8_t> pixels;
            int width = 0, height = 0;
            if (save_frame_bmp(dir / (stem + "-game.bmp")))
                std::cerr << "h2-port: snapshot " << (dir / (stem + "-game.bmp")) << '\n';
            if (session.gpu->read_window_pixels(pixels, width, height) &&
                save_bgra_bmp(dir / (stem + "-window.bmp"), pixels, width, height))
                std::cerr << "h2-port: snapshot " << (dir / (stem + "-window.bmp")) << '\n';
        }
        if (keep && !session.screenshot.empty() && (last || periodic)) {
            std::filesystem::path path = session.screenshot;
            if (periodic && !last)
                path.replace_filename(path.stem().string() + "-" + std::to_string(session.frames) +
                                      path.extension().string());
            if (save_frame_bmp(path))
                std::cout << "Saved rendered frame " << session.frames << " to " << path << std::endl;
        }
    }
    if (session.window) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                std::exit(0);
        }
        /* the Xbox swapped at 60 Hz */
        const auto target = session.last_frame + std::chrono::microseconds(16667);
        const auto now = std::chrono::steady_clock::now();
        if (now < target)
            std::this_thread::sleep_for(target - now);
        session.last_frame = std::chrono::steady_clock::now();
    }
    update_input(session);
    if (last) {
        const auto stats = session.gpu ? session.gpu->stats() : h2::gpu::RendererStats{};
        std::cout << "Presented " << session.frames << " game frames; draws " << stats.draws << " (skipped "
                  << stats.draws_skipped << ", failed " << stats.draws_failed << "), clears " << stats.clears
                  << ", pipelines " << stats.pipelines << ", textures " << stats.textures << std::endl;
        std::exit(0);
    }
}

extern "C" int host_h2_input_state(unsigned int port, void* state) {
    /* XINPUT_STATE: packet number, then XINPUT_GAMEPAD (buttons, eight
       analog bytes, four thumb axes) */
    if (port != 0)
        return 0;
    auto& session = game_session();
    auto* bytes = static_cast<unsigned char*>(state);
    std::memcpy(bytes, &session.packet, 4);
    std::memcpy(bytes + 4, &session.buttons, 2);
    std::memcpy(bytes + 6, session.analog, 8);
    std::memcpy(bytes + 14, session.thumbs, 8);
    return 1;
}

extern "C" void host_h2_mouse_take(void* look) {
    /* H2_MOUSE_SENSITIVITY scales 0.003 radians a point */
    static const float scale = [] {
        const char* text = std::getenv("H2_MOUSE_SENSITIVITY");
        const float multiplier = text && *text ? std::strtof(text, nullptr) : 1.0f;
        return 0.003f * (multiplier > 0.0f ? multiplier : 1.0f);
    }();
    auto& session = game_session();
    const float radians[2] = {session.mouse_x * scale, session.mouse_y * scale};
    std::memcpy(look, radians, sizeof(radians));
    session.mouse_x = 0;
    session.mouse_y = 0;
}

extern "C" unsigned int host_h2_start_map(char* buffer, unsigned int size) {
    const char* name = std::getenv("H2_START_MAP");
    if (!name || !*name || !buffer || !size)
        return 0;
    const std::string map = std::filesystem::path(name).stem().string();
    if (map.size() + 1 > size)
        return 0;
    std::memcpy(buffer, map.c_str(), map.size() + 1);
    std::cerr << "h2-port: the first game starts at " << map << '\n';
    return static_cast<unsigned int>(map.size());
}

/* the next "guest ..." control line, or 0 when none is waiting */
extern "C" unsigned int host_h2_guest_command(char* buffer, unsigned int size) {
    if (g_guest_commands.empty() || !buffer || !size)
        return 0;
    const std::string command = g_guest_commands.front();
    g_guest_commands.pop_front();
    const std::size_t length = std::min<std::size_t>(command.size(), size - 1);
    std::memcpy(buffer, command.data(), length);
    buffer[length] = 0;
    return static_cast<unsigned int>(length);
}

extern "C" unsigned int host_h2_trace_flags(void) {
    static const unsigned int flags = [] {
        const char* trace = std::getenv("H2_TRACE");
        /* 1: D3D method counts; 2: the guest's per-frame player state */
        return (trace && std::strstr(trace, "d3d") ? 1u : 0u) | (trace && std::strstr(trace, "state") ? 2u : 0u);
    }();
    return flags;
}

extern "C" unsigned int host_h2_input_connected(void) {
    return 1u; /* the keyboard and the first gamepad share port 0 */
}

namespace {

} // namespace

int main(int argc, char** argv) {
    install_fault_reporter();
    Options options;
    if (!parse_options(argc, argv, options)) {
        usage(argv[0]);
        return 2;
    }

    if (!options.guest_entry.empty() && options.guest_image.empty()) {
        std::cerr << "error: --guest-entry requires --guest-image\n";
        return 2;
    }

    const auto report = h2::port::inspect_retail_data(options.data_root);
    if (!report.ready) {
        for (const auto& error : report.errors) {
            std::cerr << "error: " << error << '\n';
        }
        return 1;
    }

    std::string arena_error;
    auto arena = h2::port::GuestArena::create(arena_error);
    if (!arena) {
        std::cerr << "error: " << arena_error << '\n';
        return 1;
    }
    h2::port::XbeLoadReport image;
    if (!arena->load_xbe(options.data_root / "default.xbe", image, arena_error)) {
        std::cerr << "error: " << arena_error << '\n';
        return 1;
    }

    std::optional<h2::port::GuestElfImage> guest_image;
    std::optional<h2::port::AsyncTextureGuestEntries> texture_controller_entries;
    std::optional<AsyncTextureControllerGuard> texture_controller;
    std::optional<h2::port::HostServiceStats> guest_service_stats;
    GuestEntry guest_frame = nullptr;
    GuestEntry guest_press_start = nullptr;
    GuestEntry guest_accept = nullptr;
    GuestEntry guest_campaign_load = nullptr;
    GuestEntry guest_up = nullptr;
    GuestEntry guest_down = nullptr;
    GuestEntry guest_cancel = nullptr;
    void* guest_stack_top = nullptr;
    if (!options.guest_image.empty()) {
        if (!h2::port::configure_cache_services(
                *arena, options.data_root / "maps" / "mainmenu.map", arena_error) ||
            !h2::port::configure_profile_services(options.profile_root, arena_error)) {
            std::cerr << "error: " << arena_error << '\n';
            return 1;
        }
        guest_image = h2::port::GuestElfImage::load(options.guest_image, *arena,
                                                    arena_error);
        if (!guest_image ||
            !h2::port::patch_standard_host_imports(
                *guest_image, *arena, arena_error) ||
            !h2::port::patch_async_host_imports(
                *guest_image, *arena, arena_error) ||
            !h2::port::patch_xbox_system_imports(*guest_image, *arena, arena_error)) {
            std::cerr << "error: " << arena_error << '\n';
            return 1;
        }
        {
            /* the game session's frame and controller services (whole images) */
            const std::pair<const char*, std::uintptr_t> session_imports[] = {
                {"host_h2_frame", reinterpret_cast<std::uintptr_t>(&host_h2_frame)},
                {"host_h2_input_state", reinterpret_cast<std::uintptr_t>(&host_h2_input_state)},
                {"host_h2_input_connected", reinterpret_cast<std::uintptr_t>(&host_h2_input_connected)},
                {"host_h2_mouse_take", reinterpret_cast<std::uintptr_t>(&host_h2_mouse_take)},
                {"host_h2_trace_flags", reinterpret_cast<std::uintptr_t>(&host_h2_trace_flags)},
                {"host_h2_gpu_draw", reinterpret_cast<std::uintptr_t>(&host_h2_gpu_draw)},
                {"host_h2_gpu_clear", reinterpret_cast<std::uintptr_t>(&host_h2_gpu_clear)},
                {"host_h2_gpu_visibility_end", reinterpret_cast<std::uintptr_t>(&host_h2_gpu_visibility_end)},
                {"host_h2_gpu_visibility_result", reinterpret_cast<std::uintptr_t>(&host_h2_gpu_visibility_result)},
                {"host_h2_audio_command", reinterpret_cast<std::uintptr_t>(&host_h2_audio_command)},
                {"host_h2_audio_status", reinterpret_cast<std::uintptr_t>(&host_h2_audio_status)},
                {"host_h2_start_map", reinterpret_cast<std::uintptr_t>(&host_h2_start_map)},
                {"host_h2_guest_command", reinterpret_cast<std::uintptr_t>(&host_h2_guest_command)},
                {"host_h2_wma_open", reinterpret_cast<std::uintptr_t>(&host_h2_wma_open)},
                {"host_h2_wma_decode", reinterpret_cast<std::uintptr_t>(&host_h2_wma_decode)},
                {"host_h2_wma_close", reinterpret_cast<std::uintptr_t>(&host_h2_wma_close)},
            };
            for (const auto& [symbol, target] : session_imports) {
                if (guest_image->symbol_address(std::string("h2_host_slot_") + symbol) &&
                    !guest_image->patch_host_import(*arena, symbol, target, arena_error)) {
                    std::cerr << "error: " << arena_error << '\n';
                    return 1;
                }
            }
        }
        std::size_t relocated_code_pointers = 0;
        if (!h2::port::apply_retail_code_map(*guest_image, *arena, image.sections,
                                             relocated_code_pointers, arena_error)) {
            std::cerr << "error: " << arena_error << '\n';
            return 1;
        }
        if (relocated_code_pointers)
            std::cout << "Relocated " << relocated_code_pointers
                      << " retail code pointers to native code" << std::endl;
        if (options.guest_entry == "h2_main_menu_guest_bootstrap") {
            h2::port::AsyncTextureGuestEntries entries{};
            const auto resolve_entry = [&](const char* symbol,
                                           std::uint32_t& result) {
                const auto address = guest_image->symbol_address(symbol);
                if (!address ||
                    *address > std::numeric_limits<std::uint32_t>::max()) {
                    arena_error = std::string("async texture controller entry is missing: ") +
                                  symbol;
                    return false;
                }
                result = static_cast<std::uint32_t>(*address);
                return true;
            };
            if (!resolve_entry("h2_native_async_worker_step", entries.worker_step) ||
                !resolve_entry("h2_native_async_queue_initialize",
                               entries.queue_initialize) ||
                !resolve_entry("h2_native_async_queue_teardown",
                               entries.queue_teardown) ||
                !resolve_entry("h2_native_texture_cache_flush",
                               entries.texture_flush) ||
                !resolve_entry("?texture_cache_block_delete",
                               entries.block_delete) ||
                !resolve_entry("?texture_cache_entry_busy",
                               entries.entry_busy) ||
                !resolve_entry("?texture_cache_entry_state",
                               entries.entry_state)) {
                std::cerr << "error: " << arena_error << '\n';
                return 1;
            }
            texture_controller_entries = entries;
        }
        if (!options.guest_entry.empty()) {
            const auto address = guest_image->symbol_address(options.guest_entry);
            if (!address) {
                std::cerr << "error: guest entry symbol not found: "
                          << options.guest_entry << '\n';
                return 1;
            }
            if (!arena->protect(kGuestStackAddress, kGuestStackSize,
                                h2::port::GuestMemoryProtection::read_write,
                                arena_error)) {
                std::cerr << "error: could not prepare guest stack: "
                          << arena_error << '\n';
                return 1;
            }
            const auto entry = reinterpret_cast<GuestEntry>(arena->pointer(*address));
            void* const stack_top = arena->pointer(
                kGuestStackAddress + kGuestStackSize - 16);
            if (options.guest_entry == "h2_whole_game_main" &&
                (!h2::port::configure_async_services(*arena, arena_error) ||
                 !h2::port::configure_xbox_system(*arena, options.data_root,
                                                  options.profile_root / "title",
                                                  arena_error) ||
                 !open_game_session(options.headless, options.frame_count, options.screenshot,
                                    arena.get(), options.data_root))) {
                std::cerr << "error: guest async services: " << arena_error << '\n';
                return 1;
            }
            h2::port::reset_host_service_stats();
            const int result = h2_call_guest_int_0(entry, stack_top);
            guest_service_stats = h2::port::host_service_stats();
            if (result != 0) {
                std::cerr << "error: guest entry returned " << result << '\n';
                return 1;
            }
            if (options.guest_entry == "h2_main_menu_guest_bootstrap" &&
                guest_service_stats->render_request_calls == 0) {
                std::cerr << "error: main-menu bootstrap produced no render requests\n";
                return 1;
            }
            if (options.guest_entry == "h2_main_menu_guest_bootstrap" &&
                guest_service_stats->text_request_calls == 0) {
                std::cerr << "error: main-menu bootstrap produced no localized text requests\n";
                return 1;
            }
            if (options.guest_entry == "h2_main_menu_guest_bootstrap") {
                const auto frame_address =
                    guest_image->symbol_address("h2_main_menu_guest_frame");
                if (!frame_address) {
                    std::cerr << "error: main-menu frame entry is missing\n";
                    return 1;
                }
                const auto press_start_address =
                    guest_image->symbol_address("h2_main_menu_guest_press_start");
                if (!press_start_address) {
                    std::cerr << "error: main-menu press-start entry is missing\n";
                    return 1;
                }
                const auto accept_address =
                    guest_image->symbol_address("h2_main_menu_guest_accept");
                if (!accept_address) {
                    std::cerr << "error: main-menu accept entry is missing\n";
                    return 1;
                }
                const auto campaign_load_address =
                    guest_image->symbol_address("h2_main_menu_guest_load_selected_campaign");
                if (!campaign_load_address) {
                    std::cerr << "error: selected Campaign cache-load entry is missing\n";
                    return 1;
                }
                const auto up_address =
                    guest_image->symbol_address("h2_main_menu_guest_up");
                const auto down_address =
                    guest_image->symbol_address("h2_main_menu_guest_down");
                if (!up_address || !down_address) {
                    std::cerr << "error: main-menu directional entries are missing\n";
                    return 1;
                }
                const auto cancel_address =
                    guest_image->symbol_address("h2_main_menu_guest_cancel");
                if (!cancel_address) {
                    std::cerr << "error: main-menu cancel entry is missing\n";
                    return 1;
                }
                guest_frame = reinterpret_cast<GuestEntry>(
                    arena->pointer(*frame_address));
                guest_press_start = reinterpret_cast<GuestEntry>(
                    arena->pointer(*press_start_address));
                guest_accept = reinterpret_cast<GuestEntry>(
                    arena->pointer(*accept_address));
                guest_campaign_load = reinterpret_cast<GuestEntry>(
                    arena->pointer(*campaign_load_address));
                guest_up = reinterpret_cast<GuestEntry>(
                    arena->pointer(*up_address));
                guest_down = reinterpret_cast<GuestEntry>(
                    arena->pointer(*down_address));
                guest_cancel = reinterpret_cast<GuestEntry>(
                    arena->pointer(*cancel_address));
                guest_stack_top = stack_top;

                const std::array cache_files{
                    h2::port::AsyncCacheFileConfiguration{
                        options.data_root / "maps" / "mainmenu.map", 2,
                        "mainmenu", true},
                    h2::port::AsyncCacheFileConfiguration{
                        options.data_root / "maps" / "shared.map", 3,
                        "shared", false},
                    h2::port::AsyncCacheFileConfiguration{
                        options.data_root / "maps" /
                            "single_player_shared.map",
                        4, "single_player_shared", false},
                };
                if (!texture_controller_entries ||
                    !h2::port::start_async_texture_controller(
                        *arena, cache_files, *texture_controller_entries,
                        arena_error)) {
                    std::cerr << "error: " << arena_error << '\n';
                    return 1;
                }
                texture_controller.emplace(*texture_controller_entries);
                texture_controller->activate();
            }
        }
    }

    std::cout << "Supported retail Halo 2 data verified at " << options.data_root << '\n'
              << "default.xbe sha256 " << report.xbe_sha256 << '\n'
              << "cache build " << report.cache_build << ", map "
              << report.main_menu_name << '\n'
              << "Initialized " << image.sections.size()
              << " XBE sections in non-executable guest memory at 0x" << std::hex
              << h2::port::kGuestBias << std::dec << '\n';
    if (guest_image) {
        std::cout << "Loaded " << guest_image->segments().size()
                  << " protected guest ELF segments and patched native host services\n";
        if (!options.guest_entry.empty()) {
            std::cout << "Executed guest entry " << options.guest_entry
                      << " successfully\n";
            if (guest_service_stats) {
                std::cout << "Guest host requests: render "
                          << guest_service_stats->render_request_calls << ", text "
                          << guest_service_stats->text_request_calls << ", video "
                          << guest_service_stats->video_control_calls << '\n';
            }
        }
    }
    std::cout << "Native host preflight passed; game boot is not implemented yet.\n";
    const int result = options.headless
        ? 0
        : run_window(guest_frame, guest_press_start, guest_accept, guest_campaign_load,
                     guest_up, guest_down, guest_cancel, guest_stack_top,
                     options.frame_count,
                     options.press_start_frame, options.accept_frame, options.select_frame,
                     options.finish_frame, options.menu_accept_frame,
                     options.campaign_accept_frame, options.level_accept_frame,
                     options.campaign_load_frame, options.down_frame,
                     options.up_frame, options.cancel_frame,
                     options.screenshot,
                     arena.get(), options.data_root);
    if (texture_controller && !texture_controller->stop(arena_error)) {
        std::cerr << "error: async texture controller teardown failed: "
                  << arena_error << '\n';
        return 1;
    }
    return result;
}
