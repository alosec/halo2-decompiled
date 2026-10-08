#include "h2/guest_arena.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <limits>

#include <sys/mman.h>
#include <unistd.h>

namespace h2::port {
namespace {

constexpr std::size_t kMinimumHeaderSize = 0x168;
constexpr std::size_t kSectionRecordSize = 56;
constexpr std::size_t kMaximumXbeSize = 1ULL << 30;

bool range_valid(std::size_t offset, std::size_t size, std::size_t total) {
    return offset <= total && size <= total - offset;
}

bool read_u32(const std::vector<unsigned char>& data, std::size_t offset,
              std::uint32_t& value) {
    if (!range_valid(offset, 4, data.size())) {
        return false;
    }
    value = static_cast<std::uint32_t>(data[offset]) |
            (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
            (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
            (static_cast<std::uint32_t>(data[offset + 3]) << 24);
    return true;
}

bool va_to_offset(std::uint32_t address, std::uint32_t base, std::size_t size,
                  std::size_t file_size, std::size_t& offset) {
    if (address < base) {
        return false;
    }
    offset = static_cast<std::size_t>(address - base);
    return range_valid(offset, size, file_size);
}

bool read_name(const std::vector<unsigned char>& data, std::uint32_t address,
               std::uint32_t base, std::string& name) {
    std::size_t offset = 0;
    if (!va_to_offset(address, base, 1, data.size(), offset)) {
        return false;
    }
    const auto begin = data.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto end = std::find(begin, data.end(), 0);
    if (end == data.end()) {
        return false;
    }
    name.assign(begin, end);
    return true;
}

}  // namespace

GuestArena::GuestArena(void* reservation) : reservation_(reservation) {
    const long page_size = sysconf(_SC_PAGESIZE);
    page_size_ = page_size > 0 ? static_cast<std::size_t>(page_size) : 0;
}

GuestArena::~GuestArena() {
    if (reservation_) {
        munmap(reservation_, kGuestAddressSpaceSize);
    }
}

std::unique_ptr<GuestArena> GuestArena::create(std::string& error) {
    void* const requested = reinterpret_cast<void*>(kGuestBias);
    void* const reservation = mmap(requested, kGuestAddressSpaceSize, PROT_NONE,
                                   MAP_PRIVATE | MAP_ANON, -1, 0);
    if (reservation == MAP_FAILED) {
        error = "could not reserve the 4 GB guest arena: " + std::string(std::strerror(errno));
        return nullptr;
    }
    if (reservation != requested) {
        munmap(reservation, kGuestAddressSpaceSize);
        error = "macOS could not reserve the guest arena at its required address";
        return nullptr;
    }
    auto arena = std::unique_ptr<GuestArena>(new GuestArena(reservation));
    if (!arena->page_size_) {
        error = "could not determine the host page size";
        return nullptr;
    }
    return arena;
}

void* GuestArena::pointer(std::uint32_t guest_address) const {
    return reinterpret_cast<void*>(kGuestBias + guest_address);
}

const void* GuestArena::pointer_const(std::uint32_t guest_address) const {
    return reinterpret_cast<const void*>(kGuestBias + guest_address);
}

bool GuestArena::make_writable(std::uint32_t address, std::uint32_t size,
                               std::string& error) {
    if (!size) {
        return true;
    }
    const std::uint64_t end = static_cast<std::uint64_t>(address) + size;
    if (end > kGuestAddressSpaceSize) {
        error = "XBE section extends beyond the 32-bit guest address space";
        return false;
    }
    const std::uint64_t mask = page_size_ - 1;
    const std::uint64_t first = static_cast<std::uint64_t>(address) & ~mask;
    const std::uint64_t last = (end + mask) & ~mask;
    if (mprotect(reinterpret_cast<void*>(kGuestBias + first),
                 static_cast<std::size_t>(last - first), PROT_READ | PROT_WRITE) != 0) {
        error = "could not commit guest memory: " + std::string(std::strerror(errno));
        return false;
    }
    return true;
}

bool GuestArena::load_xbe(const std::filesystem::path& path, XbeLoadReport& report,
                          std::string& error) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) {
        error = "could not open XBE: " + path.string();
        return false;
    }
    const auto end = input.tellg();
    if (end < 0 || static_cast<std::uintmax_t>(end) > kMaximumXbeSize) {
        error = "XBE has an invalid or unreasonable size";
        return false;
    }
    std::vector<unsigned char> data(static_cast<std::size_t>(end));
    input.seekg(0);
    if (!data.empty() && !input.read(reinterpret_cast<char*>(data.data()),
                                     static_cast<std::streamsize>(data.size()))) {
        error = "could not read XBE";
        return false;
    }
    if (data.size() < kMinimumHeaderSize ||
        data[0] != 'X' || data[1] != 'B' || data[2] != 'E' || data[3] != 'H') {
        error = "file is not a valid XBE header";
        return false;
    }

    std::uint32_t section_count = 0;
    std::uint32_t section_table_address = 0;
    if (!read_u32(data, 0x104, report.image_base) ||
        !read_u32(data, 0x11c, section_count) ||
        !read_u32(data, 0x120, section_table_address) ||
        !read_u32(data, 0x128, report.encoded_entry)) {
        error = "XBE header is truncated";
        return false;
    }
    if (section_count > 4096) {
        error = "XBE has an unreasonable section count";
        return false;
    }
    std::size_t section_table = 0;
    const std::size_t table_size = static_cast<std::size_t>(section_count) * kSectionRecordSize;
    if (!va_to_offset(section_table_address, report.image_base, table_size,
                      data.size(), section_table)) {
        error = "XBE section table points outside the file";
        return false;
    }

    report.sections.clear();
    std::size_t header_size = data.size();
    for (std::uint32_t index = 0; index < section_count; ++index) {
        const std::size_t offset = section_table + index * kSectionRecordSize;
        XbeSection section;
        std::uint32_t name_address = 0;
        if (!read_u32(data, offset, section.flags) ||
            !read_u32(data, offset + 4, section.virtual_address) ||
            !read_u32(data, offset + 8, section.virtual_size) ||
            !read_u32(data, offset + 12, section.raw_offset) ||
            !read_u32(data, offset + 16, section.raw_size) ||
            !read_u32(data, offset + 20, name_address) ||
            !read_name(data, name_address, report.image_base, section.name)) {
            error = "XBE section record is malformed";
            return false;
        }
        if (!range_valid(section.raw_offset, section.raw_size, data.size()) ||
            section.raw_size > section.virtual_size) {
            error = "XBE section data is outside the file or larger than its virtual size";
            return false;
        }
        const std::uint64_t virtual_end =
            static_cast<std::uint64_t>(section.virtual_address) + section.virtual_size;
        if (virtual_end > kGuestAddressSpaceSize) {
            error = "XBE section is outside the guest address space";
            return false;
        }
        if (section.raw_offset) {
            header_size = std::min(header_size, static_cast<std::size_t>(section.raw_offset));
        }
        report.sections.push_back(std::move(section));
    }

    if (header_size > kGuestAddressSpaceSize - report.image_base ||
        !make_writable(report.image_base, static_cast<std::uint32_t>(header_size), error)) {
        if (error.empty()) {
            error = "XBE header does not fit in guest memory";
        }
        return false;
    }
    std::memcpy(pointer(report.image_base), data.data(), header_size);

    for (const auto& section : report.sections) {
        if (!make_writable(section.virtual_address, section.virtual_size, error)) {
            return false;
        }
        if (section.raw_size) {
            std::memcpy(pointer(section.virtual_address), data.data() + section.raw_offset,
                        section.raw_size);
        }
    }
    return true;
}

}  // namespace h2::port
