#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"
#include "h2/native_async_worker.hpp"

#include <atomic>
#include <iostream>

namespace {

bool check(bool condition, const char* message) {
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

struct StepContext {
    std::atomic<bool> may_complete = false;
    std::atomic<std::uint32_t> calls = 0;
};

h2::port::NativeAsyncWorker::StepResult run_step(void* raw_context) {
    auto& context = *static_cast<StepContext*>(raw_context);
    ++context.calls;
    return context.may_complete.load()
               ? h2::port::NativeAsyncWorker::StepResult::completed
               : h2::port::NativeAsyncWorker::StepResult::retry;
}

} // namespace

int main() {
    using Lifecycle = h2::port::NativeAsyncLifecycle;
    using Platform = h2::port::NativeAsyncPlatform;
    using Worker = h2::port::NativeAsyncWorker;

    bool ok = true;
    Platform platform;
    Lifecycle lifecycle;
    Worker worker(platform, lifecycle);
    StepContext context;

    ok &= check(!worker.start(nullptr, nullptr),
                "worker accepted a null step callback");
    ok &= check(worker.start(run_step, &context) &&
                    lifecycle.state() == Lifecycle::State::accepting &&
                    worker.worker_handle() != Platform::invalid_handle &&
                    worker.wake_handle() != Platform::invalid_handle &&
                    platform.active_handle_count() == 2,
                "worker start did not own the recovered handle pair");
    ok &= check(!worker.start(run_step, &context),
                "running worker started twice");
    ok &= check(worker.try_admit_and_wake(),
                "worker rejected an accepting admission");

    while (context.calls.load() == 0)
        Platform::yield();
    ok &= check(!worker.shutdown(1) &&
                    lifecycle.state() == Lifecycle::State::stopping &&
                    lifecycle.admitted_count() == 1 &&
                    worker.worker_handle() != Platform::invalid_handle &&
                    worker.wake_handle() != Platform::invalid_handle &&
                    platform.active_handle_count() == 2 &&
                    !worker.try_admit_and_wake(),
                "timed-out drain did not preserve stopping ownership");

    context.may_complete = true;
    ok &= check(worker.shutdown(Lifecycle::infinite) &&
                    lifecycle.state() == Lifecycle::State::stopped &&
                    lifecycle.admitted_count() == 0 &&
                    worker.worker_handle() == Platform::invalid_handle &&
                    worker.wake_handle() == Platform::invalid_handle &&
                    platform.active_handle_count() == 0,
                "drained worker did not join, close, and stop deterministically");

    context.calls = 0;
    context.may_complete = true;
    ok &= check(worker.start(run_step, &context) &&
                    worker.try_admit_and_wake() &&
                    worker.shutdown(Lifecycle::infinite) &&
                    context.calls.load() == 1 &&
                    platform.active_handle_count() == 0,
                "worker did not restart and drain exactly one admission");
    ok &= check(worker.shutdown(0), "stopped worker shutdown was not idempotent");

    return ok ? 0 : 1;
}
