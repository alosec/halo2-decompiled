#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"
#include "h2/native_cache_file_slots.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

namespace {

bool check(bool condition, const char* message) {
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
                 const std::string& name, bool valid = true) {
    std::vector<std::uint8_t> bytes(0x1000, 0);
    write_u32(bytes, 0, valid ? 0x68656164u : 0u);
    write_u32(bytes, 4, 8);
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

int main() {
    using Lifecycle = h2::port::NativeAsyncLifecycle;
    using Platform = h2::port::NativeAsyncPlatform;
    using Slots = h2::port::NativeCacheFileSlots;
    bool ok = true;

    const auto directory = std::filesystem::temp_directory_path() /
        ("h2-native-cache-slots-" + std::to_string(getpid()));
    std::error_code filesystem_error;
    std::filesystem::create_directory(directory, filesystem_error);
    const auto mainmenu = directory / "mainmenu.map";
    const auto shared = directory / "shared.map";
    const auto single_player_shared = directory / "single_player_shared.map";
    const auto campaign_a = directory / "campaign_a.map";
    const auto campaign_b = directory / "campaign_b.map";
    const auto malformed = directory / "malformed.map";
    ok &= check(write_cache(mainmenu, 2, "mainmenu") &&
                    write_cache(shared, 3, "shared") &&
                    write_cache(single_player_shared, 4, "single_player_shared") &&
                    write_cache(campaign_a, 0, "campaign_a") &&
                    write_cache(campaign_b, 0, "campaign_b") &&
                    write_cache(malformed, 0, "malformed", false),
                "could not create synthetic cache files");

    Platform platform;
    Lifecycle lifecycle;
    Slots slots(platform, lifecycle);
    std::string error;
    ok &= check(slots.install(mainmenu, 2, "mainmenu", true, error) &&
                    slots.active_slot() == 2,
                "mainmenu cache did not enter recovered slot 2");
    const Platform::Handle mainmenu_handle = slots.slot_handle(2);
    ok &= check(mainmenu_handle != Platform::invalid_handle &&
                    slots.slot_header(2)[0] == 'd',
                "mainmenu slot did not retain its token and header");
    ok &= check(slots.install(shared, 3, "shared", false, error) &&
                    slots.slot_handle(0) != Platform::invalid_handle &&
                    slots.install(single_player_shared, 4, "single_player_shared",
                                  false, error) &&
                    slots.slot_handle(1) != Platform::invalid_handle,
                "shared caches did not enter recovered slots 0 and 1");

    ok &= check(lifecycle.start(), "scheduler lifecycle did not start");
    ok &= check(!slots.install(campaign_a, 0, "campaign_a", true, error) &&
                    !slots.clear(error),
                "cache slots changed while the scheduler was accepting");
    ok &= check(lifecycle.begin_stop() && lifecycle.wait_drained(0) &&
                    lifecycle.finish_stop(),
                "scheduler lifecycle did not return to stopped");

    ok &= check(slots.install(campaign_a, 0, "campaign_a", true, error) &&
                    slots.active_slot() == 4 &&
                    slots.slot_handle(2) == mainmenu_handle,
                "first Campaign cache did not enter slot 4 or preserve mainmenu");
    const Platform::Handle campaign_a_handle = slots.slot_handle(4);
    ok &= check(slots.install(campaign_b, 0, "campaign_b", true, error) &&
                    slots.active_slot() == 5 &&
                    slots.slot_handle(4) == Platform::invalid_handle &&
                    slots.slot_handle(5) != Platform::invalid_handle,
                "second Campaign cache did not alternate to slot 5");
    std::uint64_t closed_size = 0;
    ok &= check(!platform.file_size(campaign_a_handle, &closed_size),
                "replaced Campaign token remained live");

    const auto symlink = directory / "campaign.link";
    std::filesystem::create_symlink(campaign_b, symlink, filesystem_error);
    ok &= check(!slots.install(malformed, 0, "malformed", true, error) &&
                    !slots.install(symlink, 0, "campaign_b", true, error) &&
                    !slots.install(campaign_b, 3, "campaign_b", false, error) &&
                    slots.active_slot() == 5,
                "invalid cache input changed slot ownership");

    ok &= check(slots.clear(error) &&
                    slots.active_slot() == Slots::no_active_slot &&
                    platform.active_handle_count() == 0,
                "cache slot teardown did not close and clear every token");
    std::filesystem::remove_all(directory, filesystem_error);
    return ok ? 0 : 1;
}
