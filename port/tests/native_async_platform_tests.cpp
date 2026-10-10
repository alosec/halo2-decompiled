#include "h2/native_async_platform.hpp"

#include <atomic>
#include <iostream>
#include <thread>

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
