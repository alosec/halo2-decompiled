#ifndef H2_PORT_HOST_SERVICES_HPP
#define H2_PORT_HOST_SERVICES_HPP

#include <cstdint>

namespace h2::port {

using VideoControlHandler = void (*)(int command, void* context);
using RenderRequestHandler = void (*)(const void* request, const void* vertices,
                                      void* context);

struct HostServiceStats {
    std::uint64_t video_control_calls;
    std::uint64_t render_request_calls;
};

void set_video_control_handler(VideoControlHandler handler, void* context = nullptr);
void set_render_request_handler(RenderRequestHandler handler, void* context = nullptr);
HostServiceStats host_service_stats();
void reset_host_service_stats();

}  // namespace h2::port

#endif
