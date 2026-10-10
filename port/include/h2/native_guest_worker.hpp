#ifndef H2_PORT_NATIVE_GUEST_WORKER_HPP
#define H2_PORT_NATIVE_GUEST_WORKER_HPP

#include "h2/native_async_worker.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace h2::port {

class GuestArena;

class NativeGuestWorker {
public:
    static constexpr std::uint32_t default_stack_address = 0x20100000u;
    static constexpr std::uint32_t default_stack_size = 0x00010000u;

    NativeGuestWorker(GuestArena& arena, NativeAsyncPlatform& platform,
                      NativeAsyncLifecycle& lifecycle);
    ~NativeGuestWorker();
    NativeGuestWorker(const NativeGuestWorker&) = delete;
    NativeGuestWorker& operator=(const NativeGuestWorker&) = delete;

    bool start(std::uint32_t guest_entry_address, std::string& error,
               std::uint32_t stack_address = default_stack_address,
               std::uint32_t stack_size = default_stack_size,
               std::int32_t priority = 1);
    bool try_admit_and_wake();
    bool shutdown(std::uint32_t milliseconds);

    NativeAsyncPlatform::Handle worker_handle() const;
    NativeAsyncPlatform::Handle wake_handle() const;

private:
    struct Data;
    static NativeAsyncWorker::StepResult run_guest_step(void* context);
    std::unique_ptr<Data> data_;
};

} // namespace h2::port

#endif
