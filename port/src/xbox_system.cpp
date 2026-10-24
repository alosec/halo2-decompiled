#include "h2/xbox_system.hpp"

#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

#include <unistd.h>

namespace {

using h2::port::GuestArena;
using h2::port::GuestMemoryProtection;

/* Guest regions owned by these services; the curated image's fixed regions
   (0x20000000 stacks, 0x30000000 virtual, 0x80060000 physical) stay clear. */
constexpr std::uint32_t kVirtualBase = 0x40000000u;
constexpr std::uint32_t kVirtualEnd = 0x7f000000u;
constexpr std::uint32_t kPhysicalBase = 0x88000000u;
/* The game masks texture data addresses with 0x0fffffff and rebuilds them with
   0x80000000, so contiguous memory must stay below 0x90000000. */
constexpr std::uint32_t kPhysicalEnd = 0x90000000u;
constexpr std::uint32_t kInvalidHandle = 0xffffffffu;
constexpr std::uint32_t kFileHandleBase = 0x48000000u;
constexpr std::uint32_t kFindHandleBase = 0x49000000u;

/* Win32 error codes the recovered code tests for. */
constexpr std::uint32_t kErrorFileNotFound = 2;
constexpr std::uint32_t kErrorPathNotFound = 3;
constexpr std::uint32_t kErrorAccessDenied = 5;
constexpr std::uint32_t kErrorNoMoreFiles = 18;
constexpr std::uint32_t kErrorFileExists = 80;
constexpr std::uint32_t kErrorAlreadyExists = 183;

constexpr std::uint32_t kFileAttributeReadOnly = 0x01;
constexpr std::uint32_t kFileAttributeDirectory = 0x10;
constexpr std::uint32_t kFileAttributeNormal = 0x80;

/* 100 ns intervals between 1601-01-01 and 1970-01-01. */
constexpr std::uint64_t kFileTimeUnixEpoch = 116444736000000000ull;

/* A first-fit page allocator over one guest region. */
class RegionAllocator {
public:
    RegionAllocator(std::uint32_t base, std::uint32_t end) : base_(base), end_(end) {
        free_[base] = end - base;
    }

    std::uint32_t allocate(GuestArena& arena, std::uint32_t size, std::uint32_t alignment,
                           bool top_down) {
        const std::uint32_t page = static_cast<std::uint32_t>(arena.page_size());
        alignment = std::max(alignment, page);
        const std::uint64_t length = (static_cast<std::uint64_t>(size) + page - 1) & ~(std::uint64_t{page} - 1);
        if (!length || length > end_ - base_)
            return 0;
        std::optional<std::uint32_t> chosen;
        if (top_down) {
            for (auto it = free_.rbegin(); it != free_.rend(); ++it) {
                const std::uint64_t block_end = std::uint64_t{it->first} + it->second;
                if (block_end < length)
                    continue;
                const std::uint64_t start = (block_end - length) & ~(std::uint64_t{alignment} - 1);
                if (start >= it->first) {
                    chosen = static_cast<std::uint32_t>(start);
                    break;
                }
            }
        } else {
            for (const auto& [block, block_size] : free_) {
                const std::uint64_t start = (std::uint64_t{block} + alignment - 1) & ~(std::uint64_t{alignment} - 1);
                if (start + length <= std::uint64_t{block} + block_size) {
                    chosen = static_cast<std::uint32_t>(start);
                    break;
                }
            }
        }
        if (!chosen)
            return 0;
        carve(*chosen, static_cast<std::uint32_t>(length));
        std::string error;
        if (!arena.protect(*chosen, static_cast<std::uint32_t>(length),
                           GuestMemoryProtection::read_write, error)) {
            release(arena, *chosen);
            return 0;
        }
        std::memset(arena.pointer(*chosen), 0, length);
        used_[*chosen] = static_cast<std::uint32_t>(length);
        return *chosen;
    }

    bool release(GuestArena& arena, std::uint32_t address) {
        const auto found = used_.find(address);
        if (found == used_.end())
            return false;
        const std::uint32_t length = found->second;
        used_.erase(found);
        std::string error;
        arena.protect(address, length, GuestMemoryProtection::none, error);
        auto next = free_.emplace(address, length).first;
        if (next != free_.begin()) {
            auto previous = std::prev(next);
            if (previous->first + previous->second == next->first) {
                previous->second += next->second;
                free_.erase(next);
                next = previous;
            }
        }
        auto after = std::next(next);
        if (after != free_.end() && next->first + next->second == after->first) {
            next->second += after->second;
            free_.erase(after);
        }
        return true;
    }

    std::uint32_t size_of(std::uint32_t address) const {
        const auto found = used_.upper_bound(address);
        if (found == used_.begin())
            return 0;
        const auto block = std::prev(found);
        return address < block->first + block->second ? block->second - (address - block->first) : 0;
    }

    bool owns(std::uint32_t address) const { return address >= base_ && address < end_; }

private:
    void carve(std::uint32_t start, std::uint32_t length) {
        auto it = std::prev(free_.upper_bound(start));
        const std::uint32_t block = it->first;
        const std::uint32_t block_size = it->second;
        free_.erase(it);
        if (start > block)
            free_[block] = start - block;
        const std::uint64_t block_end = std::uint64_t{block} + block_size;
        if (start + std::uint64_t{length} < block_end)
            free_[start + length] = static_cast<std::uint32_t>(block_end - start - length);
    }

    std::uint32_t base_;
    std::uint32_t end_;
    std::map<std::uint32_t, std::uint32_t> free_;
    std::map<std::uint32_t, std::uint32_t> used_;
};

struct OpenFile {
    std::FILE* stream = nullptr;
    bool writable = false;
};

struct FindState {
    std::vector<std::filesystem::directory_entry> entries;
    std::size_t next = 0;
};

struct SystemState {
    std::mutex mutex;
    GuestArena* arena = nullptr;
    std::filesystem::path data_root;
    std::filesystem::path user_root;
    std::unique_ptr<RegionAllocator> virtual_memory;
    std::unique_ptr<RegionAllocator> physical_memory;
    std::unordered_map<std::uint32_t, OpenFile> files;
    std::unordered_map<std::uint32_t, FindState> finds;
    std::uint32_t next_file = kFileHandleBase;
    std::uint32_t next_find = kFindHandleBase;
    std::uint32_t last_error = 0;
};

SystemState& system_state() {
    static SystemState state;
    return state;
}

bool trace_files() {
    static const bool enabled = std::getenv("H2_TRACE_FILES") != nullptr;
    return enabled;
}

std::string lower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

/* "d:\maps\mainmenu.map" -> <data root>/maps/mainmenu.map; other drives live
   in their own directory of the private user root. Returns nothing for paths
   the title cannot name. */
std::optional<std::filesystem::path> host_path(const char* guest, bool* read_only) {
    if (!guest)
        return std::nullopt;
    std::string text(guest);
    for (char& c : text)
        if (c == '\\')
            c = '/';
    if (text.size() < 2 || text[1] != ':')
        return std::nullopt;
    const char drive = static_cast<char>(std::tolower(static_cast<unsigned char>(text[0])));
    std::string rest = text.substr(2);
    while (!rest.empty() && rest.front() == '/')
        rest.erase(rest.begin());
    auto& state = system_state();
    std::filesystem::path root;
    if (drive == 'd') {
        root = state.data_root;
        if (read_only)
            *read_only = true;
    } else {
        root = state.user_root / std::string(1, drive);
        if (read_only)
            *read_only = false;
    }
    std::filesystem::path result = root;
    std::filesystem::path relative(rest);
    for (const auto& part : relative) {
        if (part == "..")
            return std::nullopt;
        /* Xbox names are case-insensitive: reuse an existing entry's spelling. */
        std::filesystem::path candidate = result / part;
        std::error_code ignored;
        if (!std::filesystem::exists(candidate, ignored) && std::filesystem::is_directory(result, ignored)) {
            const std::string wanted = lower(part.string());
            for (const auto& entry : std::filesystem::directory_iterator(result, ignored)) {
                if (lower(entry.path().filename().string()) == wanted) {
                    candidate = entry.path();
                    break;
                }
            }
        }
        result = candidate;
    }
    return result;
}

std::uint64_t file_time(std::filesystem::file_time_type time) {
    /* libc++ has no clock_cast: carry the offset between the two clocks over */
    const auto system = std::chrono::system_clock::now() +
                        std::chrono::duration_cast<std::chrono::system_clock::duration>(
                            time - std::filesystem::file_time_type::clock::now());
    const auto since = std::chrono::duration_cast<std::chrono::nanoseconds>(system.time_since_epoch());
    return kFileTimeUnixEpoch + static_cast<std::uint64_t>(since.count() / 100);
}

/* WIN32_FIND_DATA as the Xbox lays it out (narrow names, 0x13c bytes). */
struct GuestFindData {
    std::uint32_t attributes;
    std::uint64_t creation_time;
    std::uint64_t access_time;
    std::uint64_t write_time;
    std::uint32_t size_high;
    std::uint32_t size_low;
    std::uint32_t reserved0;
    std::uint32_t reserved1;
    char name[260];
    char alternate_name[14];
} __attribute__((packed));

void fill_find_data(const std::filesystem::directory_entry& entry, GuestFindData* out) {
    std::memset(out, 0, sizeof(*out));
    std::error_code ignored;
    const bool directory = entry.is_directory(ignored);
    out->attributes = directory ? kFileAttributeDirectory : kFileAttributeNormal;
    const std::uint64_t size = directory ? 0 : entry.file_size(ignored);
    out->size_high = static_cast<std::uint32_t>(size >> 32);
    out->size_low = static_cast<std::uint32_t>(size);
    const std::uint64_t time = file_time(entry.last_write_time(ignored));
    out->creation_time = out->access_time = out->write_time = time;
    std::snprintf(out->name, sizeof(out->name), "%s", entry.path().filename().string().c_str());
}

bool wildcard_match(const std::string& pattern, const std::string& name) {
    std::size_t p = 0, n = 0, star = std::string::npos, mark = 0;
    while (n < name.size()) {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == name[n])) {
            ++p;
            ++n;
        } else if (p < pattern.size() && pattern[p] == '*') {
            star = p++;
            mark = n;
        } else if (star != std::string::npos) {
            p = star + 1;
            n = ++mark;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        ++p;
    return p == pattern.size();
}

template <typename T>
std::uintptr_t function_address(T* function) {
    return reinterpret_cast<std::uintptr_t>(function);
}

} // namespace

extern "C" {

unsigned int host_h2_xbox_memory_alloc(unsigned int size, unsigned int alignment,
                                       unsigned int physical, unsigned int top_down) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    if (!state.arena)
        return 0;
    auto& region = physical ? *state.physical_memory : *state.virtual_memory;
    const std::uint32_t address = region.allocate(*state.arena, size, alignment, top_down != 0);
    if (!address)
        std::fprintf(stderr, "h2-port: %s allocation of 0x%x bytes failed\n",
                     physical ? "physical" : "virtual", size);
    return address;
}

int host_h2_xbox_memory_free(unsigned int address) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    if (!state.arena)
        return 0;
    if (state.virtual_memory->owns(address))
        return state.virtual_memory->release(*state.arena, address);
    if (state.physical_memory->owns(address))
        return state.physical_memory->release(*state.arena, address);
    return 0;
}

unsigned int host_h2_xbox_memory_size(unsigned int address) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    if (state.virtual_memory && state.virtual_memory->owns(address))
        return state.virtual_memory->size_of(address);
    if (state.physical_memory && state.physical_memory->owns(address))
        return state.physical_memory->size_of(address);
    return 0;
}

unsigned int host_h2_last_error(void) {
    return system_state().last_error;
}

void host_h2_set_last_error(unsigned int error) {
    system_state().last_error = error;
}

/* access: bit 0 read, bit 1 write. disposition: Win32 CREATE_NEW 1,
   CREATE_ALWAYS 2, OPEN_EXISTING 3, OPEN_ALWAYS 4, TRUNCATE_EXISTING 5. */
unsigned int host_h2_file_open(const char* path, unsigned int access, unsigned int disposition) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    bool read_only = false;
    const auto resolved = host_path(path, &read_only);
    if (!resolved) {
        state.last_error = kErrorPathNotFound;
        return kInvalidHandle;
    }
    const bool want_write = access & 2;
    std::error_code error;
    const bool exists = std::filesystem::is_regular_file(*resolved, error);
    if (want_write && read_only) {
        state.last_error = kErrorAccessDenied;
        return kInvalidHandle;
    }
    if ((disposition == 3 || disposition == 5) && !exists) {
        state.last_error = kErrorFileNotFound;
        if (trace_files())
            std::fprintf(stderr, "h2-file: open %s -> not found\n", path);
        return kInvalidHandle;
    }
    if (disposition == 1 && exists) {
        state.last_error = kErrorFileExists;
        return kInvalidHandle;
    }
    if (!exists && !std::filesystem::is_directory(resolved->parent_path(), error)) {
        state.last_error = kErrorPathNotFound;
        return kInvalidHandle;
    }
    const bool truncate = disposition == 2 || disposition == 5;
    const char* mode = !want_write ? "rb" : (truncate || !exists ? "w+b" : "r+b");
    std::FILE* stream = std::fopen(resolved->c_str(), mode);
    if (!stream) {
        state.last_error = kErrorAccessDenied;
        return kInvalidHandle;
    }
    const std::uint32_t handle = state.next_file++;
    state.files[handle] = {stream, want_write};
    state.last_error = exists && (disposition == 2 || disposition == 4) ? kErrorAlreadyExists : 0;
    if (trace_files())
        std::fprintf(stderr, "h2-file: open %s -> %s (0x%x)\n", path, resolved->c_str(), handle);
    return handle;
}

int host_h2_file_is_handle(unsigned int handle) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    return state.files.count(handle) ? 1 : 0;
}

int host_h2_file_read(unsigned int handle, void* buffer, unsigned int size, unsigned int* read) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found == state.files.end())
        return 0;
    const std::size_t count = size ? std::fread(buffer, 1, size, found->second.stream) : 0;
    if (read)
        *read = static_cast<unsigned int>(count);
    if (trace_files() && count != size)
        std::fprintf(stderr, "h2-file: read 0x%x of 0x%x from 0x%x\n", static_cast<unsigned>(count), size, handle);
    return !std::ferror(found->second.stream);
}

int host_h2_file_write(unsigned int handle, const void* buffer, unsigned int size,
                       unsigned int* written) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found == state.files.end() || !found->second.writable) {
        if (trace_files())
            std::fprintf(stderr, "h2-file: write to 0x%x refused\n", handle);
        return 0;
    }
    const std::size_t count = size ? std::fwrite(buffer, 1, size, found->second.stream) : 0;
    if (written)
        *written = static_cast<unsigned int>(count);
    return count == size;
}

/* method: FILE_BEGIN 0, FILE_CURRENT 1, FILE_END 2 */
int host_h2_file_seek(unsigned int handle, long long distance, unsigned int method,
                      unsigned long long* position) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found == state.files.end() || method > 2)
        return 0;
    const int whence = method == 0 ? SEEK_SET : (method == 1 ? SEEK_CUR : SEEK_END);
    if (fseeko(found->second.stream, distance, whence) != 0) {
        if (trace_files())
            std::fprintf(stderr, "h2-file: seek 0x%x to %lld (%u) failed\n", handle, distance, method);
        return 0;
    }
    if (position)
        *position = static_cast<unsigned long long>(ftello(found->second.stream));
    return 1;
}

int host_h2_file_size(unsigned int handle, unsigned long long* size) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found == state.files.end())
        return 0;
    const off_t current = ftello(found->second.stream);
    fseeko(found->second.stream, 0, SEEK_END);
    const off_t end = ftello(found->second.stream);
    fseeko(found->second.stream, current, SEEK_SET);
    if (size)
        *size = static_cast<unsigned long long>(end);
    return 1;
}

int host_h2_file_truncate(unsigned int handle) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found == state.files.end() || !found->second.writable)
        return 0;
    std::fflush(found->second.stream);
    return ftruncate(fileno(found->second.stream), ftello(found->second.stream)) == 0;
}

int host_h2_file_flush(unsigned int handle) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    return found != state.files.end() && std::fflush(found->second.stream) == 0;
}

int host_h2_file_close(unsigned int handle) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.files.find(handle);
    if (found != state.files.end()) {
        std::fclose(found->second.stream);
        state.files.erase(found);
        return 1;
    }
    return state.finds.erase(handle) ? 1 : 0;
}

/* Returns the Win32 attributes, or 0xffffffff when the path does not exist. */
unsigned int host_h2_file_attributes(const char* path, unsigned long long* size,
                                     unsigned long long* write_time) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    bool read_only = false;
    const auto resolved = host_path(path, &read_only);
    std::error_code error;
    if (!resolved || !std::filesystem::exists(*resolved, error)) {
        state.last_error = kErrorFileNotFound;
        return kInvalidHandle;
    }
    const bool directory = std::filesystem::is_directory(*resolved, error);
    if (size)
        *size = directory ? 0 : std::filesystem::file_size(*resolved, error);
    if (write_time)
        *write_time = file_time(std::filesystem::last_write_time(*resolved, error));
    unsigned int attributes = directory ? kFileAttributeDirectory : kFileAttributeNormal;
    if (read_only && !directory)
        attributes |= kFileAttributeReadOnly;
    return attributes;
}

int host_h2_file_delete(const char* path) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    bool read_only = false;
    const auto resolved = host_path(path, &read_only);
    std::error_code error;
    if (!resolved || read_only || !std::filesystem::is_regular_file(*resolved, error)) {
        state.last_error = kErrorFileNotFound;
        return 0;
    }
    return std::filesystem::remove(*resolved, error);
}

int host_h2_directory_create(const char* path) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    bool read_only = false;
    const auto resolved = host_path(path, &read_only);
    std::error_code error;
    if (!resolved || read_only) {
        state.last_error = kErrorAccessDenied;
        return 0;
    }
    if (std::filesystem::exists(*resolved, error)) {
        state.last_error = kErrorAlreadyExists;
        return 0;
    }
    return std::filesystem::create_directories(*resolved, error);
}

int host_h2_directory_remove(const char* path) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    bool read_only = false;
    const auto resolved = host_path(path, &read_only);
    std::error_code error;
    if (!resolved || read_only)
        return 0;
    return std::filesystem::remove_all(*resolved, error) > 0;
}

unsigned int host_h2_find_first(const char* pattern, void* data) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    if (!pattern) {
        state.last_error = kErrorPathNotFound;
        return kInvalidHandle;
    }
    std::string text(pattern);
    std::replace(text.begin(), text.end(), '\\', '/');
    const auto slash = text.rfind('/');
    const std::string directory = slash == std::string::npos ? text : text.substr(0, slash);
    const std::string wildcard = lower(slash == std::string::npos ? "*" : text.substr(slash + 1));
    const auto resolved = host_path(directory.c_str(), nullptr);
    std::error_code error;
    if (!resolved || !std::filesystem::is_directory(*resolved, error)) {
        state.last_error = kErrorPathNotFound;
        return kInvalidHandle;
    }
    FindState find;
    for (const auto& entry : std::filesystem::directory_iterator(*resolved, error))
        if (wildcard_match(wildcard, lower(entry.path().filename().string())))
            find.entries.push_back(entry);
    std::sort(find.entries.begin(), find.entries.end(),
              [](const auto& a, const auto& b) { return a.path().filename() < b.path().filename(); });
    if (find.entries.empty()) {
        state.last_error = kErrorFileNotFound;
        return kInvalidHandle;
    }
    fill_find_data(find.entries[0], static_cast<GuestFindData*>(data));
    find.next = 1;
    const std::uint32_t handle = state.next_find++;
    state.finds[handle] = std::move(find);
    return handle;
}

int host_h2_find_next(unsigned int handle, void* data) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    const auto found = state.finds.find(handle);
    if (found == state.finds.end() || found->second.next >= found->second.entries.size()) {
        state.last_error = kErrorNoMoreFiles;
        return 0;
    }
    fill_find_data(found->second.entries[found->second.next++], static_cast<GuestFindData*>(data));
    return 1;
}

/* FILETIME now, in 100 ns intervals since 1601 (UTC). */
unsigned long long host_h2_system_time(void) {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return kFileTimeUnixEpoch +
           static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() / 100);
}

/* Seconds east of UTC for the host's local time zone. */
int host_h2_local_time_offset(void) {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
    localtime_r(&now, &local);
    return static_cast<int>(local.tm_gmtoff);
}

unsigned long long host_h2_performance_counter(void) {
    return static_cast<unsigned long long>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

double host_h2_math_atan(double value) { return std::atan(value); }
double host_h2_math_atan2(double y, double x) { return std::atan2(y, x); }
double host_h2_math_asin(double value) { return std::asin(value); }

} // extern "C"

namespace h2::port {

bool configure_xbox_system(GuestArena& arena, const std::filesystem::path& data_root,
                           const std::filesystem::path& user_root, std::string& error) {
    auto& state = system_state();
    const std::lock_guard lock(state.mutex);
    state.arena = &arena;
    state.data_root = data_root;
    state.user_root = user_root;
    state.virtual_memory = std::make_unique<RegionAllocator>(kVirtualBase, kVirtualEnd);
    state.physical_memory = std::make_unique<RegionAllocator>(kPhysicalBase, kPhysicalEnd);
    std::error_code fs_error;
    for (const char drive : {'t', 'u', 'z', 'x', 'y', 'w'})
        std::filesystem::create_directories(user_root / std::string(1, drive), fs_error);
    if (fs_error) {
        error = "could not create the native title drives under " + user_root.string();
        return false;
    }
    return true;
}

bool patch_xbox_system_imports(GuestElfImage& image, GuestArena& arena, std::string& error) {
    struct Import {
        const char* symbol;
        std::uintptr_t target;
    };
    const Import imports[] = {
        {"host_h2_xbox_memory_alloc", function_address(&host_h2_xbox_memory_alloc)},
        {"host_h2_xbox_memory_free", function_address(&host_h2_xbox_memory_free)},
        {"host_h2_xbox_memory_size", function_address(&host_h2_xbox_memory_size)},
        {"host_h2_last_error", function_address(&host_h2_last_error)},
        {"host_h2_set_last_error", function_address(&host_h2_set_last_error)},
        {"host_h2_file_open", function_address(&host_h2_file_open)},
        {"host_h2_file_is_handle", function_address(&host_h2_file_is_handle)},
        {"host_h2_file_read", function_address(&host_h2_file_read)},
        {"host_h2_file_write", function_address(&host_h2_file_write)},
        {"host_h2_file_seek", function_address(&host_h2_file_seek)},
        {"host_h2_file_size", function_address(&host_h2_file_size)},
        {"host_h2_file_truncate", function_address(&host_h2_file_truncate)},
        {"host_h2_file_flush", function_address(&host_h2_file_flush)},
        {"host_h2_file_close", function_address(&host_h2_file_close)},
        {"host_h2_file_attributes", function_address(&host_h2_file_attributes)},
        {"host_h2_file_delete", function_address(&host_h2_file_delete)},
        {"host_h2_directory_create", function_address(&host_h2_directory_create)},
        {"host_h2_directory_remove", function_address(&host_h2_directory_remove)},
        {"host_h2_find_first", function_address(&host_h2_find_first)},
        {"host_h2_find_next", function_address(&host_h2_find_next)},
        {"host_h2_system_time", function_address(&host_h2_system_time)},
        {"host_h2_local_time_offset", function_address(&host_h2_local_time_offset)},
        {"host_h2_performance_counter", function_address(&host_h2_performance_counter)},
        {"host_h2_math_atan", function_address(&host_h2_math_atan)},
        {"host_h2_math_atan2", function_address(&host_h2_math_atan2)},
        {"host_h2_math_asin", function_address(&host_h2_math_asin)},
    };
    for (const auto& import : imports) {
        if (image.symbol_address(std::string("h2_host_slot_") + import.symbol) &&
            !image.patch_host_import(arena, import.symbol, import.target, error))
            return false;
    }
    return true;
}

} // namespace h2::port
