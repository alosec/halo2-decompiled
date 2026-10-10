#ifndef H2_PORT_NATIVE_ASYNC_PLATFORM_HPP
#define H2_PORT_NATIVE_ASYNC_PLATFORM_HPP

#include <cstdint>
#include <memory>

namespace h2::port {

class NativeAsyncPlatform {
public:
    using Handle = std::uint32_t;
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
