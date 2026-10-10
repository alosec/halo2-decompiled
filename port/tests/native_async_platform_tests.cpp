#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"

#include <array>
#include <atomic>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <thread>
#include <unistd.h>

namespace {

bool check(bool condition, const char* message) {
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

struct ThreadProbeContext {
    h2::port::NativeAsyncPlatform* platform;
    h2::port::NativeAsyncPlatform::Handle semaphore;
    std::atomic<bool>* ran;
};

std::uint32_t thread_probe(void* raw_context) {
    auto* context = static_cast<ThreadProbeContext*>(raw_context);
    if (context->platform->wait(context->semaphore,
                                h2::port::NativeAsyncPlatform::infinite) !=
        h2::port::NativeAsyncPlatform::WaitResult::signaled)
        return 1;
    *context->ran = true;
    return 0x12345678u;
}

} // namespace

int main() {
    using Lifecycle = h2::port::NativeAsyncLifecycle;
    using Platform = h2::port::NativeAsyncPlatform;
    bool ok = true;

    Lifecycle lifecycle;
    ok &= check(lifecycle.state() == Lifecycle::State::stopped &&
                    lifecycle.admitted_count() == 0 &&
                    !lifecycle.try_admit() && !lifecycle.complete() &&
                    !lifecycle.wait_drained(0) && !lifecycle.finish_stop(),
                "stopped lifecycle accepted an operation");
    ok &= check(lifecycle.start() && !lifecycle.start() &&
                    lifecycle.state() == Lifecycle::State::accepting,
                "lifecycle start transition failed");
    ok &= check(lifecycle.try_admit() && lifecycle.try_admit() &&
                    lifecycle.admitted_count() == 2,
                "lifecycle admission accounting failed");
    ok &= check(lifecycle.begin_stop() && !lifecycle.begin_stop() &&
                    lifecycle.state() == Lifecycle::State::stopping &&
                    !lifecycle.try_admit() && lifecycle.admitted_count() == 2,
                "lifecycle stop admission was not linearized");
    std::atomic<bool> lifecycle_drained = false;
    std::thread lifecycle_waiter([&] {
        lifecycle_drained = lifecycle.wait_drained(Lifecycle::infinite);
    });
    ok &= check(!lifecycle.wait_drained(0) && lifecycle.complete() &&
                    !lifecycle.finish_stop() && lifecycle.complete(),
                "lifecycle drain tracking failed");
    lifecycle_waiter.join();
    ok &= check(lifecycle_drained.load() && lifecycle.wait_drained(0),
                "lifecycle drain waiter was not awakened");
    ok &= check(lifecycle.finish_stop() &&
                    lifecycle.state() == Lifecycle::State::stopped &&
                    lifecycle.admitted_count() == 0 && !lifecycle.complete(),
                "lifecycle finish transition failed");
    ok &= check(lifecycle.start() && lifecycle.begin_stop() &&
                    lifecycle.wait_drained(Lifecycle::infinite) &&
                    lifecycle.finish_stop(),
                "empty lifecycle restart/drain failed");

    Lifecycle capacity_lifecycle;
    ok &= check(capacity_lifecycle.start(), "capacity lifecycle start failed");
    for (std::uint32_t index = 0; index < Lifecycle::maximum_admitted; ++index)
        ok &= check(capacity_lifecycle.try_admit(),
                    "lifecycle rejected an in-capacity admission");
    ok &= check(!capacity_lifecycle.try_admit() &&
                    capacity_lifecycle.admitted_count() == Lifecycle::maximum_admitted &&
                    capacity_lifecycle.begin_stop(),
                "lifecycle exceeded the recovered queue capacity");
    for (std::uint32_t index = 0; index < Lifecycle::maximum_admitted; ++index)
        ok &= check(capacity_lifecycle.complete(),
                    "lifecycle capacity drain underflowed");
    ok &= check(capacity_lifecycle.wait_drained(0) &&
                    capacity_lifecycle.finish_stop(),
                "capacity lifecycle did not finish stopping");

    Platform platform;

    ok &= check(platform.create_semaphore(-1, 1) == Platform::invalid_handle,
                "negative semaphore count accepted");
    ok &= check(platform.create_semaphore(2, 1) == Platform::invalid_handle,
                "oversized semaphore count accepted");

    const Platform::Handle mutex = platform.create_mutex(false);
    ok &= check(mutex != Platform::invalid_handle, "mutex creation failed");
    ok &= check(platform.wait(mutex, 0) == Platform::WaitResult::signaled,
                "mutex acquisition failed");
    ok &= check(platform.wait(mutex, 0) == Platform::WaitResult::signaled,
                "recursive mutex acquisition failed");
    ok &= check(platform.release_mutex(mutex) && platform.release_mutex(mutex),
                "recursive mutex release failed");

    const Platform::Handle semaphore = platform.create_semaphore(0, 2);
    ok &= check(semaphore != Platform::invalid_handle, "semaphore creation failed");
    ok &= check(platform.wait(semaphore, 0) == Platform::WaitResult::timeout,
                "empty semaphore did not time out");
    std::int32_t previous = -1;
    ok &= check(platform.release_semaphore(semaphore, 2, &previous) && previous == 0,
                "semaphore release/count reporting failed");
    ok &= check(!platform.release_semaphore(semaphore, 1, nullptr),
                "semaphore maximum was not enforced");
    ok &= check(platform.wait(semaphore, 0) == Platform::WaitResult::signaled &&
                platform.wait(semaphore, 0) == Platform::WaitResult::signaled &&
                platform.wait(semaphore, 0) == Platform::WaitResult::timeout,
                "semaphore count consumption failed");

    std::atomic<bool> awakened = false;
    std::thread waiter([&] {
        awakened = platform.wait(semaphore, Platform::infinite) ==
                   Platform::WaitResult::signaled;
    });
    ok &= check(platform.release_semaphore(semaphore, 1, nullptr),
                "semaphore wake release failed");
    waiter.join();
    ok &= check(awakened.load(), "semaphore waiter was not awakened");

    const Platform::Handle thread_semaphore = platform.create_semaphore(0, 1);
    std::atomic<bool> thread_ran = false;
    ThreadProbeContext thread_context{&platform, thread_semaphore, &thread_ran};
    const Platform::Handle thread = platform.create_thread(0x4000, thread_probe,
                                                            &thread_context);
    ok &= check(thread != Platform::invalid_handle &&
                    platform.create_thread(0, nullptr, nullptr) == Platform::invalid_handle,
                "worker thread creation validation failed");
    ok &= check(platform.wait(thread, 0) == Platform::WaitResult::timeout,
                "running worker thread reported completion");
    ok &= check(platform.set_thread_priority(thread, 1) &&
                    !platform.set_thread_priority(thread, 16) &&
                    !platform.set_thread_priority(semaphore, 1),
                "worker thread priority mapping failed");
    ok &= check(platform.release_semaphore(thread_semaphore, 1, nullptr) &&
                    platform.wait(thread, Platform::infinite) == Platform::WaitResult::signaled,
                "worker thread completion wait failed");
    std::uint32_t exit_code = 0;
    ok &= check(thread_ran.load() && platform.thread_exit_code(thread, &exit_code) &&
                    exit_code == 0x12345678u,
                "worker thread exit state was not preserved");
    ok &= check(platform.close(thread) && platform.close(thread_semaphore) &&
                    platform.wait(thread, 0) == Platform::WaitResult::failed,
                "worker thread join/teardown failed");

    char file_template[] = "/tmp/h2-native-async-XXXXXX";
    const int temporary_fd = ::mkstemp(file_template);
    constexpr char payload[] = "0123456789abcdef";
    ok &= check(temporary_fd >= 0, "temporary file creation failed");
    if (temporary_fd >= 0) {
        ok &= check(::write(temporary_fd, payload, sizeof(payload) - 1) ==
                        static_cast<ssize_t>(sizeof(payload) - 1),
                    "temporary file write failed");
        ::close(temporary_fd);
    }
    const std::filesystem::path file_path(file_template);
    const std::filesystem::path symlink_path = file_path.string() + ".link";
    std::error_code filesystem_error;
    std::filesystem::create_symlink(file_path, symlink_path, filesystem_error);
    const Platform::Handle file = platform.open_read_only(file_path);
    ok &= check(file != Platform::invalid_handle, "read-only file open failed");
    ok &= check(platform.open_read_only(symlink_path) == Platform::invalid_handle,
                "symbolic-link file open was accepted");
    std::int32_t high = 0;
    std::uint32_t position = 0;
    std::array<char, 5> bytes{};
    std::uint32_t bytes_read = 0;
    ok &= check(platform.set_file_pointer(file, 4, &high, 0, &position) &&
                    position == 4 && high == 0,
                "absolute file seek failed");
    ok &= check(platform.read_file(file, bytes.data(), 4, &bytes_read) &&
                    bytes_read == 4 && std::string_view(bytes.data(), 4) == "4567",
                "positioned file read failed");
    ok &= check(platform.set_file_pointer(file, 2, nullptr, 1, &position) &&
                    position == 10 &&
                    platform.read_file(file, bytes.data(), 2, &bytes_read) &&
                    bytes_read == 2 && std::string_view(bytes.data(), 2) == "ab",
                "current-relative file seek failed");
    high = 1;
    ok &= check(platform.set_file_pointer(file, 0, &high, 0, &position) &&
                    position == 0 && high == 1,
                "64-bit file seek failed");
    ok &= check(platform.set_file_pointer(file, -2, nullptr, 2, &position) &&
                    position == sizeof(payload) - 3,
                "end-relative file seek failed");
    ok &= check(platform.read_file(file, bytes.data(), 4, &bytes_read) &&
                    bytes_read == 2 && std::string_view(bytes.data(), 2) == "ef",
                "short file read failed");
    ok &= check(!platform.set_file_pointer(file, 0, nullptr, 3, &position),
                "invalid file seek method accepted");
    ok &= check(platform.close(file) &&
                    !platform.read_file(file, bytes.data(), 1, &bytes_read),
                "file handle teardown failed");
    std::filesystem::remove(symlink_path, filesystem_error);
    std::filesystem::remove(file_path, filesystem_error);

    ok &= check(platform.active_handle_count() == 2, "active handle count mismatch");
    ok &= check(platform.close(semaphore), "semaphore close failed");
    ok &= check(platform.wait(semaphore, 0) == Platform::WaitResult::failed,
                "closed semaphore token remained valid");
    const Platform::Handle replacement = platform.create_semaphore(1, 1);
    ok &= check(replacement != semaphore &&
                platform.wait(semaphore, 0) == Platform::WaitResult::failed &&
                platform.wait(replacement, 0) == Platform::WaitResult::signaled,
                "stale handle generation was reused");
    ok &= check(!platform.release_mutex(replacement),
                "semaphore token was accepted as a mutex");
    ok &= check(platform.close(replacement) && platform.close(mutex),
                "handle teardown failed");
    ok &= check(platform.active_handle_count() == 0, "handle table did not drain");
    ok &= check(!platform.close(mutex), "stale mutex close succeeded");

    Platform::yield();
    return ok ? 0 : 1;
}
