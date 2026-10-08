#include "h2/guest_arena.hpp"
#include "h2/retail_data.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>

#include <SDL3/SDL.h>

namespace {

struct Options {
    std::filesystem::path data_root;
    bool headless = false;
};

void usage(const char* program) {
    std::cerr << "Usage: " << program << " [--headless] [--data-root PATH]\n";
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

    std::cout << "Supported retail Halo 2 data verified at " << options.data_root << '\n'
              << "default.xbe sha256 " << report.xbe_sha256 << '\n'
              << "cache build " << report.cache_build << ", map "
              << report.main_menu_name << '\n'
              << "Initialized " << image.sections.size()
              << " XBE sections in non-executable guest memory at 0x" << std::hex
              << h2::port::kGuestBias << std::dec << '\n'
              << "Native host preflight passed; game boot is not implemented yet.\n";
    return options.headless ? 0 : run_window();
}
