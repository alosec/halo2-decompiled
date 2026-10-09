#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"
#include "h2/render_protocol.h"
#include "h2/retail_data.hpp"
#include "h2/texture_decode.hpp"

#include <charconv>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <filesystem>
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

struct NativeRenderContext {
    SDL_Renderer* renderer = nullptr;
    const h2::port::GuestArena* arena = nullptr;
    std::unordered_map<std::uint32_t, SDL_Texture*> textures;
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
        const unsigned int format = descriptor[0x17];
        const std::uint32_t data_offset = read_u32(descriptor + 0x1c);
        const std::uint32_t data_size = read_u32(descriptor + 0x34);
        if (format == 16 && width > 0 && height > 0 && width <= 4096 &&
            height <= 4096 && data_offset != 0xffffffffu && data_size != 0 &&
            data_size <= 64u * 1024u * 1024u) {
            std::vector<unsigned char> compressed(data_size);
            std::vector<unsigned char> pixels;
            if (h2::port::copy_cache_bytes(data_offset, compressed.data(),
                                            compressed.size()) &&
                h2::port::decode_dxt5(compressed, width, height, pixels)) {
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

void draw_guest_text(const std::uint32_t* text, const void* bounds_pointer,
                     const void*, const void*, int, float scale,
                     std::uint32_t color, const void*, void* context_pointer) {
    auto& context = *static_cast<NativeRenderContext*>(context_pointer);
    if (!text || !bounds_pointer || context.failed) {
        return;
    }
    std::string utf8;
    for (std::size_t i = 0; i < 2048 && text[i]; ++i) {
        const std::uint32_t character = text[i];
        if (character >= 0x20 && character <= 0x7e) {
            utf8.push_back(static_cast<char>(character));
        } else {
            utf8.push_back('?');
        }
    }
    if (utf8.empty()) {
        return;
    }
    const auto& bounds = *static_cast<const GuestRectangle*>(bounds_pointer);
    const float glyph_scale = scale > 0.0f ? scale : 1.0f;
    const float width = static_cast<float>(utf8.size()) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE *
                        glyph_scale;
    const float height = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * glyph_scale;
    const float x = (static_cast<float>(bounds.left) + bounds.right - width) * 0.5f;
    const float y = (static_cast<float>(bounds.top) + bounds.bottom - height) * 0.5f;
    const SDL_FColor unpacked = unpack_color(color);
    if (!SDL_SetRenderDrawColorFloat(context.renderer, unpacked.r, unpacked.g,
                                     unpacked.b, unpacked.a) ||
        !SDL_RenderDebugText(context.renderer, x, y, utf8.c_str())) {
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
    SDL_Vertex vertices[H2_RENDER_IMMEDIATE_MAX_VERTICES]{};
    const SDL_FColor color = unpack_color(request.color);
    for (unsigned int i = 0; i < request.vertex_count; ++i) {
        vertices[i].position = {source[i].x, source[i].y};
        vertices[i].color = color;
        vertices[i].tex_coord = {source[i].texture_x, source[i].texture_y};
    }
    static constexpr int indices[] = {0, 1, 2, 0, 2, 3};
    const int index_count = request.vertex_count == 3 ? 3 : 6;
    SDL_Texture* texture = nullptr;
    if (request.kind == H2_RENDER_REQUEST_INTERFACE_QUAD &&
        request.texture_tag != -1) {
        texture = get_interface_texture(
            context, static_cast<std::uint32_t>(request.texture_tag));
    }
    if (!SDL_RenderGeometry(context.renderer, texture, vertices,
                            static_cast<int>(request.vertex_count), indices,
                            index_count)) {
        context.failed = true;
    }
}

struct Options {
    std::filesystem::path data_root;
    std::filesystem::path guest_image;
    std::string guest_entry;
    std::filesystem::path screenshot;
    int frame_count = 0;
    bool headless = false;
};

void usage(const char* program) {
    std::cerr << "Usage: " << program
              << " [--headless] [--data-root PATH] [--guest-image PATH]"
                 " [--guest-entry SYMBOL] [--frames COUNT] [--screenshot BMP]\n";
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
        } else if (argument == "--screenshot" && i + 1 < argc) {
            options.screenshot = argv[++i];
        } else {
            return false;
        }
    }
    return !options.data_root.empty() &&
        (options.screenshot.empty() || options.frame_count > 0);
}

int run_window(GuestEntry guest_frame, void* guest_stack_top, int frame_count,
               const std::filesystem::path& screenshot,
               const h2::port::GuestArena* arena) {
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

    NativeRenderContext render_context{renderer, arena, {}, false};
    h2::port::set_render_request_handler(draw_guest_request, &render_context);
    h2::port::set_text_request_handler(draw_guest_text, &render_context);
    bool running = true;
    int result = 0;
    int rendered_frames = 0;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running = false;
            }
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
        std::cout << "Rendered " << rendered_frames << " host frames; guest requests: render "
                  << stats.render_request_calls << ", text " << stats.text_request_calls
                  << ", video " << stats.video_control_calls << "; decoded textures "
                  << decoded_textures << '/' << render_context.textures.size() << '\n';
    }
    for (const auto& [address, texture] : render_context.textures) {
        (void)address;
        if (texture) {
            SDL_DestroyTexture(texture);
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
    void* guest_stack_top = nullptr;
    if (!options.guest_image.empty()) {
        if (!h2::port::configure_cache_services(
                *arena, options.data_root / "maps" / "mainmenu.map", arena_error)) {
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
                guest_frame = reinterpret_cast<GuestEntry>(
                    arena->pointer(*frame_address));
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
        : run_window(guest_frame, guest_stack_top, options.frame_count,
                     options.screenshot, arena.get());
}
