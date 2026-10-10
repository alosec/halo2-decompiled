#include "h2/guest_arena.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

namespace {

void write_u32(std::vector<unsigned char>& data, std::size_t offset, std::uint32_t value) {
    data[offset] = static_cast<unsigned char>(value);
    data[offset + 1] = static_cast<unsigned char>(value >> 8);
    data[offset + 2] = static_cast<unsigned char>(value >> 16);
    data[offset + 3] = static_cast<unsigned char>(value >> 24);
}

}  // namespace

int main() {
    constexpr std::uint32_t kBase = 0x10000;
    constexpr std::uint32_t kSectionAddress = 0x20000;
    std::vector<unsigned char> image(0x2004, 0);
    image[0] = 'X';
    image[1] = 'B';
    image[2] = 'E';
    image[3] = 'H';
    write_u32(image, 0x104, kBase);
    write_u32(image, 0x11c, 1);
    write_u32(image, 0x120, kBase + 0x200);
    write_u32(image, 0x128, 0x12345678);
    write_u32(image, 0x200, 7);
    write_u32(image, 0x204, kSectionAddress);
    write_u32(image, 0x208, 0x8000);
    write_u32(image, 0x20c, 0x2000);
    write_u32(image, 0x210, 4);
    write_u32(image, 0x214, kBase + 0x300);
    const char name[] = ".data";
    std::copy(std::begin(name), std::end(name), image.begin() + 0x300);
    image[0x2000] = 0xde;
    image[0x2001] = 0xad;
    image[0x2002] = 0xbe;
    image[0x2003] = 0xef;

    const auto path = std::filesystem::temp_directory_path() /
                      ("h2-guest-arena-" + std::to_string(getpid()) + ".xbe");
    {
        std::ofstream output(path, std::ios::binary);
        output.write(reinterpret_cast<const char*>(image.data()),
                     static_cast<std::streamsize>(image.size()));
    }

    std::string error;
    auto arena = h2::port::GuestArena::create(error);
    if (!arena) {
        std::cerr << error << '\n';
        std::filesystem::remove(path);
        return 1;
    }
    h2::port::XbeLoadReport report;
    if (!arena->load_xbe(path, report, error)) {
        std::cerr << error << '\n';
        std::filesystem::remove(path);
        return 1;
    }
    std::filesystem::remove(path);

    const auto* header = static_cast<const unsigned char*>(arena->pointer_const(kBase));
    const auto* section = static_cast<const unsigned char*>(arena->pointer_const(kSectionAddress));
    if (report.image_base != kBase || report.encoded_entry != 0x12345678 ||
        report.sections.size() != 1 || report.sections[0].name != ".data" ||
        header[0] != 'X' || header[3] != 'H' || section[0] != 0xde ||
        section[3] != 0xef || section[0x4000] != 0) {
        std::cerr << "synthetic XBE did not initialize guest memory correctly\n";
        return 1;
    }

    unsigned char host_byte = 0;
    constexpr std::uint32_t kProbeAddress = 0x40000;
    if (!arena->host_range_accessible(header, 4, false) ||
        !arena->host_range_accessible(section, 0x8000, true) ||
        arena->host_range_accessible(&host_byte, 1, false) ||
        arena->host_range_accessible(arena->pointer(kProbeAddress), 1, false) ||
        arena->host_range_accessible(arena->pointer(0xfffffff0u), 0x20, false)) {
        std::cerr << "initial guest accessibility validation failed\n";
        return 1;
    }
    if (!arena->protect(kProbeAddress, static_cast<std::uint32_t>(arena->page_size()),
                        h2::port::GuestMemoryProtection::read_write, error) ||
        !arena->host_range_accessible(arena->pointer(kProbeAddress),
                                      arena->page_size(), true) ||
        !arena->protect(kProbeAddress, static_cast<std::uint32_t>(arena->page_size()),
                        h2::port::GuestMemoryProtection::read_only, error) ||
        !arena->host_range_accessible(arena->pointer(kProbeAddress),
                                      arena->page_size(), false) ||
        arena->host_range_accessible(arena->pointer(kProbeAddress), 1, true) ||
        arena->host_range_executable(arena->pointer(kProbeAddress), 1) ||
        !arena->protect(kProbeAddress, static_cast<std::uint32_t>(arena->page_size()),
                        h2::port::GuestMemoryProtection::read_execute, error) ||
        !arena->host_range_executable(arena->pointer(kProbeAddress),
                                      arena->page_size()) ||
        arena->host_range_executable(&host_byte, 1) ||
        !arena->protect(kProbeAddress, static_cast<std::uint32_t>(arena->page_size()),
                        h2::port::GuestMemoryProtection::none, error) ||
        arena->host_range_accessible(arena->pointer(kProbeAddress), 1, false) ||
        arena->host_range_executable(arena->pointer(kProbeAddress), 1)) {
        std::cerr << "guest protection accessibility tracking failed: " << error << '\n';
        return 1;
    }
    return 0;
}
