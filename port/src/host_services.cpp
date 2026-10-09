#include "h2/host_abi.h"
#include "h2/guest_elf_image.hpp"
#include "h2/host_services.hpp"
#include "h2/profile_store.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

struct DispatchState {
    std::mutex mutex;
    h2::port::VideoControlHandler video_handler = nullptr;
    void* video_context = nullptr;
    h2::port::RenderRequestHandler render_handler = nullptr;
    void* render_context = nullptr;
    h2::port::TextRequestHandler text_handler = nullptr;
    void* text_context = nullptr;
    std::atomic<std::uint64_t> video_calls{0};
    std::atomic<std::uint64_t> render_calls{0};
    std::atomic<std::uint64_t> text_calls{0};
};

DispatchState& dispatch_state() {
    static DispatchState state;
    return state;
}

constexpr std::uint32_t kCacheHeaderAddress = 0x00557c94u;
constexpr std::uint32_t kPhysicalRegionAddress = 0x80060000u;
constexpr std::uint32_t kPhysicalRegionSize = 0x02000000u;
constexpr std::uint32_t kPhysicalAllocationBase = 0x80061000u;
constexpr std::uint32_t kCacheHeaderSize = 0x800u;

struct CacheState {
    std::mutex mutex;
    h2::port::GuestArena* arena = nullptr;
    std::vector<unsigned char> bytes;
    std::filesystem::path map_directory;
    std::uint32_t next_address = kPhysicalAllocationBase;
};

CacheState& cache_state() {
    static CacheState state;
    return state;
}

struct ProfileState {
    std::mutex mutex;
    h2::port::NativeProfileStore store;
};

ProfileState& profile_state() {
    static ProfileState state;
    return state;
}

const Clock::time_point kClockStart = Clock::now();

template <typename Function>
std::uintptr_t function_address(Function function) {
    static_assert(sizeof(function) == sizeof(std::uintptr_t));
    std::uintptr_t address = 0;
    std::memcpy(&address, &function, sizeof(address));
    return address;
}

std::uint32_t read_u32(const std::array<unsigned char, kCacheHeaderSize>& data,
                       std::size_t offset) {
    return static_cast<std::uint32_t>(data[offset]) |
           (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(data[offset + 3]) << 24);
}

std::string fixed_string(const std::array<unsigned char, kCacheHeaderSize>& data,
                         std::size_t offset, std::size_t capacity) {
    std::size_t length = 0;
    while (length < capacity && data[offset + length]) {
        ++length;
    }
    return std::string(reinterpret_cast<const char*>(data.data() + offset), length);
}

bool locate_campaign_cache(const std::filesystem::path& map_directory,
                           const std::string& map_name,
                           std::filesystem::path& path) {
    path = map_directory / (map_name + ".map");
    std::error_code error;
    const auto status = std::filesystem::symlink_status(path, error);
    if (error || status.type() != std::filesystem::file_type::regular) {
        return false;
    }
    const auto file_size = std::filesystem::file_size(path, error);
    if (error || file_size < kCacheHeaderSize || file_size > 0x20800000u) {
        return false;
    }
    std::ifstream input(path, std::ios::binary);
    std::array<unsigned char, kCacheHeaderSize> header{};
    if (!input.read(reinterpret_cast<char*>(header.data()),
                    static_cast<std::streamsize>(header.size()))) {
        return false;
    }
    return read_u32(header, 0) == 0x68656164u && read_u32(header, 4) == 8u &&
           read_u32(header, 8) == file_size && read_u32(header, 0x140) == 0u &&
           read_u32(header, 0x7fc) == 0x666f6f74u &&
           fixed_string(header, 0x120, 0x20) == "02.09.27.09809" &&
           fixed_string(header, 0x198, 0x20) == map_name;
}

bool guest_campaign_cache_name(const CacheState& state, const char* map_name,
                               std::string& cache_name) {
    if (!map_name || !state.arena) {
        return false;
    }
    const std::uintptr_t start = reinterpret_cast<std::uintptr_t>(map_name);
    const std::uintptr_t guest_start = h2::port::kGuestBias;
    const std::uintptr_t guest_end = guest_start + h2::port::kGuestAddressSpaceSize;
    if (start < guest_start || start >= guest_end || guest_end - start < 0x100) {
        return false;
    }
    std::string name;
    for (std::size_t index = 0; index < 0x100; ++index) {
        const unsigned char value = static_cast<unsigned char>(map_name[index]);
        if (!value) {
            const auto separator = name.find_last_of('\\');
            cache_name = separator == std::string::npos
                ? name
                : name.substr(separator + 1);
            return !cache_name.empty();
        }
        const bool safe = (value >= 'a' && value <= 'z') ||
                          (value >= 'A' && value <= 'Z') ||
                          (value >= '0' && value <= '9') || value == '_' || value == '-' ||
                          value == '\\';
        if (!safe) {
            return false;
        }
        name.push_back(static_cast<char>(value));
    }
    return false;
}

}  // namespace

namespace h2::port {

void set_video_control_handler(VideoControlHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.video_handler = handler;
    state.video_context = context;
}

void set_render_request_handler(RenderRequestHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.render_handler = handler;
    state.render_context = context;
}

void set_text_request_handler(TextRequestHandler handler, void* context) {
    auto& state = dispatch_state();
    const std::lock_guard lock(state.mutex);
    state.text_handler = handler;
    state.text_context = context;
}

HostServiceStats host_service_stats() {
    auto& state = dispatch_state();
    return {state.video_calls.load(std::memory_order_relaxed),
            state.render_calls.load(std::memory_order_relaxed),
            state.text_calls.load(std::memory_order_relaxed)};
}

void reset_host_service_stats() {
    auto& state = dispatch_state();
    state.video_calls.store(0, std::memory_order_relaxed);
    state.render_calls.store(0, std::memory_order_relaxed);
    state.text_calls.store(0, std::memory_order_relaxed);
}

bool configure_cache_services(GuestArena& arena, const std::filesystem::path& map_path,
                              std::string& error) {
    std::ifstream input(map_path, std::ios::binary);
    if (!input) {
        error = "could not open cache map: " + map_path.string();
        return false;
    }
    std::vector<unsigned char> bytes(std::istreambuf_iterator<char>(input), {});
    if (input.bad() || bytes.size() < kCacheHeaderSize) {
        error = "could not read a complete cache header: " + map_path.string();
        return false;
    }
    if (!arena.protect(kCacheHeaderAddress, kCacheHeaderSize,
                       GuestMemoryProtection::read_write, error) ||
        !arena.protect(kPhysicalRegionAddress, kPhysicalRegionSize,
                       GuestMemoryProtection::read_write, error)) {
        return false;
    }
    std::memcpy(arena.pointer(kCacheHeaderAddress), bytes.data(), kCacheHeaderSize);
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    state.arena = &arena;
    state.bytes = std::move(bytes);
    state.map_directory = map_path.parent_path();
    state.next_address = kPhysicalAllocationBase;
    return true;
}

void reset_cache_services() {
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    state.arena = nullptr;
    state.bytes.clear();
    state.map_directory.clear();
    state.next_address = kPhysicalAllocationBase;
}

bool configure_profile_services(const std::filesystem::path& directory,
                                std::string& error) {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    if (!state.store.open(directory, error)) {
        return false;
    }
    std::vector<std::uint32_t> ids;
    if (!state.store.list(ids, error)) {
        state.store.close();
        return false;
    }
    return true;
}

void reset_profile_services() {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    state.store.close();
}

bool list_native_profiles(std::vector<std::uint32_t>& ids, std::string& error) {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    return state.store.list(ids, error);
}

bool create_native_profile(std::span<const std::byte> record, std::uint32_t& id,
                           std::string& error) {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    return state.store.create(record, id, error);
}

bool read_native_profile(std::uint32_t id, std::span<std::byte> record,
                         std::string& error) {
    if (record.size() != kNativeProfileRecordSize) {
        error = "native profile read buffer has the wrong size";
        return false;
    }
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    std::array<std::byte, kNativeProfileRecordSize> stored{};
    if (!state.store.read(id, stored, error)) {
        return false;
    }
    std::copy(stored.begin(), stored.end(), record.begin());
    return true;
}

bool write_native_profile(std::uint32_t id, std::span<const std::byte> record,
                          std::string& error) {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    return state.store.write(id, record, error);
}

bool erase_native_profile(std::uint32_t id, std::string& error) {
    auto& state = profile_state();
    const std::lock_guard lock(state.mutex);
    return state.store.erase(id, error);
}

bool copy_cache_bytes(std::uint32_t offset, void* destination, std::size_t size) {
    if (!destination) {
        return false;
    }
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    if (offset > state.bytes.size() || size > state.bytes.size() - offset) {
        return false;
    }
    std::memcpy(destination, state.bytes.data() + offset, size);
    return true;
}

bool patch_standard_host_imports(GuestElfImage& image, GuestArena& arena,
                                 std::string& error) {
    struct Import {
        const char* symbol;
        std::uintptr_t target;
    };
    const Import imports[] = {
        {"host_h2_physical_memory_malloc_low",
         function_address(&host_h2_physical_memory_malloc_low)},
        {"host_h2_cache_read", function_address(&host_h2_cache_read)},
        {"host_h2_map_preflight", function_address(&host_h2_map_preflight)},
        {"host_h2_map_select", function_address(&host_h2_map_select)},
        {"host_h2_video_control", function_address(&host_h2_video_control)},
        {"host_h2_render_request", function_address(&host_h2_render_request)},
        {"host_h2_text_request", function_address(&host_h2_text_request)},
        {"host_h2_get_milliseconds", function_address(&host_h2_get_milliseconds)},
        {"host_h2_sleep", function_address(&host_h2_sleep)},
        {"host_h2_cos", function_address(&host_h2_cos)},
        {"host_h2_profile_list", function_address(&host_h2_profile_list)},
        {"host_h2_profile_create", function_address(&host_h2_profile_create)},
        {"host_h2_profile_read", function_address(&host_h2_profile_read)},
        {"host_h2_profile_write", function_address(&host_h2_profile_write)},
        {"host_h2_profile_delete", function_address(&host_h2_profile_delete)},
    };
    for (const auto& import : imports) {
        if (!image.patch_host_import(arena, import.symbol, import.target, error)) {
            return false;
        }
    }
    return true;
}

}  // namespace h2::port

extern "C" unsigned int host_h2_abi_version() {
    return H2_HOST_ABI_VERSION;
}

extern "C" void host_h2_log(const char* message) {
    if (message) {
        std::cerr << "guest: " << message << '\n';
    }
}

extern "C" void* host_h2_physical_memory_malloc_low(int size, unsigned int) {
    if (size <= 0) {
        return nullptr;
    }
    const std::uint64_t aligned =
        (static_cast<std::uint64_t>(static_cast<unsigned int>(size)) + 0xfffu) &
        ~std::uint64_t{0xfff};
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    const std::uint64_t end = static_cast<std::uint64_t>(state.next_address) + aligned;
    const std::uint64_t limit =
        static_cast<std::uint64_t>(kPhysicalRegionAddress) + kPhysicalRegionSize;
    if (!state.arena || end > limit) {
        return nullptr;
    }
    const std::uint32_t result = state.next_address;
    state.next_address = static_cast<std::uint32_t>(end);
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(result));
}

extern "C" int host_h2_cache_read(unsigned int offset, int size, void* buffer) {
    if (size < 0 || !buffer) {
        return 0;
    }
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    const std::size_t count = static_cast<std::size_t>(size);
    if (!state.arena || offset > state.bytes.size() ||
        count > state.bytes.size() - offset) {
        return 0;
    }
    const std::uintptr_t start = reinterpret_cast<std::uintptr_t>(buffer);
    const std::uintptr_t guest_start = h2::port::kGuestBias;
    const std::uintptr_t guest_end = guest_start + h2::port::kGuestAddressSpaceSize;
    if (start < guest_start || start > guest_end || count > guest_end - start) {
        return 0;
    }
    std::memcpy(buffer, state.bytes.data() + offset, count);
    return 1;
}

extern "C" int host_h2_map_preflight(const char* map_name) {
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    std::string cache_name;
    std::filesystem::path path;
    return guest_campaign_cache_name(state, map_name, cache_name) &&
           locate_campaign_cache(state.map_directory, cache_name, path);
}

extern "C" int host_h2_map_select(const char* map_name) {
    auto& state = cache_state();
    const std::lock_guard lock(state.mutex);
    std::string cache_name;
    std::filesystem::path path;
    if (!guest_campaign_cache_name(state, map_name, cache_name) ||
        !locate_campaign_cache(state.map_directory, cache_name, path)) {
        return 0;
    }
    std::ifstream input(path, std::ios::binary);
    std::vector<unsigned char> bytes(std::istreambuf_iterator<char>(input), {});
    if (input.bad() || bytes.size() < kCacheHeaderSize) {
        return 0;
    }
    const std::uint32_t declared_size = static_cast<std::uint32_t>(bytes[8]) |
        (static_cast<std::uint32_t>(bytes[9]) << 8) |
        (static_cast<std::uint32_t>(bytes[10]) << 16) |
        (static_cast<std::uint32_t>(bytes[11]) << 24);
    if (bytes.size() != declared_size) {
        return 0;
    }
    std::memcpy(state.arena->pointer(kCacheHeaderAddress), bytes.data(), kCacheHeaderSize);
    state.bytes = std::move(bytes);
    return 1;
}

extern "C" void host_h2_video_control(int command) {
    auto& state = dispatch_state();
    state.video_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::VideoControlHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.video_handler;
        context = state.video_context;
    }
    if (handler) {
        handler(command, context);
    }
}

extern "C" void host_h2_render_request(const void* request, const void* vertices) {
    auto& state = dispatch_state();
    state.render_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::RenderRequestHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.render_handler;
        context = state.render_context;
    }
    if (handler) {
        handler(request, vertices, context);
    }
}

extern "C" void host_h2_text_request(const unsigned int* text, const void* bounds,
                                       const void* clip, const void* position,
                                       int line_gap, float scale, unsigned int color,
                                       const void* shadow) {
    auto& state = dispatch_state();
    state.text_calls.fetch_add(1, std::memory_order_relaxed);
    h2::port::TextRequestHandler handler;
    void* context;
    {
        const std::lock_guard lock(state.mutex);
        handler = state.text_handler;
        context = state.text_context;
    }
    if (handler) {
        handler(text, bounds, clip, position, line_gap, scale, color, shadow, context);
    }
}

extern "C" unsigned int host_h2_get_milliseconds() {
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - kClockStart);
    return static_cast<unsigned int>(elapsed.count());
}

extern "C" void host_h2_sleep(unsigned int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

extern "C" double host_h2_cos(double value) {
    return std::cos(value);
}

extern "C" int host_h2_profile_list(unsigned int* identifiers,
                                      unsigned int capacity) {
    if ((!identifiers && capacity) || capacity > 4096) {
        return -1;
    }
    std::vector<std::uint32_t> ids;
    std::string error;
    if (!h2::port::list_native_profiles(ids, error)) {
        return -1;
    }
    const std::size_t copied = std::min<std::size_t>(ids.size(), capacity);
    if (copied) {
        std::copy_n(ids.begin(), copied, identifiers);
    }
    return static_cast<int>(ids.size());
}

extern "C" int host_h2_profile_create(const void* record, unsigned int size) {
    if (!record || size != h2::port::kNativeProfileRecordSize) {
        return -1;
    }
    std::uint32_t id = 0;
    std::string error;
    const auto bytes = std::span(
        static_cast<const std::byte*>(record), h2::port::kNativeProfileRecordSize);
    return h2::port::create_native_profile(bytes, id, error) ?
        static_cast<int>(id) : -1;
}

extern "C" int host_h2_profile_read(unsigned int identifier, void* record,
                                      unsigned int size) {
    if (!record || size != h2::port::kNativeProfileRecordSize) {
        return 0;
    }
    std::string error;
    const auto bytes = std::span(
        static_cast<std::byte*>(record), h2::port::kNativeProfileRecordSize);
    return h2::port::read_native_profile(identifier, bytes, error) ? 1 : 0;
}

extern "C" int host_h2_profile_write(unsigned int identifier,
                                       const void* record, unsigned int size) {
    if (!record || size != h2::port::kNativeProfileRecordSize) {
        return 0;
    }
    std::string error;
    const auto bytes = std::span(
        static_cast<const std::byte*>(record), h2::port::kNativeProfileRecordSize);
    return h2::port::write_native_profile(identifier, bytes, error) ? 1 : 0;
}

extern "C" int host_h2_profile_delete(unsigned int identifier) {
    std::string error;
    return h2::port::erase_native_profile(identifier, error) ? 1 : 0;
}
