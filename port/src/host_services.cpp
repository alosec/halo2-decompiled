#include "h2/host_abi.h"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>

namespace {

using Clock = std::chrono::steady_clock;

struct DispatchState {
    std::mutex mutex;
    h2::port::VideoControlHandler video_handler = nullptr;
    void* video_context = nullptr;
    h2::port::RenderRequestHandler render_handler = nullptr;
    void* render_context = nullptr;
    h2::port::TextRequestHandler text_handler = nullptr;
    void* text_context = nullptr;
    std::atomic<std::uint64_t> video_calls{0};
    std::atomic<std::uint64_t> render_calls{0};
    std::atomic<std::uint64_t> text_calls{0};
};

DispatchState& dispatch_state() {
    static DispatchState state;
    return state;
}

const Clock::time_point kClockStart = Clock::now();

template <typename Function>
std::uintptr_t function_address(Function function) {
    static_assert(sizeof(function) == sizeof(std::uintptr_t));
    std::uintptr_t address = 0;
    std::memcpy(&address, &function, sizeof(address));
    return address;
}

}  // namespace

namespace h2::port {

void set_video_control_handler(VideoControlHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.video_handler = handler;
    state.video_context = context;
}

void set_render_request_handler(RenderRequestHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.render_handler = handler;
    state.render_context = context;
}

void set_text_request_handler(TextRequestHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.text_handler = handler;
    state.text_context = context;
}

HostServiceStats host_service_stats() {
    auto& state = dispatch_state();
    return {state.video_calls.load(std::memory_order_relaxed),
            state.render_calls.load(std::memory_order_relaxed),
            state.text_calls.load(std::memory_order_relaxed)};
}

void reset_host_service_stats() {
    auto& state = dispatch_state();
    state.video_calls.store(0, std::memory_order_relaxed);
    state.render_calls.store(0, std::memory_order_relaxed);
    state.text_calls.store(0, std::memory_order_relaxed);
}

bool patch_standard_host_imports(GuestElfImage& image, GuestArena& arena,
                                 std::string& error) {
    struct Import {
        const char* symbol;
        std::uintptr_t target;
    };
    const Import imports[] = {
        {"host_h2_video_control", function_address(&host_h2_video_control)},
        {"host_h2_render_request", function_address(&host_h2_render_request)},
        {"host_h2_text_request", function_address(&host_h2_text_request)},
        {"host_h2_get_milliseconds", function_address(&host_h2_get_milliseconds)},
        {"host_h2_sleep", function_address(&host_h2_sleep)},
        {"host_h2_cos", function_address(&host_h2_cos)},
    };
    for (const auto& import : imports) {
        if (!image.patch_host_import(arena, import.symbol, import.target, error)) {
            return false;
        }
    }
    return true;
}

}  // namespace h2::port

extern "C" unsigned int host_h2_abi_version() {
    return H2_HOST_ABI_VERSION;
}

extern "C" void host_h2_log(const char* message) {
    if (message) {
        std::cerr << "guest: " << message << '\n';
    }
}

extern "C" void host_h2_video_control(int command) {
    auto& state = dispatch_state();
    state.video_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::VideoControlHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.video_handler;
        context = state.video_context;
    }
    if (handler) {
        handler(command, context);
    }
}

extern "C" void host_h2_render_request(const void* request, const void* vertices) {
    auto& state = dispatch_state();
    state.render_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::RenderRequestHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.render_handler;
        context = state.render_context;
    }
    if (handler) {
        handler(request, vertices, context);
    }
}

extern "C" void host_h2_text_request(const unsigned int* text, const void* bounds,
                                       const void* clip, const void* position,
                                       int line_gap, float scale, unsigned int color,
                                       const void* shadow) {
    auto& state = dispatch_state();
    state.text_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::TextRequestHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.text_handler;
        context = state.text_context;
    }
    if (handler) {
        handler(text, bounds, clip, position, line_gap, scale, color, shadow, context);
    }
}

extern "C" unsigned int host_h2_get_milliseconds() {
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - kClockStart);
    return static_cast<unsigned int>(elapsed.count());
}

extern "C" void host_h2_sleep(unsigned int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

extern "C" double host_h2_cos(double value) {
    return std::cos(value);
}
