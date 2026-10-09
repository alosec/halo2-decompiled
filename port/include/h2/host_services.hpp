#ifndef H2_PORT_HOST_SERVICES_HPP
#define H2_PORT_HOST_SERVICES_HPP

#include <cstdint>
#include <string>

namespace h2::port {

class GuestArena;
class GuestElfImage;

using VideoControlHandler = void (*)(int command, void* context);
using RenderRequestHandler = void (*)(const void* request, const void* vertices,
                                      void* context);
using TextRequestHandler = void (*)(const std::uint32_t* text, const void* bounds,
                                    const void* clip, const void* position,
                                    int line_gap, float scale, std::uint32_t color,
                                    const void* shadow, void* context);

struct HostServiceStats {
    std::uint64_t video_control_calls;
    std::uint64_t render_request_calls;
    std::uint64_t text_request_calls;
};

void set_video_control_handler(VideoControlHandler handler, void* context = nullptr);
void set_render_request_handler(RenderRequestHandler handler, void* context = nullptr);
void set_text_request_handler(TextRequestHandler handler, void* context = nullptr);
HostServiceStats host_service_stats();
void reset_host_service_stats();
bool patch_standard_host_imports(GuestElfImage& image, GuestArena& arena,
                                 std::string& error);

}  // namespace h2::port

#endif
