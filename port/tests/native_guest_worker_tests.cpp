#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"
#include "h2/native_guest_worker.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

namespace {

bool check(bool condition, const std::string& message) {
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

void write_u32(std::vector<std::uint8_t>& bytes, std::size_t offset,
               std::uint32_t value) {
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    bytes[offset + 2] = static_cast<std::uint8_t>(value >> 16);
    bytes[offset + 3] = static_cast<std::uint8_t>(value >> 24);
}

bool write_cache(const std::filesystem::path& path, std::int16_t type,
                 const std::string& name) {
    std::vector<std::uint8_t> bytes(0x1000, 0);
    write_u32(bytes, 0, 0x68656164u);
    write_u32(bytes, 4, 8u);
    write_u32(bytes, 8, static_cast<std::uint32_t>(bytes.size()));
    write_u32(bytes, 0x140, static_cast<std::uint16_t>(type));
    write_u32(bytes, 0x7fc, 0x666f6f74u);
    constexpr char build[] = "02.09.27.09809";
    std::copy(std::begin(build), std::end(build), bytes.begin() + 0x120);
    std::copy(name.begin(), name.end(), bytes.begin() + 0x198);
    std::ofstream output(path, std::ios::binary);
    output.write(reinterpret_cast<const char*>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    return output.good();
}

} // namespace

int main(int argc, char** argv) {
    using Lifecycle = h2::port::NativeAsyncLifecycle;
    using Platform = h2::port::NativeAsyncPlatform;
    using Worker = h2::port::NativeGuestWorker;

    if (argc != 2) {
        std::cerr << "usage: native_guest_worker_tests <guest-elf>\n";
        return 2;
    }
    bool ok = true;
    std::string error;
    auto arena = h2::port::GuestArena::create(error);
    ok &= check(arena != nullptr, error);
    if (!arena)
        return 1;
    auto image = h2::port::GuestElfImage::load(argv[1], *arena, error);
    ok &= check(image.has_value(), error);
    if (!image)
        return 1;
    const auto entry = image->symbol_address("h2_worker_step_probe");
    const auto counter = image->symbol_address("h2_worker_step_calls");
    const auto lifecycle_entry = image->symbol_address("h2_queue_lifecycle_probe");
    const auto lifecycle_counter = image->symbol_address("h2_queue_lifecycle_calls");
    const auto queue_initialize =
        image->symbol_address("h2_native_async_queue_initialize");
    const auto queue_teardown =
        image->symbol_address("h2_native_async_queue_teardown");
    const auto queue_free_count = image->symbol_address("h2_async_queue_free_count");
    const auto queue_remove_free_head =
        image->symbol_address("h2_async_queue_remove_free_head");
    const auto queue_restore_free_head =
        image->symbol_address("h2_async_queue_restore_free_head");
    const auto queue_is_cleared = image->symbol_address("h2_async_queue_is_cleared");
    ok &= check(entry.has_value() && counter.has_value() &&
                    lifecycle_entry.has_value() && lifecycle_counter.has_value() &&
                    queue_initialize.has_value() && queue_teardown.has_value() &&
                    queue_free_count.has_value() &&
                    queue_remove_free_head.has_value() &&
                    queue_restore_free_head.has_value() &&
                    queue_is_cleared.has_value(),
                "guest worker fixture symbols are missing");
    if (!entry || !counter || !lifecycle_entry || !lifecycle_counter ||
        !queue_initialize || !queue_teardown || !queue_free_count ||
        !queue_remove_free_head || !queue_restore_free_head || !queue_is_cleared)
        return 1;

    Platform platform;
    Lifecycle lifecycle;
    Worker worker(*arena, platform, lifecycle);
    int invoke_result = 0;
    ok &= check(worker.invoke_stopped(*lifecycle_entry, invoke_result, error) &&
                    invoke_result == 1,
                "stopped guest invocation did not use the validated low stack");
    ok &= check(!worker.start(*counter, error) &&
                    error.find("not mapped executable") != std::string::npos,
                "guest worker accepted a writable data entry");
    ok &= check(worker.start(*entry, error), error);
    ok &= check(!worker.start(*entry, error),
                "running guest worker started twice");
    ok &= check(worker.try_admit_and_wake(),
                "guest worker rejected an admission");
    ok &= check(worker.shutdown(Lifecycle::infinite),
                "guest worker did not drain and join");
    std::uint32_t calls = 0;
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 2 && lifecycle.state() == Lifecycle::State::stopped &&
                    lifecycle.admitted_count() == 0 &&
                    platform.active_handle_count() == 0,
                "guest worker did not retry then complete on its low stack");

    ok &= check(worker.start(*entry, error) && worker.try_admit_and_wake() &&
                    worker.shutdown(Lifecycle::infinite),
                "guest worker did not restart cleanly");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 3 && platform.active_handle_count() == 0,
                "restarted guest worker did not execute exactly once");

    constexpr std::uint32_t cache_slots_address = 0x00557c90u;
    constexpr std::uint32_t cache_active_address = 0x0055aca8u;
    constexpr std::uint32_t cache_slots_size = 6 * 0x804 + 4;
    ok &= check(arena->protect(cache_slots_address, cache_slots_size,
                               h2::port::GuestMemoryProtection::read_write,
                               error),
                "guest cache slot storage could not be mapped: " + error);
    const auto cache_directory = std::filesystem::temp_directory_path() /
        ("h2-async-host-cache-" + std::to_string(getpid()));
    std::error_code filesystem_error;
    std::filesystem::create_directory(cache_directory, filesystem_error);
    const auto mainmenu_path = cache_directory / "mainmenu.map";
    const auto shared_path = cache_directory / "shared.map";
    ok &= check(!filesystem_error && write_cache(mainmenu_path, 2, "mainmenu") &&
                    write_cache(shared_path, 3, "shared"),
                "synthetic async cache files could not be created");

    ok &= check(h2::port::configure_async_services(*arena, error) &&
                    h2::port::patch_async_host_imports(*image, *arena, error),
                error);
    ok &= check(h2::port::install_async_cache_file(
                    mainmenu_path, 2, "mainmenu", true, error) &&
                    h2::port::install_async_cache_file(
                        shared_path, 3, "shared", false, error),
                "host async cache slots did not install and publish: " + error);
    std::uint32_t published_mainmenu = 0;
    std::uint32_t published_shared = 0;
    std::int32_t published_active = -1;
    std::memcpy(&published_shared, arena->pointer(cache_slots_address),
                sizeof(published_shared));
    std::memcpy(&published_mainmenu,
                arena->pointer(cache_slots_address + 2 * 0x804),
                sizeof(published_mainmenu));
    std::memcpy(&published_active, arena->pointer(cache_active_address),
                sizeof(published_active));
    ok &= check(published_mainmenu != Platform::invalid_handle &&
                    published_shared != Platform::invalid_handle &&
                    published_active == 2,
                "host async cache slots published the wrong recovered records");
    ok &= check(!h2::port::start_async_worker(*entry, error) &&
                    error.find("not initialized") != std::string::npos,
                "host worker started before guest queue initialization");
    ok &= check(h2::port::initialize_async_queue(*queue_initialize, error), error);
    int free_count = 0;
    ok &= check(worker.invoke_stopped(*queue_free_count, free_count, error) &&
                    free_count == 150,
                "recovered guest queue did not initialize all 150 free nodes");
    ok &= check(h2::port::start_async_worker(*entry, error), error);
    ok &= check(!h2::port::install_async_cache_file(
                    mainmenu_path, 2, "mainmenu", true, error) &&
                    error.find("worker is still running") != std::string::npos,
                "cache replacement was accepted while the worker was live: " + error);
    ok &= check(host_h2_async_try_admit() &&
                    host_h2_async_publish_admitted() &&
                    h2::port::shutdown_async_worker(Lifecycle::infinite, error),
                "host ABI admission transaction did not execute the guest step");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 4 &&
                    h2::port::start_async_worker(*entry, error) &&
                    host_h2_async_try_admit() && host_h2_async_abort_admitted() &&
                    h2::port::shutdown_async_worker(Lifecycle::infinite, error),
                "host ABI admission abort did not drain without execution");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 4 && !h2::port::reset_async_services(error) &&
                    error.find("queue is still initialized") != std::string::npos &&
                    !h2::port::clear_async_cache_files(error) &&
                    error.find("queue must be torn down") != std::string::npos,
                "host async teardown ordering was not enforced: " + error);
    int mutation_result = 0;
    ok &= check(worker.invoke_stopped(*queue_remove_free_head, mutation_result,
                                      error) && mutation_result == 1 &&
                    !h2::port::teardown_async_queue(*queue_teardown, error) &&
                    error.find("queue teardown failed") != std::string::npos,
                "guest queue teardown accepted an incomplete free list: " + error);
    ok &= check(worker.invoke_stopped(*queue_free_count, free_count, error) &&
                    free_count == 149 &&
                    worker.invoke_stopped(*queue_restore_free_head,
                                          mutation_result, error) &&
                    mutation_result == 1 &&
                    h2::port::teardown_async_queue(*queue_teardown, error),
                "guest queue teardown retry did not restore ownership: " + error);
    int queue_cleared = 0;
    ok &= check(worker.invoke_stopped(*queue_is_cleared, queue_cleared, error) &&
                    queue_cleared == 1 &&
                    !h2::port::reset_async_services(error) &&
                    error.find("live handles") != std::string::npos &&
                    h2::port::clear_async_cache_files(error) &&
                    h2::port::reset_async_services(error) &&
                    !host_h2_async_try_admit(),
                "recovered queue/cache teardown retained guest or host ownership: " +
                    error);
    std::uint32_t cleared_handle = 1;
    std::int32_t cleared_active = 0;
    std::memcpy(&cleared_handle, arena->pointer(cache_slots_address),
                sizeof(cleared_handle));
    std::memcpy(&cleared_active, arena->pointer(cache_active_address),
                sizeof(cleared_active));
    ok &= check(cleared_handle == Platform::invalid_handle &&
                    cleared_active == -1,
                "host cache clear did not publish invalid guest ownership");

    ok &= check(h2::port::configure_async_services(*arena, error) &&
                    h2::port::install_async_cache_file(
                        mainmenu_path, 2, "mainmenu", true, error) &&
                    h2::port::initialize_async_queue(*queue_initialize, error) &&
                    h2::port::start_async_worker(*entry, error) &&
                    host_h2_async_try_admit() &&
                    host_h2_async_publish_admitted() &&
                    h2::port::shutdown_async_worker(Lifecycle::infinite, error) &&
                    h2::port::teardown_async_queue(*queue_teardown, error) &&
                    h2::port::clear_async_cache_files(error) &&
                    h2::port::reset_async_services(error),
                "host async/cache ownership did not restart cleanly: " + error);
    std::filesystem::remove_all(cache_directory, filesystem_error);
    std::uint32_t lifecycle_calls = 0;
    std::memcpy(&lifecycle_calls, arena->pointer(*lifecycle_counter),
                sizeof(lifecycle_calls));
    ok &= check(lifecycle_calls == 1,
                "stopped guest lifecycle probe did not run exactly once");
    return ok ? 0 : 1;
}
