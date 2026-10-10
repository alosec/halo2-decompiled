#ifndef H2_PORT_HOST_SERVICES_HPP
#define H2_PORT_HOST_SERVICES_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

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
bool configure_cache_services(GuestArena& arena, const std::filesystem::path& map_path,
                              std::string& error);
void reset_cache_services();
bool configure_profile_services(const std::filesystem::path& directory,
                                std::string& error);
void reset_profile_services();
bool configure_async_services(GuestArena& arena, std::string& error);
bool initialize_async_queue(std::uint32_t guest_entry_address, std::string& error);
bool start_async_worker(std::uint32_t guest_entry_address, std::string& error);
bool shutdown_async_worker(std::uint32_t milliseconds, std::string& error);
bool teardown_async_queue(std::uint32_t guest_entry_address, std::string& error);
bool reset_async_services(std::string& error);
bool list_native_profiles(std::vector<std::uint32_t>& ids, std::string& error);
bool create_native_profile(std::span<const std::byte> record, std::uint32_t& id,
                           std::string& error);
bool read_native_profile(std::uint32_t id, std::span<std::byte> record,
                         std::string& error);
bool write_native_profile(std::uint32_t id, std::span<const std::byte> record,
                          std::string& error);
bool erase_native_profile(std::uint32_t id, std::string& error);
bool copy_cache_bytes(std::uint32_t offset, void* destination, std::size_t size);
bool patch_standard_host_imports(GuestElfImage& image, GuestArena& arena,
                                 std::string& error);
bool patch_async_host_imports(GuestElfImage& image, GuestArena& arena,
                              std::string& error);

}  // namespace h2::port

#endif
