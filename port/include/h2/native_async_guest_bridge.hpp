#ifndef H2_PORT_NATIVE_ASYNC_GUEST_BRIDGE_HPP
#define H2_PORT_NATIVE_ASYNC_GUEST_BRIDGE_HPP

#include "h2/native_async_platform.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>

namespace h2::port {

class GuestArena;

class NativeAsyncGuestBridge {
public:
    using Handle = NativeAsyncPlatform::Handle;
    using WaitResult = NativeAsyncPlatform::WaitResult;

    NativeAsyncGuestBridge(GuestArena& arena, NativeAsyncPlatform& platform);

    Handle create_mutex(bool initial_owner);
    Handle create_semaphore(std::int32_t initial_count, std::int32_t maximum_count);
    Handle open_read_only_host(const std::filesystem::path& path);
    bool set_thread_priority(Handle handle, std::int32_t priority);
    bool thread_exit_code(Handle handle, std::uint32_t* guest_exit_code) const;
    bool set_file_pointer(Handle handle, std::int32_t distance,
                          std::int32_t* guest_distance_high, std::uint32_t method,
                          std::uint32_t* guest_position_low);
    bool read_file(Handle handle, void* guest_buffer, std::uint32_t bytes_to_read,
                   std::uint32_t* guest_bytes_read);
    WaitResult wait(Handle handle, std::uint32_t milliseconds);
    bool release_mutex(Handle handle);
    bool release_semaphore(Handle handle, std::int32_t release_count,
                           std::int32_t* guest_previous_count);
    bool close(Handle handle);

private:
    bool guest_read_write(const void* pointer, std::size_t size) const;

    GuestArena& arena_;
    NativeAsyncPlatform& platform_;
};

} // namespace h2::port

#endif
