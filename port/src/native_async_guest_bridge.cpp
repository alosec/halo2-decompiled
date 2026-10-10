#include "h2/native_async_guest_bridge.hpp"

#include "h2/guest_arena.hpp"

namespace h2::port {

NativeAsyncGuestBridge::NativeAsyncGuestBridge(GuestArena& arena,
                                               NativeAsyncPlatform& platform)
    : arena_(arena), platform_(platform) {}

NativeAsyncGuestBridge::Handle NativeAsyncGuestBridge::create_mutex(bool initial_owner) {
    return platform_.create_mutex(initial_owner);
}

NativeAsyncGuestBridge::Handle NativeAsyncGuestBridge::create_semaphore(
    std::int32_t initial_count, std::int32_t maximum_count) {
    return platform_.create_semaphore(initial_count, maximum_count);
}

NativeAsyncGuestBridge::Handle NativeAsyncGuestBridge::open_read_only_host(
    const std::filesystem::path& path) {
    return platform_.open_read_only(path);
}

bool NativeAsyncGuestBridge::set_thread_priority(Handle handle,
                                                 std::int32_t priority) {
    return platform_.set_thread_priority(handle, priority);
}

bool NativeAsyncGuestBridge::thread_exit_code(
    Handle handle, std::uint32_t* guest_exit_code) const {
    if (!guest_read_write(guest_exit_code, sizeof(*guest_exit_code)))
        return false;
    return platform_.thread_exit_code(handle, guest_exit_code);
}

bool NativeAsyncGuestBridge::set_file_pointer(
    Handle handle, std::int32_t distance, std::int32_t* guest_distance_high,
    std::uint32_t method, std::uint32_t* guest_position_low) {
    if ((guest_distance_high &&
         !guest_read_write(guest_distance_high, sizeof(*guest_distance_high))) ||
        (guest_position_low &&
         !guest_read_write(guest_position_low, sizeof(*guest_position_low))))
        return false;
    return platform_.set_file_pointer(handle, distance, guest_distance_high, method,
                                      guest_position_low);
}

bool NativeAsyncGuestBridge::read_file(Handle handle, void* guest_buffer,
                                       std::uint32_t bytes_to_read,
                                       std::uint32_t* guest_bytes_read) {
    if ((bytes_to_read && !guest_read_write(guest_buffer, bytes_to_read)) ||
        (guest_bytes_read &&
         !guest_read_write(guest_bytes_read, sizeof(*guest_bytes_read))))
        return false;
    return platform_.read_file(handle, guest_buffer, bytes_to_read, guest_bytes_read);
}

NativeAsyncGuestBridge::WaitResult NativeAsyncGuestBridge::wait(
    Handle handle, std::uint32_t milliseconds) {
    return platform_.wait(handle, milliseconds);
}

bool NativeAsyncGuestBridge::release_mutex(Handle handle) {
    return platform_.release_mutex(handle);
}

bool NativeAsyncGuestBridge::release_semaphore(
    Handle handle, std::int32_t release_count,
    std::int32_t* guest_previous_count) {
    if (guest_previous_count &&
        !guest_read_write(guest_previous_count, sizeof(*guest_previous_count)))
        return false;
    return platform_.release_semaphore(handle, release_count, guest_previous_count);
}

bool NativeAsyncGuestBridge::close(Handle handle) {
    return platform_.close(handle);
}

bool NativeAsyncGuestBridge::guest_read_write(const void* pointer,
                                               std::size_t size) const {
    return arena_.host_range_accessible(pointer, size, true);
}

} // namespace h2::port
