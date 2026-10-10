#include "h2/native_guest_worker.hpp"

#include "h2/guest_arena.hpp"

#include <cstdint>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

namespace h2::port {
namespace {

bool prepare_guest_call(GuestArena& arena, std::uint32_t guest_entry_address,
                        std::uint32_t stack_address, std::uint32_t stack_size,
                        int (*&entry)(), void*& stack_top, std::string& error) {
    const std::uint64_t stack_end =
        static_cast<std::uint64_t>(stack_address) + stack_size;
    const std::uint32_t page_size = static_cast<std::uint32_t>(arena.page_size());
    if (!page_size || stack_size < 0x4000 || stack_address % page_size != 0 ||
        stack_size % page_size != 0 || stack_end > kGuestAddressSpaceSize) {
        error = "guest worker stack range is invalid or not page aligned";
        return false;
    }
    void* const entry_pointer = arena.pointer(guest_entry_address);
    if (!arena.host_range_executable(entry_pointer, 4)) {
        error = "guest worker entry is not mapped executable guest code";
        return false;
    }
    if (guest_entry_address >= stack_address && guest_entry_address < stack_end) {
        error = "guest worker stack overlaps its entry point";
        return false;
    }
    if (!arena.protect(stack_address, stack_size,
                       GuestMemoryProtection::read_write, error))
        return false;
    entry = reinterpret_cast<int (*)()>(entry_pointer);
    stack_top = arena.pointer(static_cast<std::uint32_t>(stack_end) - 16);
    return true;
}

} // namespace

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
    if (!prepare_guest_call(data_->arena, guest_entry_address, stack_address,
                            stack_size, data_->entry, data_->stack_top, error))
        return false;
    if (!data_->worker.start(run_guest_step, data_.get(), priority, 0x4000)) {
        data_->entry = nullptr;
        data_->stack_top = nullptr;
        error = "native guest worker could not start";
        return false;
    }
    return true;
}

bool NativeGuestWorker::invoke_stopped(std::uint32_t guest_entry_address,
                                       int& result, std::string& error,
                                       std::uint32_t stack_address,
                                       std::uint32_t stack_size) {
    error.clear();
    if (data_->worker.worker_handle() != NativeAsyncPlatform::invalid_handle) {
        error = "native guest worker is running";
        return false;
    }
    int (*entry)() = nullptr;
    void* stack_top = nullptr;
    if (!prepare_guest_call(data_->arena, guest_entry_address, stack_address,
                            stack_size, entry, stack_top, error))
        return false;
    result = h2_call_guest_int_0(entry, stack_top);
    return true;
}

bool NativeGuestWorker::try_admit() {
    return data_->worker.try_admit();
}

bool NativeGuestWorker::publish_admitted_wake() {
    return data_->worker.publish_admitted_wake();
}

bool NativeGuestWorker::abort_admitted() {
    return data_->worker.abort_admitted();
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
