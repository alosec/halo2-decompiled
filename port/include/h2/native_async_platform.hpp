#ifndef H2_PORT_NATIVE_ASYNC_PLATFORM_HPP
#define H2_PORT_NATIVE_ASYNC_PLATFORM_HPP

#include <cstdint>
#include <filesystem>
#include <memory>

namespace h2::port {

class NativeAsyncPlatform {
public:
    using Handle = std::uint32_t;
    using ThreadStart = std::uint32_t (*)(void*);
    static constexpr Handle invalid_handle = 0;
    static constexpr std::uint32_t infinite = 0xffffffffu;

    enum class WaitResult {
        signaled,
        timeout,
        failed,
    };

    NativeAsyncPlatform();
    ~NativeAsyncPlatform();
    NativeAsyncPlatform(const NativeAsyncPlatform&) = delete;
    NativeAsyncPlatform& operator=(const NativeAsyncPlatform&) = delete;

    Handle create_mutex(bool initial_owner);
    Handle create_semaphore(std::int32_t initial_count, std::int32_t maximum_count);
    Handle open_read_only(const std::filesystem::path& path);
    Handle create_thread(std::uint32_t stack_size, ThreadStart start, void* parameter);
    bool set_thread_priority(Handle handle, std::int32_t priority);
    bool thread_exit_code(Handle handle, std::uint32_t* exit_code) const;
    bool set_file_pointer(Handle handle, std::int32_t distance,
                          std::int32_t* distance_high, std::uint32_t method,
                          std::uint32_t* position_low);
    bool read_file(Handle handle, void* buffer, std::uint32_t bytes_to_read,
                   std::uint32_t* bytes_read);
    bool file_size(Handle handle, std::uint64_t* size) const;
    WaitResult wait(Handle handle, std::uint32_t milliseconds);
    bool release_mutex(Handle handle);
    bool release_semaphore(Handle handle, std::int32_t release_count,
                           std::int32_t* previous_count);
    bool close(Handle handle);
    std::uint32_t active_handle_count() const;

    static void yield();

private:
    struct State;
    std::unique_ptr<State> state_;
};

} // namespace h2::port

#endif
