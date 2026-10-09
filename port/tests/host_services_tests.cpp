#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/render_protocol.h"

struct color4f { float alpha, red, green, blue; };
struct short_rectangle2d { short top, left, bottom, right; };

#include <cmath>
#include <cstdint>
#include <iostream>

extern "C" void D3DDevice_Begin(unsigned int primitive_type);
extern "C" void D3DDevice_End();
extern "C" void D3DDevice_SetVertexData2f(unsigned int vertex_register, float x, float y);
extern "C" void D3DDevice_SetVertexData4f(unsigned int vertex_register, float x, float y,
                                            float z, float w);
extern "C" void D3DDevice_SetVertexDataColor(unsigned int vertex_register,
                                               unsigned int color);
extern "C" void h2_render_preview_quad(long bitmap_tag, long bitmap_index,
                                         long layer, const void* vertices);
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
    ok &= check(capture.render_calls == 4 && solid_stats.render_request_calls == 4,
                "solid rectangle did not cross the host boundary once");

    h2::port::set_video_control_handler(nullptr);
    h2::port::set_render_request_handler(nullptr);
    h2::port::set_text_request_handler(nullptr);
    if (ok) {
        std::cout << "native host ABI services passed\n";
    }
    return ok ? 0 : 1;
}
