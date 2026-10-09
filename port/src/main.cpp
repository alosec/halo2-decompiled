#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"
#include "h2/retail_data.hpp"

#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>

#include <SDL3/SDL.h>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

namespace {

constexpr std::uint32_t kGuestStackAddress = 0x20000000u;
constexpr std::uint32_t kGuestStackSize = 0x00100000u;

struct Options {
    std::filesystem::path data_root;
    std::filesystem::path guest_image;
    std::string guest_entry;
    bool headless = false;
};

void usage(const char* program) {
    std::cerr << "Usage: " << program
              << " [--headless] [--data-root PATH] [--guest-image PATH]"
                 " [--guest-entry SYMBOL]\n";
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
        } else {
            return false;
        }
    }
    return !options.data_root.empty();
}

int run_window() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Halo 2 native port - retail data verified, engine bootstrap pending", 960, 540,
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

    bool running = true;
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
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
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
    if (!options.guest_image.empty()) {
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
            using GuestEntry = int (*)();
            const auto entry = reinterpret_cast<GuestEntry>(arena->pointer(*address));
            void* const stack_top = arena->pointer(
                kGuestStackAddress + kGuestStackSize - 16);
            const int result = h2_call_guest_int_0(entry, stack_top);
            if (result != 0) {
                std::cerr << "error: guest entry returned " << result << '\n';
                return 1;
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
        }
    }
    std::cout << "Native host preflight passed; game boot is not implemented yet.\n";
    return options.headless ? 0 : run_window();
}
