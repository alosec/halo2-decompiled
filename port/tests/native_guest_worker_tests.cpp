#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"
#include "h2/native_guest_worker.hpp"
#include "h2/native_texture_pool_storage.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

extern "C" int h2_call_guest_int_0(int (*function)(), void* stack_top);

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
                 const std::string& name, std::uint8_t payload_base) {
    std::vector<std::uint8_t> bytes(0x1000, 0);
    for (std::size_t index = 0; index < 16; ++index)
        bytes[0x800 + index] = static_cast<std::uint8_t>(payload_base + index);
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
    const auto recovered_worker_step =
        image->symbol_address("h2_native_async_worker_step");
    const auto active_read_submit =
        image->symbol_address("h2_async_submit_active_read");
    const auto shared_read_submit =
        image->symbol_address("h2_async_submit_shared_read");
    const auto campaign_read_submit =
        image->symbol_address("h2_async_submit_campaign_read");
    const auto read_completed = image->symbol_address("h2_async_read_completed");
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
    const auto texture_math_probe = image->symbol_address("h2_texture_math_probe");
    const auto texture_upload_policy_probe =
        image->symbol_address("h2_texture_upload_policy_probe");
    const auto texture_pool_probe = image->symbol_address("h2_texture_pool_probe");
    const auto texture_upload_initialize =
        image->symbol_address("h2_texture_upload_initialize");
    const auto texture_upload_submit =
        image->symbol_address("h2_texture_upload_submit");
    const auto texture_upload_completed =
        image->symbol_address("h2_texture_upload_completed");
    const auto texture_cache_callback_probe =
        image->symbol_address("h2_texture_cache_callback_probe");
    const auto texture_cache_flush =
        image->symbol_address("h2_native_texture_cache_flush");
    const auto texture_cache_flush_completed =
        image->symbol_address("h2_texture_cache_flush_completed");
    const auto texture_block_delete =
        image->symbol_address("?texture_cache_block_delete");
    const auto texture_entry_busy =
        image->symbol_address("?texture_cache_entry_busy");
    const auto texture_entry_state =
        image->symbol_address("?texture_cache_entry_state");
    ok &= check(entry.has_value() && recovered_worker_step.has_value() &&
                    active_read_submit.has_value() &&
                    shared_read_submit.has_value() &&
                    campaign_read_submit.has_value() && read_completed.has_value() &&
                    counter.has_value() && lifecycle_entry.has_value() &&
                    lifecycle_counter.has_value() && queue_initialize.has_value() &&
                    queue_teardown.has_value() &&
                    queue_free_count.has_value() &&
                    queue_remove_free_head.has_value() &&
                    queue_restore_free_head.has_value() &&
                    queue_is_cleared.has_value() && texture_math_probe.has_value() &&
                    texture_upload_policy_probe.has_value() &&
                    texture_pool_probe.has_value() &&
                    texture_upload_initialize.has_value() &&
                    texture_upload_submit.has_value() &&
                    texture_upload_completed.has_value() &&
                    texture_cache_callback_probe.has_value() &&
                    texture_cache_flush.has_value() &&
                    texture_cache_flush_completed.has_value() &&
                    texture_block_delete.has_value() &&
                    texture_entry_busy.has_value() &&
                    texture_entry_state.has_value(),
                "guest worker fixture symbols are missing");
    if (!entry || !recovered_worker_step || !active_read_submit ||
        !shared_read_submit || !campaign_read_submit || !read_completed ||
        !counter || !lifecycle_entry || !lifecycle_counter ||
        !queue_initialize ||
        !queue_teardown || !queue_free_count || !queue_remove_free_head ||
        !queue_restore_free_head || !queue_is_cleared || !texture_math_probe ||
        !texture_upload_policy_probe || !texture_pool_probe ||
        !texture_upload_initialize || !texture_upload_submit ||
        !texture_upload_completed || !texture_cache_callback_probe ||
        !texture_cache_flush || !texture_cache_flush_completed ||
        !texture_block_delete || !texture_entry_busy || !texture_entry_state)
        return 1;

    const h2::port::AsyncTextureGuestEntries texture_entries{
        .worker_step = *recovered_worker_step,
        .queue_initialize = *queue_initialize,
        .queue_teardown = *queue_teardown,
        .texture_flush = *texture_cache_flush,
        .block_delete = *texture_block_delete,
        .entry_busy = *texture_entry_busy,
        .entry_state = *texture_entry_state,
    };
    const auto install_texture_pools = [&] {
        return h2::port::install_async_texture_pools(
            texture_entries.block_delete, texture_entries.entry_busy,
            texture_entries.entry_state, error);
    };

    Platform platform;
    Lifecycle lifecycle;
    Worker worker(*arena, platform, lifecycle);
    int invoke_result = 0;
    ok &= check(worker.invoke_stopped(*texture_math_probe, invoke_result, error) &&
                    invoke_result == 1,
                "recovered texture math probe failed: " + error);
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
    constexpr std::uint32_t submit_stack_address = 0x20200000u;
    constexpr std::uint32_t submit_stack_size = 0x00010000u;
    ok &= check(arena->protect(cache_slots_address, cache_slots_size,
                               h2::port::GuestMemoryProtection::read_write,
                               error) &&
                    arena->protect(submit_stack_address, submit_stack_size,
                                   h2::port::GuestMemoryProtection::read_write,
                                   error),
                "guest cache slots or submission stack could not be mapped: " + error);
    void* const submit_stack_top = arena->pointer(
        submit_stack_address + submit_stack_size - 16);
    const auto cache_directory = std::filesystem::temp_directory_path() /
        ("h2-async-host-cache-" + std::to_string(getpid()));
    std::error_code filesystem_error;
    std::filesystem::create_directory(cache_directory, filesystem_error);
    const auto mainmenu_path = cache_directory / "mainmenu.map";
    const auto shared_path = cache_directory / "shared.map";
    const auto campaign_path = cache_directory / "campaign.map";
    ok &= check(!filesystem_error &&
                    write_cache(mainmenu_path, 2, "mainmenu", 0xa0u) &&
                    write_cache(shared_path, 3, "shared", 0xc0u) &&
                    write_cache(campaign_path, 0, "campaign", 0xe0u),
                "synthetic async cache files could not be created");

    ok &= check(h2::port::configure_async_services(*arena, error) &&
                    h2::port::patch_async_host_imports(*image, *arena, error),
                error);
    ok &= check(!h2::port::install_async_texture_pools(
                    cache_slots_address, *texture_entry_busy,
                    *texture_entry_state, error) &&
                    error.find("callback is not executable") != std::string::npos,
                "texture pools accepted a writable callback address");
    ok &= check(h2::port::install_async_cache_file(
                    mainmenu_path, 2, "mainmenu", true, error) &&
                    h2::port::install_async_cache_file(
                        shared_path, 3, "shared", false, error) &&
                    install_texture_pools(),
                "host async cache slots and texture pools did not install: " + error);
    const auto read_guest_u32 = [&](std::uint32_t address) {
        std::uint32_t value = 0;
        std::memcpy(&value, arena->pointer(address), sizeof(value));
        return value;
    };
    using TexturePools = h2::port::NativeTexturePoolStorage;
    ok &= check(
        read_guest_u32(TexturePools::global_texture_pool) ==
                TexturePools::texture_pool_address &&
            read_guest_u32(TexturePools::global_upload_buffer) ==
                TexturePools::upload_buffer_address &&
            read_guest_u32(TexturePools::global_physical_object) ==
                TexturePools::physical_object_address &&
            read_guest_u32(TexturePools::global_upload_count) == 0 &&
            read_guest_u32(TexturePools::texture_pool_address + 0x20) ==
                TexturePools::maximum_entries &&
            read_guest_u32(TexturePools::texture_pool_address + 0x24) == 0x28 &&
            read_guest_u32(TexturePools::texture_pool_address + 0x44) ==
                TexturePools::texture_entries_address &&
            read_guest_u32(TexturePools::texture_pool_address + 0x48) ==
                TexturePools::texture_bitmap_address &&
            read_guest_u32(TexturePools::physical_object_address + 0x20) ==
                *texture_block_delete &&
            read_guest_u32(TexturePools::physical_object_address + 0x24) ==
                *texture_entry_busy &&
            read_guest_u32(TexturePools::physical_object_address + 0x28) ==
                *texture_entry_state &&
            read_guest_u32(TexturePools::physical_object_address + 0x30) ==
                TexturePools::upload_page_count &&
            read_guest_u32(TexturePools::physical_object_address + 0x34) == 12 &&
            read_guest_u32(TexturePools::physical_object_address + 0x64) ==
                TexturePools::physical_pool_address,
        "host texture pools published an invalid guest layout");
    ok &= check(worker.invoke_stopped(*texture_upload_policy_probe, invoke_result,
                                      error) &&
                    invoke_result == 1,
                "complete recovered texture upload policy probe failed: " + error);
    ok &= check(worker.invoke_stopped(*texture_pool_probe, invoke_result, error) &&
                    invoke_result == 1,
                "recovered texture/physical pool probe failed: " + error);
    ok &= check(worker.invoke_stopped(*texture_pool_probe, invoke_result, error) &&
                    invoke_result == 1,
                "recovered texture/physical pools did not reinitialize: " + error);
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
    ok &= check(!install_texture_pools() &&
                    error.find("queue must be stopped") != std::string::npos &&
                    !h2::port::clear_async_texture_pools(error) &&
                    error.find("queue must be torn down") != std::string::npos,
                "texture pool lifecycle changed while the guest queue was live");
    ok &= check(worker.invoke_stopped(*texture_upload_initialize, invoke_result,
                                      error) &&
                    invoke_result == 1,
                "texture upload pools did not initialize: " + error);
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
    const auto execute_submission = [&](std::uint32_t submit_address,
                                        std::uint32_t completed_address) {
        auto* const submit = reinterpret_cast<int (*)()>(
            arena->pointer(submit_address));
        int completed_result = 0;
        return h2::port::start_async_worker(*recovered_worker_step, error) &&
               h2_call_guest_int_0(submit, submit_stack_top) == 1 &&
               h2::port::shutdown_async_worker(Lifecycle::infinite, error) &&
               worker.invoke_stopped(completed_address, completed_result, error) &&
               completed_result == 1 &&
               worker.invoke_stopped(*queue_free_count, free_count, error) &&
               free_count == 150;
    };
    const auto execute_read = [&](std::uint32_t submit_address) {
        return execute_submission(submit_address, *read_completed);
    };
    const auto execute_running_read = [&](std::uint32_t submit_address) {
        auto* const submit = reinterpret_cast<int (*)()>(
            arena->pointer(submit_address));
        int completed_result = 0;
        return h2_call_guest_int_0(submit, submit_stack_top) == 1 &&
               h2::port::shutdown_async_worker(Lifecycle::infinite, error) &&
               worker.invoke_stopped(*read_completed, completed_result, error) &&
               completed_result == 1 &&
               worker.invoke_stopped(*queue_free_count, free_count, error) &&
               free_count == 150;
    };
    ok &= check(calls == 4 &&
                    execute_submission(*texture_upload_submit,
                                       *texture_upload_completed),
                "recovered texture upload allocation did not complete: " + error);
    ok &= check(!h2::port::flush_async_texture_pools(
                    *texture_cache_flush, error) &&
                    error.find("queue must be torn down") != std::string::npos &&
                    h2::port::teardown_async_queue(*queue_teardown, error) &&
                    !h2::port::flush_async_texture_pools(
                        *texture_cache_callback_probe, error) &&
                    error.find("did not release all records") != std::string::npos &&
                    !h2::port::clear_async_texture_pools(error) &&
                    error.find("live records") != std::string::npos,
                "texture pool lifetime ordering was not enforced");
    ok &= check(h2::port::flush_async_texture_pools(
                    *texture_cache_flush, error) &&
                    worker.invoke_stopped(*texture_cache_flush_completed,
                                          invoke_result, error) &&
                    invoke_result == 1 &&
                    h2::port::initialize_async_queue(*queue_initialize, error),
                "ordered texture cache flush did not release source bitmaps: " +
                    error);
    ok &= check(execute_read(*active_read_submit) &&
                    execute_read(*shared_read_submit),
                "recovered active/shared cache routing did not read and recycle: " +
                    error);
    ok &= check(worker.invoke_stopped(*texture_upload_initialize, invoke_result,
                                      error) &&
                    invoke_result == 1 &&
                    execute_submission(*texture_upload_submit,
                                       *texture_upload_completed),
                "pre-switch resident texture upload did not complete: " + error);
    ok &= check(!h2::port::install_async_cache_file(
                    campaign_path, 0, "campaign", true, error) &&
                    error.find("texture pools must clear") != std::string::npos,
                "active cache replacement retained live texture pools");
    ok &= check(h2::port::switch_async_texture_map(
                    campaign_path, 0, "campaign", texture_entries,
                    Lifecycle::infinite, error),
                "ordered texture-pool/cache map switch failed: " + error);
    std::memcpy(&published_active, arena->pointer(cache_active_address),
                sizeof(published_active));
    ok &= check(published_active == 4 &&
                    execute_running_read(*campaign_read_submit) &&
                    worker.invoke_stopped(*texture_cache_flush_completed,
                                          invoke_result, error) &&
                    invoke_result == 1 &&
                    execute_read(*shared_read_submit),
                "recovered dynamic/shared cache routing did not read and recycle: " +
                    error);
    ok &= check(!h2::port::reset_async_services(error) &&
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
                    error.find("texture pools are still installed") != std::string::npos &&
                    !h2::port::clear_async_cache_files(error) &&
                    error.find("texture pools must clear") != std::string::npos &&
                    h2::port::clear_async_texture_pools(error) &&
                    !h2::port::reset_async_services(error) &&
                    error.find("live handles") != std::string::npos &&
                    h2::port::clear_async_cache_files(error) &&
                    h2::port::reset_async_services(error) &&
                    !host_h2_async_try_admit(),
                "recovered queue/pool/cache teardown retained ownership: " + error);
    std::uint32_t cleared_handle = 1;
    std::int32_t cleared_active = 0;
    std::memcpy(&cleared_handle, arena->pointer(cache_slots_address),
                sizeof(cleared_handle));
    std::memcpy(&cleared_active, arena->pointer(cache_active_address),
                sizeof(cleared_active));
    ok &= check(cleared_handle == Platform::invalid_handle &&
                    cleared_active == -1 &&
                    read_guest_u32(TexturePools::global_texture_pool) == 0 &&
                    read_guest_u32(TexturePools::global_upload_buffer) == 0 &&
                    read_guest_u32(TexturePools::global_physical_object) == 0 &&
                    read_guest_u32(TexturePools::global_upload_count) == 0 &&
                    read_guest_u32(TexturePools::texture_pool_address + 0x20) == 0,
                "host cache/pool clear retained guest ownership");

    const std::array controller_caches{
        h2::port::AsyncCacheFileConfiguration{
            mainmenu_path, 2, "mainmenu", true},
        h2::port::AsyncCacheFileConfiguration{
            shared_path, 3, "shared", false},
    };
    const std::array invalid_controller_caches{
        h2::port::AsyncCacheFileConfiguration{
            mainmenu_path, 2, "mainmenu", false},
    };
    const std::array duplicate_controller_caches{
        h2::port::AsyncCacheFileConfiguration{
            mainmenu_path, 2, "mainmenu", true},
        h2::port::AsyncCacheFileConfiguration{
            mainmenu_path, 2, "mainmenu", false},
    };
    auto invalid_controller_entries = texture_entries;
    invalid_controller_entries.block_delete = cache_slots_address;
    ok &= check(!h2::port::start_async_texture_controller(
                    *arena, invalid_controller_caches, texture_entries, error) &&
                    error.find("exactly one active cache") != std::string::npos &&
                    !h2::port::start_async_texture_controller(
                        *arena, duplicate_controller_caches, texture_entries,
                        error) &&
                    error.find("cache types are invalid") != std::string::npos &&
                    !h2::port::start_async_texture_controller(
                        *arena, controller_caches, invalid_controller_entries,
                        error) &&
                    error.find("texture pool installation") != std::string::npos &&
                    h2::port::start_async_texture_controller(
                        *arena, controller_caches, texture_entries, error) &&
                    h2::port::stop_async_texture_controller(
                        texture_entries, Lifecycle::infinite, error) &&
                    !host_h2_async_try_admit(),
                "complete async texture controller did not restart cleanly: " +
                    error);
    std::filesystem::remove_all(cache_directory, filesystem_error);
    std::uint32_t lifecycle_calls = 0;
    std::memcpy(&lifecycle_calls, arena->pointer(*lifecycle_counter),
                sizeof(lifecycle_calls));
    ok &= check(lifecycle_calls == 1,
                "stopped guest lifecycle probe did not run exactly once");
    return ok ? 0 : 1;
}
