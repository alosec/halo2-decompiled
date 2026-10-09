#include "h2/host_abi.h"
#include "h2/host_services.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>

namespace {

struct CallbackCapture {
    int command = 0;
    const void* request = nullptr;
    const void* vertices = nullptr;
    const std::uint32_t* text = nullptr;
    const void* bounds = nullptr;
    int line_gap = 0;
    float scale = 0.0f;
    std::uint32_t color = 0;
    int video_calls = 0;
    int render_calls = 0;
    int text_calls = 0;
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
}

void capture_text(const std::uint32_t* text, const void* bounds, const void*,
                  const void*, int line_gap, float scale, std::uint32_t color,
                  const void*, void* context) {
    auto& capture = *static_cast<CallbackCapture*>(context);
    capture.text = text;
    capture.bounds = bounds;
    capture.line_gap = line_gap;
    capture.scale = scale;
    capture.color = color;
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

    const unsigned int before = host_h2_get_milliseconds();
    host_h2_sleep(2);
    const unsigned int after = host_h2_get_milliseconds();
    ok &= check(after - before >= 1, "host monotonic clock did not advance");

    CallbackCapture capture;
    h2::port::reset_host_service_stats();
    h2::port::set_video_control_handler(capture_video, &capture);
    h2::port::set_render_request_handler(capture_render, &capture);
    h2::port::set_text_request_handler(capture_text, &capture);
    const auto* request = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x1234));
    const auto* vertices = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x5678));
    const std::uint32_t text[] = {'H', '2', 0};
    const auto* bounds = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x9abc));
    host_h2_video_control(7);
    host_h2_render_request(request, vertices);
    host_h2_text_request(text, bounds, nullptr, nullptr, 3, 1.5f, 0xff00ff00u, nullptr);
    const auto stats = h2::port::host_service_stats();
    ok &= check(capture.command == 7 && capture.video_calls == 1,
                "video-control callback did not receive the guest command");
    ok &= check(capture.request == request && capture.vertices == vertices &&
                    capture.render_calls == 1,
                "render callback did not receive the guest pointers");
    ok &= check(capture.text == text && capture.bounds == bounds &&
                    capture.line_gap == 3 && capture.scale == 1.5f &&
                    capture.color == 0xff00ff00u && capture.text_calls == 1,
                "text callback did not receive the guest text request");
    ok &= check(stats.video_control_calls == 1 && stats.render_request_calls == 1 &&
                    stats.text_request_calls == 1,
                "host service counters are incorrect");

    h2::port::set_video_control_handler(nullptr);
    h2::port::set_render_request_handler(nullptr);
    h2::port::set_text_request_handler(nullptr);
    if (ok) {
        std::cout << "native host ABI services passed\n";
    }
    return ok ? 0 : 1;
}
