#include "h2/native_async_worker.hpp"

#include <atomic>
#include <chrono>
#include <mutex>

namespace h2::port {

struct NativeAsyncWorker::Data {
    Data(NativeAsyncPlatform& platform_value,
         NativeAsyncLifecycle& lifecycle_value)
        : platform(platform_value), lifecycle(lifecycle_value) {}

    NativeAsyncPlatform& platform;
    NativeAsyncLifecycle& lifecycle;
    mutable std::mutex mutex;
    Step step = nullptr;
    void* context = nullptr;
    NativeAsyncPlatform::Handle worker = NativeAsyncPlatform::invalid_handle;
    NativeAsyncPlatform::Handle wake = NativeAsyncPlatform::invalid_handle;
    std::atomic<bool> exit_requested = false;
    bool exit_wake_sent = false;
};

namespace {

std::uint32_t remaining_milliseconds(
    std::uint32_t requested,
    const std::chrono::steady_clock::time_point& deadline) {
    if (requested == NativeAsyncLifecycle::infinite)
        return NativeAsyncLifecycle::infinite;
    const auto now = std::chrono::steady_clock::now();
    if (now >= deadline)
        return 0;
    const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
        deadline - now);
    return static_cast<std::uint32_t>(remaining.count());
}

} // namespace

std::uint32_t NativeAsyncWorker::worker_main(void* raw_data) {
    auto& data = *static_cast<Data*>(raw_data);
    for (;;) {
        if (data.platform.wait(data.wake, NativeAsyncPlatform::infinite) !=
            NativeAsyncPlatform::WaitResult::signaled)
            return 1;
        if (data.exit_requested.load(std::memory_order_acquire))
            return 0;
        if (data.step(data.context) == StepResult::completed) {
            if (!data.lifecycle.complete())
                return 2;
        } else if (!data.platform.release_semaphore(data.wake, 1, nullptr)) {
            return 3;
        }
    }
}

NativeAsyncWorker::NativeAsyncWorker(NativeAsyncPlatform& platform,
                                     NativeAsyncLifecycle& lifecycle)
    : data_(std::make_unique<Data>(platform, lifecycle)) {}

NativeAsyncWorker::~NativeAsyncWorker() {
    if (data_->worker != NativeAsyncPlatform::invalid_handle)
        shutdown(NativeAsyncLifecycle::infinite);
}

bool NativeAsyncWorker::start(Step step, void* context, std::int32_t priority,
                              std::uint32_t stack_size) {
    const std::lock_guard lock(data_->mutex);
    if (!step || data_->worker != NativeAsyncPlatform::invalid_handle ||
        data_->wake != NativeAsyncPlatform::invalid_handle ||
        data_->lifecycle.state() != NativeAsyncLifecycle::State::stopped)
        return false;

    data_->step = step;
    data_->context = context;
    data_->exit_requested.store(false, std::memory_order_release);
    data_->exit_wake_sent = false;
    data_->wake = data_->platform.create_semaphore(
        0, static_cast<std::int32_t>(NativeAsyncLifecycle::maximum_admitted));
    if (data_->wake == NativeAsyncPlatform::invalid_handle)
        return false;
    data_->worker = data_->platform.create_thread(stack_size, worker_main, data_.get());
    if (data_->worker != NativeAsyncPlatform::invalid_handle &&
        data_->platform.set_thread_priority(data_->worker, priority) &&
        data_->lifecycle.start())
        return true;

    if (data_->worker != NativeAsyncPlatform::invalid_handle) {
        data_->exit_requested.store(true, std::memory_order_release);
        data_->platform.release_semaphore(data_->wake, 1, nullptr);
        data_->platform.wait(data_->worker, NativeAsyncPlatform::infinite);
        data_->platform.close(data_->worker);
        data_->worker = NativeAsyncPlatform::invalid_handle;
    }
    data_->platform.close(data_->wake);
    data_->wake = NativeAsyncPlatform::invalid_handle;
    data_->step = nullptr;
    data_->context = nullptr;
    return false;
}

bool NativeAsyncWorker::try_admit_and_wake() {
    const std::lock_guard lock(data_->mutex);
    if (data_->worker == NativeAsyncPlatform::invalid_handle ||
        !data_->lifecycle.try_admit())
        return false;
    if (data_->platform.release_semaphore(data_->wake, 1, nullptr))
        return true;
    data_->lifecycle.complete();
    return false;
}

bool NativeAsyncWorker::shutdown(std::uint32_t milliseconds) {
    const std::lock_guard lock(data_->mutex);
    if (data_->worker == NativeAsyncPlatform::invalid_handle)
        return data_->wake == NativeAsyncPlatform::invalid_handle &&
               data_->lifecycle.state() == NativeAsyncLifecycle::State::stopped;

    const auto deadline = std::chrono::steady_clock::now() +
        std::chrono::milliseconds(milliseconds == NativeAsyncLifecycle::infinite
                                      ? 0
                                      : milliseconds);
    if (data_->lifecycle.state() == NativeAsyncLifecycle::State::accepting &&
        !data_->lifecycle.begin_stop())
        return false;
    if (data_->lifecycle.state() != NativeAsyncLifecycle::State::stopping ||
        !data_->lifecycle.wait_drained(
            remaining_milliseconds(milliseconds, deadline)))
        return false;

    if (!data_->exit_wake_sent) {
        data_->exit_requested.store(true, std::memory_order_release);
        if (!data_->platform.release_semaphore(data_->wake, 1, nullptr))
            return false;
        data_->exit_wake_sent = true;
    }
    if (data_->platform.wait(data_->worker,
                             remaining_milliseconds(milliseconds, deadline)) !=
        NativeAsyncPlatform::WaitResult::signaled)
        return false;

    std::uint32_t exit_code = 0;
    const bool clean_exit = data_->platform.thread_exit_code(data_->worker, &exit_code) &&
                            exit_code == 0;
    const bool closed_worker = data_->platform.close(data_->worker);
    const bool closed_wake = data_->platform.close(data_->wake);
    data_->worker = NativeAsyncPlatform::invalid_handle;
    data_->wake = NativeAsyncPlatform::invalid_handle;
    data_->step = nullptr;
    data_->context = nullptr;
    data_->exit_wake_sent = false;
    const bool stopped = data_->lifecycle.finish_stop();
    return clean_exit && closed_worker && closed_wake && stopped;
}

NativeAsyncPlatform::Handle NativeAsyncWorker::worker_handle() const {
    const std::lock_guard lock(data_->mutex);
    return data_->worker;
}

NativeAsyncPlatform::Handle NativeAsyncWorker::wake_handle() const {
    const std::lock_guard lock(data_->mutex);
    return data_->wake;
}

} // namespace h2::port
