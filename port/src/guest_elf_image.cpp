#include "h2/guest_elf_image.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <limits>

namespace h2::port {
namespace {

constexpr std::size_t kElfHeaderSize = 64;
constexpr std::size_t kProgramHeaderSize = 56;
constexpr std::size_t kSectionHeaderSize = 64;
constexpr std::size_t kSymbolSize = 24;
constexpr std::size_t kMaximumImageSize = 1ULL << 30;
constexpr std::uint32_t kLoadSegment = 1;
constexpr std::uint32_t kSymbolTable = 2;
constexpr std::uint32_t kFlagExecute = 1;
constexpr std::uint32_t kFlagWrite = 2;

bool range_valid(std::uint64_t offset, std::uint64_t size, std::size_t total) {
    return offset <= total && size <= total - offset;
}

std::uint16_t read_u16(const std::vector<unsigned char>& data, std::size_t offset) {
    std::uint16_t value = 0;
    std::memcpy(&value, data.data() + offset, sizeof(value));
    return value;
}

std::uint32_t read_u32(const std::vector<unsigned char>& data, std::size_t offset) {
    std::uint32_t value = 0;
    std::memcpy(&value, data.data() + offset, sizeof(value));
    return value;
}

std::uint64_t read_u64(const std::vector<unsigned char>& data, std::size_t offset) {
    std::uint64_t value = 0;
    std::memcpy(&value, data.data() + offset, sizeof(value));
    return value;
}

bool read_file(const std::filesystem::path& path, std::vector<unsigned char>& data,
               std::string& error) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) {
        error = "could not open guest ELF: " + path.string();
        return false;
    }
    const auto end = input.tellg();
    if (end < 0 || static_cast<std::uintmax_t>(end) > kMaximumImageSize) {
        error = "guest ELF has an invalid or unreasonable size";
        return false;
    }
    data.resize(static_cast<std::size_t>(end));
    input.seekg(0);
    if (!data.empty() && !input.read(reinterpret_cast<char*>(data.data()),
                                     static_cast<std::streamsize>(data.size()))) {
        error = "could not read guest ELF";
        return false;
    }
    return true;
}

std::optional<std::string> read_string(const std::vector<unsigned char>& data,
                                       std::size_t table_offset,
                                       std::size_t table_size,
                                       std::uint32_t string_offset) {
    if (string_offset >= table_size) {
        return std::nullopt;
    }
    const std::size_t begin = table_offset + string_offset;
    const std::size_t limit = table_offset + table_size;
    const auto end = std::find(data.begin() + static_cast<std::ptrdiff_t>(begin),
                               data.begin() + static_cast<std::ptrdiff_t>(limit), 0);
    if (end == data.begin() + static_cast<std::ptrdiff_t>(limit)) {
        return std::nullopt;
    }
    return std::string(data.begin() + static_cast<std::ptrdiff_t>(begin), end);
}

}  // namespace

std::optional<GuestElfImage> GuestElfImage::load(const std::filesystem::path& path,
                                                 GuestArena& arena,
                                                 std::string& error) {
    static_assert(sizeof(std::uintptr_t) == 8);
    std::vector<unsigned char> data;
    if (!read_file(path, data, error)) {
        return std::nullopt;
    }
    if (data.size() < kElfHeaderSize || data[0] != 0x7f || data[1] != 'E' ||
        data[2] != 'L' || data[3] != 'F' || data[4] != 2 || data[5] != 1 ||
        data[6] != 1 || read_u16(data, 16) != 2 || read_u16(data, 18) != 183 ||
        read_u32(data, 20) != 1) {
        error = "guest image is not a little-endian AArch64 executable ELF";
        return std::nullopt;
    }

    const std::uint64_t program_offset = read_u64(data, 32);
    const std::uint64_t section_offset = read_u64(data, 40);
    const std::uint16_t program_size = read_u16(data, 54);
    const std::uint16_t program_count = read_u16(data, 56);
    const std::uint16_t section_size = read_u16(data, 58);
    const std::uint16_t section_count = read_u16(data, 60);
    if (program_size != kProgramHeaderSize || section_size != kSectionHeaderSize ||
        !range_valid(program_offset, static_cast<std::uint64_t>(program_size) * program_count,
                     data.size()) ||
        !range_valid(section_offset, static_cast<std::uint64_t>(section_size) * section_count,
                     data.size())) {
        error = "guest ELF header tables are malformed";
        return std::nullopt;
    }

    struct PendingSegment {
        std::uint32_t address;
        std::uint32_t size;
        std::size_t file_offset;
        std::size_t file_size;
        GuestMemoryProtection protection;
    };
    std::vector<PendingSegment> pending;
    for (std::uint16_t index = 0; index < program_count; ++index) {
        const std::size_t offset = static_cast<std::size_t>(program_offset) +
                                   index * kProgramHeaderSize;
        if (read_u32(data, offset) != kLoadSegment) {
            continue;
        }
        const std::uint32_t flags = read_u32(data, offset + 4);
        const std::uint64_t file_offset = read_u64(data, offset + 8);
        const std::uint64_t address = read_u64(data, offset + 16);
        const std::uint64_t file_size = read_u64(data, offset + 32);
        const std::uint64_t memory_size = read_u64(data, offset + 40);
        if (address > std::numeric_limits<std::uint32_t>::max() || !memory_size ||
            memory_size > std::numeric_limits<std::uint32_t>::max() ||
            memory_size > kGuestAddressSpaceSize - address || file_size > memory_size ||
            !range_valid(file_offset, file_size, data.size()) ||
            ((flags & kFlagExecute) && (flags & kFlagWrite))) {
            error = "guest ELF contains an invalid or writable-executable segment";
            return std::nullopt;
        }
        GuestMemoryProtection protection = GuestMemoryProtection::read_only;
        if (flags & kFlagExecute) {
            protection = GuestMemoryProtection::read_execute;
        } else if (flags & kFlagWrite) {
            protection = GuestMemoryProtection::read_write;
        }
        pending.push_back({static_cast<std::uint32_t>(address),
                           static_cast<std::uint32_t>(memory_size),
                           static_cast<std::size_t>(file_offset),
                           static_cast<std::size_t>(file_size), protection});
    }
    if (pending.empty()) {
        error = "guest ELF has no loadable segments";
        return std::nullopt;
    }
    std::sort(pending.begin(), pending.end(),
              [](const auto& left, const auto& right) {
                  return left.address < right.address;
              });
    const std::uint64_t page_mask = arena.page_size() - 1;
    std::uint64_t previous_page_end = 0;
    for (const auto& segment : pending) {
        const std::uint64_t page_begin = segment.address & ~page_mask;
        const std::uint64_t page_end =
            (static_cast<std::uint64_t>(segment.address) + segment.size + page_mask) &
            ~page_mask;
        if (page_begin < previous_page_end) {
            error = "guest ELF load segments share host pages";
            return std::nullopt;
        }
        previous_page_end = page_end;
    }

    GuestElfImage image;
    for (const auto& segment : pending) {
        if (!arena.protect(segment.address, segment.size,
                           GuestMemoryProtection::read_write, error)) {
            return std::nullopt;
        }
        std::memset(arena.pointer(segment.address), 0, segment.size);
        std::memcpy(arena.pointer(segment.address), data.data() + segment.file_offset,
                    segment.file_size);
        image.segments_.push_back({segment.address, segment.size, segment.protection});
    }

    for (std::uint16_t section = 0; section < section_count; ++section) {
        const std::size_t offset = static_cast<std::size_t>(section_offset) +
                                   section * kSectionHeaderSize;
        if (read_u32(data, offset + 4) != kSymbolTable) {
            continue;
        }
        const std::uint64_t symbols_offset = read_u64(data, offset + 24);
        const std::uint64_t symbols_size = read_u64(data, offset + 32);
        const std::uint32_t strings_index = read_u32(data, offset + 40);
        const std::uint64_t entry_size = read_u64(data, offset + 56);
        if (strings_index >= section_count || entry_size != kSymbolSize ||
            symbols_size % kSymbolSize ||
            !range_valid(symbols_offset, symbols_size, data.size())) {
            error = "guest ELF symbol table is malformed";
            return std::nullopt;
        }
        const std::size_t strings_header = static_cast<std::size_t>(section_offset) +
                                           strings_index * kSectionHeaderSize;
        const std::uint64_t strings_offset = read_u64(data, strings_header + 24);
        const std::uint64_t strings_size = read_u64(data, strings_header + 32);
        if (!range_valid(strings_offset, strings_size, data.size())) {
            error = "guest ELF string table is malformed";
            return std::nullopt;
        }
        for (std::uint64_t symbol = 0; symbol < symbols_size; symbol += kSymbolSize) {
            const std::size_t symbol_offset = static_cast<std::size_t>(symbols_offset + symbol);
            const std::uint32_t name_offset = read_u32(data, symbol_offset);
            const std::uint16_t definition = read_u16(data, symbol_offset + 6);
            const std::uint64_t value = read_u64(data, symbol_offset + 8);
            if (!definition || value > std::numeric_limits<std::uint32_t>::max()) {
                continue;
            }
            auto name = read_string(data, static_cast<std::size_t>(strings_offset),
                                    static_cast<std::size_t>(strings_size), name_offset);
            if (!name) {
                error = "guest ELF contains an invalid symbol name";
                return std::nullopt;
            }
            if (!name->empty()) {
                image.symbols_.emplace(std::move(*name), static_cast<std::uint32_t>(value));
            }
        }
    }

    for (const auto& segment : pending) {
        if (!arena.protect(segment.address, segment.size, segment.protection, error)) {
            return std::nullopt;
        }
    }
    return image;
}

std::optional<std::uint32_t> GuestElfImage::symbol_address(std::string_view name) const {
    const auto found = symbols_.find(std::string(name));
    if (found != symbols_.end()) {
        return found->second;
    }
    /* Curated images name MS-mangled functions by their base ("?name"), because
       ld.lld reads the rest after '@' as a version. Whole images keep the full
       name with '@' escaped as '%', so accept the one full name with that base. */
    if (!name.empty() && name.front() == '?' &&
        name.find('@') == std::string_view::npos && name.find('%') == std::string_view::npos) {
        const std::string prefix = std::string(name) + "%%";
        std::optional<std::uint32_t> match;
        for (const auto& [symbol, address] : symbols_) {
            if (symbol.compare(0, prefix.size(), prefix) == 0) {
                if (match) {
                    return std::nullopt;
                }
                match = address;
            }
        }
        return match;
    }
    return std::nullopt;
}

bool GuestElfImage::patch_host_import(GuestArena& arena, std::string_view symbol,
                                      std::uintptr_t target, std::string& error) const {
    const std::string slot_name = "h2_host_slot_" + std::string(symbol);
    const auto address = symbol_address(slot_name);
    if (!address) {
        error = "guest ELF does not define host import slot " + slot_name;
        return false;
    }
    const auto segment = std::find_if(segments_.begin(), segments_.end(), [&](const auto& item) {
        const std::uint64_t end = static_cast<std::uint64_t>(*address) + sizeof(target);
        return item.protection == GuestMemoryProtection::read_write &&
               *address >= item.address && end <= static_cast<std::uint64_t>(item.address) + item.size;
    });
    if (segment == segments_.end()) {
        error = "host import slot is not inside writable guest data";
        return false;
    }
    std::memcpy(arena.pointer(*address), &target, sizeof(target));
    return true;
}

}  // namespace h2::port
