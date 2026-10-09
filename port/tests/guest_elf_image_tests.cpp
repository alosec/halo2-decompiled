#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

bool check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
    }
    return condition;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: h2-guest-elf-image-tests IMAGE\n";
        return 2;
    }

    std::string error;
    auto arena = h2::port::GuestArena::create(error);
    if (!arena) {
        std::cerr << error << '\n';
        return 1;
    }
    auto image = h2::port::GuestElfImage::load(std::filesystem::path(argv[1]),
                                                *arena, error);
    if (!image) {
        std::cerr << error << '\n';
        return 1;
    }

    bool ok = true;
    const auto probe_address = image->symbol_address("h2_loader_probe");
    const auto data_address = image->symbol_address("h2_loader_data");
    const auto slot_address = image->symbol_address("h2_host_slot_host_h2_probe");
    ok &= check(probe_address.has_value() && data_address.has_value() &&
                    slot_address.has_value(),
                "loaded guest ELF is missing fixture symbols");
    if (!ok) {
        return 1;
    }

    std::uint32_t value = 0;
    std::memcpy(&value, arena->pointer(*data_address), sizeof(value));
    ok &= check(value == 0x12345678, "guest ELF data segment was not copied");

    using Probe = int (*)();
    const std::uintptr_t native_probe = h2::port::kGuestBias + *probe_address;
    Probe probe = nullptr;
    std::memcpy(&probe, &native_probe, sizeof(probe));
    ok &= check(probe() == 77, "loaded guest ELF code did not execute");

    constexpr std::uintptr_t kTarget = 0x123456789abcdef0ULL;
    ok &= check(image->patch_host_import(*arena, "host_h2_probe", kTarget, error), error);
    std::uintptr_t patched = 0;
    std::memcpy(&patched, arena->pointer(*slot_address), sizeof(patched));
    ok &= check(patched == kTarget, "host import target was not patched");

    ok &= check(h2::port::patch_standard_host_imports(*image, *arena, error), error);
    for (const char* symbol : {"host_h2_video_control", "host_h2_render_request",
                               "host_h2_get_milliseconds", "host_h2_sleep",
                               "host_h2_cos"}) {
        const auto standard_slot = image->symbol_address(
            std::string("h2_host_slot_") + symbol);
        std::uintptr_t standard_target = 0;
        if (standard_slot) {
            std::memcpy(&standard_target, arena->pointer(*standard_slot),
                        sizeof(standard_target));
        }
        ok &= check(standard_slot.has_value() && standard_target != 0,
                    std::string("standard host import was not patched: ") + symbol);
    }

    std::string missing_error;
    ok &= check(!image->patch_host_import(*arena, "host_h2_missing", kTarget,
                                          missing_error) &&
                    missing_error.find("h2_host_slot_host_h2_missing") != std::string::npos,
                "missing host import slot was not rejected precisely");

    if (ok) {
        std::cout << "native guest ELF load, execution, and host patching passed\n";
    }
    return ok ? 0 : 1;
}
