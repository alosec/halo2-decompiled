#ifndef H2_PORT_NATIVE_ASYNC_WORKER_HPP
#define H2_PORT_NATIVE_ASYNC_WORKER_HPP

#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"

#include <cstdint>
#include <memory>

namespace h2::port {

class NativeAsyncWorker {
public:
    enum class StepResult {
        retry,
        completed,
    };
    using Step = StepResult (*)(void* context);

    NativeAsyncWorker(NativeAsyncPlatform& platform,
                      NativeAsyncLifecycle& lifecycle);
    ~NativeAsyncWorker();
    NativeAsyncWorker(const NativeAsyncWorker&) = delete;
    NativeAsyncWorker& operator=(const NativeAsyncWorker&) = delete;

    bool start(Step step, void* context, std::int32_t priority = 1,
               std::uint32_t stack_size = 0x4000);
    bool try_admit();
    bool publish_admitted_wake();
    bool abort_admitted();
    bool try_admit_and_wake();
    bool shutdown(std::uint32_t milliseconds);

    NativeAsyncPlatform::Handle worker_handle() const;
    NativeAsyncPlatform::Handle wake_handle() const;

private:
    struct Data;
    static std::uint32_t worker_main(void* raw_data);
    std::unique_ptr<Data> data_;
};

} // namespace h2::port

#endif
