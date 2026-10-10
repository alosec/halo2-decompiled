#include "h2/guest_arena.hpp"
#include "h2/native_async_guest_bridge.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <unistd.h>

namespace {

bool check(bool condition, const char* message) {
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

std::uint32_t exit_probe(void*) {
    return 0xabcdef01u;
}

} // namespace

int main() {
    using Bridge = h2::port::NativeAsyncGuestBridge;
    using Platform = h2::port::NativeAsyncPlatform;
    bool ok = true;
    std::string error;
    auto arena = h2::port::GuestArena::create(error);
    if (!arena) {
        std::cerr << error << '\n';
        return 1;
    }

    constexpr std::uint32_t writable_address = 0x10000;
    constexpr std::uint32_t read_only_address = 0x20000;
    const auto page_size = static_cast<std::uint32_t>(arena->page_size());
    ok &= check(arena->protect(writable_address, page_size,
                               h2::port::GuestMemoryProtection::read_write, error),
                "could not create writable guest test page");
    ok &= check(arena->protect(read_only_address, page_size,
                               h2::port::GuestMemoryProtection::read_only, error),
                "could not create read-only guest test page");

    auto* const guest = static_cast<unsigned char*>(arena->pointer(writable_address));
    auto* const guest_bytes_read = reinterpret_cast<std::uint32_t*>(guest + 0x40);
    auto* const guest_high = reinterpret_cast<std::int32_t*>(guest + 0x44);
    auto* const guest_position = reinterpret_cast<std::uint32_t*>(guest + 0x48);
    auto* const guest_previous = reinterpret_cast<std::int32_t*>(guest + 0x4c);
    auto* const guest_exit_code = reinterpret_cast<std::uint32_t*>(guest + 0x50);

    Platform platform;
    Bridge bridge(*arena, platform);
    const Bridge::Handle mutex = bridge.create_mutex(false);
    const Bridge::Handle semaphore = bridge.create_semaphore(0, 2);
    ok &= check(mutex != Platform::invalid_handle &&
                    semaphore != Platform::invalid_handle,
                "bridged synchronization creation failed");
    ok &= check(bridge.wait(mutex, 0) == Bridge::WaitResult::signaled &&
                    bridge.release_mutex(mutex),
                "bridged mutex operation failed");

    std::int32_t host_previous = -1;
    ok &= check(!bridge.release_semaphore(semaphore, 1, &host_previous) &&
                    bridge.wait(semaphore, 0) == Bridge::WaitResult::timeout,
                "invalid host output pointer caused a semaphore side effect");
    *guest_previous = -1;
    ok &= check(bridge.release_semaphore(semaphore, 1, guest_previous) &&
                    *guest_previous == 0 &&
                    bridge.wait(semaphore, 0) == Bridge::WaitResult::signaled,
                "bridged semaphore output failed");

    char file_template[] = "/tmp/h2-native-async-bridge-XXXXXX";
    const int temporary_fd = ::mkstemp(file_template);
    constexpr char payload[] = "0123456789abcdef";
    ok &= check(temporary_fd >= 0, "temporary bridge file creation failed");
    if (temporary_fd >= 0) {
        ok &= check(::write(temporary_fd, payload, sizeof(payload) - 1) ==
                        static_cast<ssize_t>(sizeof(payload) - 1),
                    "temporary bridge file write failed");
        ::close(temporary_fd);
    }
    const std::filesystem::path file_path(file_template);
    const Bridge::Handle file = bridge.open_read_only_host(file_path);
    ok &= check(file != Platform::invalid_handle, "bridged file open failed");

    *guest_high = 0;
    *guest_position = 0;
    ok &= check(bridge.set_file_pointer(file, 4, guest_high, 0, guest_position) &&
                    *guest_high == 0 && *guest_position == 4,
                "bridged guest seek outputs failed");
    std::array<char, 4> host_buffer{};
    ok &= check(!bridge.read_file(file, host_buffer.data(), host_buffer.size(),
                                  guest_bytes_read),
                "bridged read accepted an ordinary host buffer");
    auto* const read_only_buffer = arena->pointer(read_only_address);
    ok &= check(!bridge.read_file(file, read_only_buffer, 4, guest_bytes_read),
                "bridged read accepted a read-only guest buffer");
    auto* const crossing_buffer = guest + page_size - 2;
    ok &= check(!bridge.read_file(file, crossing_buffer, 4, guest_bytes_read),
                "bridged read accepted a range crossing into an unmapped page");
    *guest_bytes_read = 0;
    ok &= check(bridge.read_file(file, guest, 4, guest_bytes_read) &&
                    *guest_bytes_read == 4 &&
                    std::string_view(reinterpret_cast<char*>(guest), 4) == "4567",
                "bridged file read failed or invalid reads moved the file offset");

    const Bridge::Handle thread = platform.create_thread(0x4000, exit_probe, nullptr);
    ok &= check(thread != Platform::invalid_handle &&
                    bridge.set_thread_priority(thread, 1) &&
                    bridge.wait(thread, Platform::infinite) == Bridge::WaitResult::signaled &&
                    bridge.thread_exit_code(thread, guest_exit_code) &&
                    *guest_exit_code == 0xabcdef01u &&
                    !bridge.thread_exit_code(thread,
                        reinterpret_cast<std::uint32_t*>(&host_previous)),
                "bridged worker state validation failed");

    ok &= check(bridge.close(thread) && bridge.close(file) &&
                    bridge.close(semaphore) && bridge.close(mutex) &&
                    platform.active_handle_count() == 0,
                "bridged handle teardown failed");
    std::error_code filesystem_error;
    std::filesystem::remove(file_path, filesystem_error);
    return ok ? 0 : 1;
}
