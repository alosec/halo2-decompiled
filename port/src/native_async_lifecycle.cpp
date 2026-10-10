#include "h2/native_async_lifecycle.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace h2::port {

struct NativeAsyncLifecycle::Data {
    mutable std::mutex mutex;
    std::condition_variable drained;
    State state = State::stopped;
    std::uint32_t admitted = 0;
};

NativeAsyncLifecycle::NativeAsyncLifecycle() : data_(std::make_unique<Data>()) {}
NativeAsyncLifecycle::~NativeAsyncLifecycle() = default;

bool NativeAsyncLifecycle::start() {
    const std::lock_guard lock(data_->mutex);
    if (data_->state != State::stopped || data_->admitted != 0)
        return false;
    data_->state = State::accepting;
    return true;
}

bool NativeAsyncLifecycle::try_admit() {
    const std::lock_guard lock(data_->mutex);
    if (data_->state != State::accepting ||
        data_->admitted == maximum_admitted)
        return false;
    ++data_->admitted;
    return true;
}

bool NativeAsyncLifecycle::complete() {
    const std::lock_guard lock(data_->mutex);
    if (data_->admitted == 0)
        return false;
    --data_->admitted;
    if (data_->admitted == 0)
        data_->drained.notify_all();
    return true;
}

bool NativeAsyncLifecycle::begin_stop() {
    const std::lock_guard lock(data_->mutex);
    if (data_->state != State::accepting)
        return false;
    data_->state = State::stopping;
    if (data_->admitted == 0)
        data_->drained.notify_all();
    return true;
}

bool NativeAsyncLifecycle::wait_drained(std::uint32_t milliseconds) {
    std::unique_lock lock(data_->mutex);
    if (data_->state != State::stopping)
        return false;
    const auto drained = [this] { return data_->admitted == 0; };
    if (milliseconds == infinite) {
        data_->drained.wait(lock, drained);
        return true;
    }
    return data_->drained.wait_for(lock, std::chrono::milliseconds(milliseconds),
                                   drained);
}

bool NativeAsyncLifecycle::finish_stop() {
    const std::lock_guard lock(data_->mutex);
    if (data_->state != State::stopping || data_->admitted != 0)
        return false;
    data_->state = State::stopped;
    return true;
}

NativeAsyncLifecycle::State NativeAsyncLifecycle::state() const {
    const std::lock_guard lock(data_->mutex);
    return data_->state;
}

std::uint32_t NativeAsyncLifecycle::admitted_count() const {
    const std::lock_guard lock(data_->mutex);
    return data_->admitted;
}

} // namespace h2::port
