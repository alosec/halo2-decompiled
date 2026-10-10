#include "h2/native_guest_worker.hpp"

#include "h2/guest_arena.hpp"

#include <cstdint>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

namespace h2::port {

struct NativeGuestWorker::Data {
    Data(GuestArena& arena_value, NativeAsyncPlatform& platform,
         NativeAsyncLifecycle& lifecycle)
        : arena(arena_value), worker(platform, lifecycle) {}

    GuestArena& arena;
    NativeAsyncWorker worker;
    int (*entry)() = nullptr;
    void* stack_top = nullptr;
};

NativeGuestWorker::NativeGuestWorker(GuestArena& arena,
                                     NativeAsyncPlatform& platform,
                                     NativeAsyncLifecycle& lifecycle)
    : data_(std::make_unique<Data>(arena, platform, lifecycle)) {}

NativeGuestWorker::~NativeGuestWorker() = default;

bool NativeGuestWorker::start(std::uint32_t guest_entry_address,
                              std::string& error,
                              std::uint32_t stack_address,
                              std::uint32_t stack_size,
                              std::int32_t priority) {
    error.clear();
    if (data_->worker.worker_handle() != NativeAsyncPlatform::invalid_handle) {
        error = "native guest worker is already running";
        return false;
    }
    const std::uint64_t stack_end =
        static_cast<std::uint64_t>(stack_address) + stack_size;
    const std::uint32_t page_size =
        static_cast<std::uint32_t>(data_->arena.page_size());
    if (!page_size || stack_size < 0x4000 || stack_address % page_size != 0 ||
        stack_size % page_size != 0 || stack_end > kGuestAddressSpaceSize) {
        error = "guest worker stack range is invalid or not page aligned";
        return false;
    }
    void* const entry = data_->arena.pointer(guest_entry_address);
    if (!data_->arena.host_range_executable(entry, 4)) {
        error = "guest worker entry is not mapped executable guest code";
        return false;
    }
    if (guest_entry_address >= stack_address && guest_entry_address < stack_end) {
        error = "guest worker stack overlaps its entry point";
        return false;
    }
    if (!data_->arena.protect(stack_address, stack_size,
                              GuestMemoryProtection::read_write, error))
        return false;

    data_->entry = reinterpret_cast<int (*)()>(entry);
    data_->stack_top = data_->arena.pointer(
        static_cast<std::uint32_t>(stack_end) - 16);
    if (!data_->worker.start(run_guest_step, data_.get(), priority, 0x4000)) {
        data_->entry = nullptr;
        data_->stack_top = nullptr;
        error = "native guest worker could not start";
        return false;
    }
    return true;
}

bool NativeGuestWorker::try_admit_and_wake() {
    return data_->worker.try_admit_and_wake();
}

bool NativeGuestWorker::shutdown(std::uint32_t milliseconds) {
    const bool stopped = data_->worker.shutdown(milliseconds);
    if (stopped) {
        data_->entry = nullptr;
        data_->stack_top = nullptr;
    }
    return stopped;
}

NativeAsyncPlatform::Handle NativeGuestWorker::worker_handle() const {
    return data_->worker.worker_handle();
}

NativeAsyncPlatform::Handle NativeGuestWorker::wake_handle() const {
    return data_->worker.wake_handle();
}

NativeAsyncWorker::StepResult NativeGuestWorker::run_guest_step(void* context) {
    auto& data = *static_cast<Data*>(context);
    return h2_call_guest_int_0(data.entry, data.stack_top) == 0
               ? NativeAsyncWorker::StepResult::retry
               : NativeAsyncWorker::StepResult::completed;
}

} // namespace h2::port
