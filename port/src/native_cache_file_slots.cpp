#include "h2/native_cache_file_slots.hpp"

#include "h2/native_async_lifecycle.hpp"

#include <algorithm>
#include <mutex>

namespace h2::port {
namespace {

constexpr std::uint32_t kCacheMagic = 0x68656164u;
constexpr std::uint32_t kCacheVersion = 8u;
constexpr std::uint32_t kCacheFooter = 0x666f6f74u;
constexpr const char* kRetailBuild = "02.09.27.09809";

std::uint32_t read_u32(const std::array<std::uint8_t, 0x800>& header,
                       std::size_t offset) {
    return static_cast<std::uint32_t>(header[offset]) |
           (static_cast<std::uint32_t>(header[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(header[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(header[offset + 3]) << 24);
}

std::string fixed_string(const std::array<std::uint8_t, 0x800>& header,
                         std::size_t offset, std::size_t capacity) {
    std::size_t length = 0;
    while (length < capacity && header[offset + length])
        ++length;
    return std::string(reinterpret_cast<const char*>(header.data() + offset), length);
}

std::uint32_t maximum_size(std::int16_t type) {
    switch (type) {
    case 0:
        return 0x11800000u;
    case 1:
    case 2:
        return 0x05000000u;
    case 3:
        return 0x0b400000u;
    case 4:
        return 0x20800000u;
    default:
        return 0;
    }
}

} // namespace

struct NativeCacheFileSlots::Data {
    Data(NativeAsyncPlatform& platform_value, NativeAsyncLifecycle& lifecycle_value)
        : platform(platform_value), lifecycle(lifecycle_value) {}

    struct Slot {
        NativeAsyncPlatform::Handle handle = NativeAsyncPlatform::invalid_handle;
        std::array<std::uint8_t, header_size> header{};
    };

    NativeAsyncPlatform& platform;
    NativeAsyncLifecycle& lifecycle;
    mutable std::mutex mutex;
    std::array<Slot, slot_count> slots{};
    std::int32_t active = no_active_slot;
};

NativeCacheFileSlots::NativeCacheFileSlots(NativeAsyncPlatform& platform,
                                           NativeAsyncLifecycle& lifecycle)
    : data_(std::make_unique<Data>(platform, lifecycle)) {}

NativeCacheFileSlots::~NativeCacheFileSlots() {
    for (auto& slot : data_->slots) {
        if (slot.handle != NativeAsyncPlatform::invalid_handle)
            data_->platform.close(slot.handle);
    }
}

bool NativeCacheFileSlots::install(const std::filesystem::path& path,
                                   std::int16_t expected_type,
                                   const std::string& expected_name,
                                   bool make_active, std::string& error) {
    error.clear();
    if (data_->lifecycle.state() != NativeAsyncLifecycle::State::stopped) {
        error = "cache files may change only while the async scheduler is stopped";
        return false;
    }
    const std::uint32_t type_limit = maximum_size(expected_type);
    if (!type_limit || expected_name.empty()) {
        error = "cache type or identity is invalid";
        return false;
    }
    if (expected_type == 0 && !make_active) {
        error = "type-0 cache slots are reserved for active map replacement";
        return false;
    }

    const auto handle = data_->platform.open_read_only(path);
    if (handle == NativeAsyncPlatform::invalid_handle) {
        error = "could not open validated cache file";
        return false;
    }
    auto close_new = [&] { data_->platform.close(handle); };

    std::uint64_t actual_size = 0;
    std::array<std::uint8_t, header_size> header{};
    std::uint32_t bytes_read = 0;
    if (!data_->platform.file_size(handle, &actual_size) ||
        actual_size < header_size || actual_size > type_limit ||
        !data_->platform.read_file(handle, header.data(), header.size(), &bytes_read) ||
        bytes_read != header.size()) {
        close_new();
        error = "cache file size or header read is invalid";
        return false;
    }
    const auto header_type = static_cast<std::int16_t>(read_u32(header, 0x140));
    if (read_u32(header, 0) != kCacheMagic ||
        read_u32(header, 4) != kCacheVersion ||
        read_u32(header, 8) != actual_size || header_type != expected_type ||
        read_u32(header, 0x7fc) != kCacheFooter ||
        fixed_string(header, 0x120, 0x20) != kRetailBuild ||
        fixed_string(header, 0x198, 0x20) != expected_name) {
        close_new();
        error = "cache header identity does not match the requested slot";
        return false;
    }

    std::lock_guard lock(data_->mutex);
    std::size_t target = 0;
    switch (expected_type) {
    case 0:
        target = data_->active == 4 ? 5 : 4;
        break;
    case 1:
        target = 3;
        break;
    case 2:
        target = 2;
        break;
    case 3:
        target = 0;
        break;
    case 4:
        target = 1;
        break;
    default:
        close_new();
        error = "cache type has no recovered slot";
        return false;
    }

    const auto replaced = data_->slots[target].handle;
    data_->slots[target].handle = handle;
    data_->slots[target].header = header;
    if (replaced != NativeAsyncPlatform::invalid_handle)
        data_->platform.close(replaced);

    if (make_active) {
        const std::int32_t old_active = data_->active;
        data_->active = static_cast<std::int32_t>(target);
        if (old_active >= 3 && old_active != data_->active) {
            auto& old = data_->slots[static_cast<std::size_t>(old_active)];
            if (old.handle != NativeAsyncPlatform::invalid_handle)
                data_->platform.close(old.handle);
            old = {};
        }
    }
    return true;
}

bool NativeCacheFileSlots::clear(std::string& error) {
    error.clear();
    if (data_->lifecycle.state() != NativeAsyncLifecycle::State::stopped) {
        error = "cache files may clear only while the async scheduler is stopped";
        return false;
    }
    const std::lock_guard lock(data_->mutex);
    bool result = true;
    for (auto& slot : data_->slots) {
        if (slot.handle != NativeAsyncPlatform::invalid_handle)
            result = data_->platform.close(slot.handle) && result;
        slot = {};
    }
    data_->active = no_active_slot;
    if (!result)
        error = "one or more cache handles could not close";
    return result;
}

std::int32_t NativeCacheFileSlots::active_slot() const {
    const std::lock_guard lock(data_->mutex);
    return data_->active;
}

NativeAsyncPlatform::Handle NativeCacheFileSlots::slot_handle(std::size_t index) const {
    const std::lock_guard lock(data_->mutex);
    return index < data_->slots.size() ? data_->slots[index].handle
                                      : NativeAsyncPlatform::invalid_handle;
}

std::array<std::uint8_t, NativeCacheFileSlots::header_size>
NativeCacheFileSlots::slot_header(std::size_t index) const {
    const std::lock_guard lock(data_->mutex);
    return index < data_->slots.size() ? data_->slots[index].header
                                      : std::array<std::uint8_t, header_size>{};
}

} // namespace h2::port
