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

} // namespace

int main() {
    using Platform = h2::port::NativeAsyncPlatform;
    bool ok = true;
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
