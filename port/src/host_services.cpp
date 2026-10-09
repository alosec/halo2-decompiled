#include "h2/host_abi.h"
#include "h2/host_services.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
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
    std::atomic<std::uint64_t> video_calls{0};
    std::atomic<std::uint64_t> render_calls{0};
};

DispatchState& dispatch_state() {
    static DispatchState state;
    return state;
}

const Clock::time_point kClockStart = Clock::now();

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

HostServiceStats host_service_stats() {
    auto& state = dispatch_state();
    return {state.video_calls.load(std::memory_order_relaxed),
            state.render_calls.load(std::memory_order_relaxed)};
}

void reset_host_service_stats() {
    auto& state = dispatch_state();
    state.video_calls.store(0, std::memory_order_relaxed);
    state.render_calls.store(0, std::memory_order_relaxed);
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
