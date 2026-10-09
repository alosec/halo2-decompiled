#include "h2/font_decode.hpp"
#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"
#include "h2/render_protocol.h"
#include "h2/retail_data.hpp"
#include "h2/texture_decode.hpp"

#include <array>
#include <charconv>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

namespace {

constexpr std::uint32_t kGuestStackAddress = 0x20000000u;
constexpr std::uint32_t kGuestStackSize = 0x00100000u;

using GuestEntry = int (*)();

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
        const bool supported = cache_format == 16 ||
            (cache_format == 19 && source_format == 10 && !(flags & 0x10u));
        if (supported && width > 0 && height > 0 && width <= 4096 &&
            height <= 4096 && data_offset != 0xffffffffu && data_size != 0 &&
            data_size <= 64u * 1024u * 1024u) {
            std::vector<unsigned char> encoded(data_size);
            std::vector<unsigned char> pixels;
            const bool copied = h2::port::copy_cache_bytes(
                data_offset, encoded.data(), encoded.size());
            bool decoded = false;
            if (copied && cache_format == 16) {
                decoded = (flags & 0x10u)
                    ? h2::port::decode_dxt5_linear(encoded, width, height, pixels)
                    : h2::port::decode_dxt5(encoded, width, height, pixels);
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
                      const GuestRectangle& bounds, float scale, std::uint32_t color,
                      const h2_text_request_metadata& metadata) {
    CachedFont* font = get_font(context, metadata.font);
    if (!font) {
        return false;
    }
    const float glyph_scale = scale > 0.0f ? scale : 1.0f;
    std::size_t count = 0;
    float width = 0.0f;
    while (count < 2048 && text[count] && text[count] != '\r') {
        CachedFontGlyph* glyph = get_font_glyph(context, *font, text[count]);
        if (!glyph) {
            return false;
        }
        width += glyph->metrics.advance * glyph_scale;
        ++count;
    }
    float x = static_cast<float>(bounds.left) + font->metrics.initial_offset * glyph_scale;
    if (metadata.justification == 1) {
        x = static_cast<float>(bounds.right) - width -
            font->metrics.initial_offset * glyph_scale;
    } else if (metadata.justification == 2) {
        x = (static_cast<float>(bounds.left) + bounds.right - width) * 0.5f;
    }
    const float baseline = bounds.top + font->metrics.ascending_height * glyph_scale;
    const auto draw_pass = [&](std::uint32_t pass_color, float offset) {
        const SDL_FColor modulation = unpack_color(pass_color);
        float cursor = x;
        for (std::size_t i = 0; i < count; ++i) {
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
                     const void*, const void*, int, float scale,
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
            draw_retail_text(context, text, bounds, scale, color, metadata)) {
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
                 " [--menu-accept-frame COUNT] [--campaign-accept-frame COUNT] [--down-frame COUNT] [--up-frame COUNT] [--cancel-frame COUNT]"
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
               GuestEntry guest_accept, GuestEntry guest_up, GuestEntry guest_down,
               GuestEntry guest_cancel, void* guest_stack_top,
               int frame_count, int press_start_frame, int accept_frame, int select_frame,
               int finish_frame, int menu_accept_frame, int campaign_accept_frame,
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

int main(int argc, char** argv) {
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
    std::optional<h2::port::HostServiceStats> guest_service_stats;
    GuestEntry guest_frame = nullptr;
    GuestEntry guest_press_start = nullptr;
    GuestEntry guest_accept = nullptr;
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
        if (!guest_image || !h2::port::patch_standard_host_imports(
                                *guest_image, *arena, arena_error)) {
            std::cerr << "error: " << arena_error << '\n';
            return 1;
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
                guest_up = reinterpret_cast<GuestEntry>(
                    arena->pointer(*up_address));
                guest_down = reinterpret_cast<GuestEntry>(
                    arena->pointer(*down_address));
                guest_cancel = reinterpret_cast<GuestEntry>(
                    arena->pointer(*cancel_address));
                guest_stack_top = stack_top;
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
    return options.headless
        ? 0
        : run_window(guest_frame, guest_press_start, guest_accept, guest_up, guest_down,
                     guest_cancel, guest_stack_top, options.frame_count,
                     options.press_start_frame, options.accept_frame, options.select_frame,
                     options.finish_frame, options.menu_accept_frame,
                     options.campaign_accept_frame, options.down_frame,
                     options.up_frame, options.cancel_frame,
                     options.screenshot,
                     arena.get(), options.data_root);
}
