#include "h2/native_texture_pool_storage.hpp"

#include "h2/guest_arena.hpp"
#include "h2/native_async_lifecycle.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>

namespace {

constexpr std::uint32_t kDataArraySignature = 0x64407440u;
constexpr std::uint32_t kStorageAddress = 0x20400000u;
constexpr std::uint32_t kStorageSize = 0x00010000u;
constexpr std::uint32_t kGlobalsAddress = 0x004e6000u;
constexpr std::uint32_t kGlobalsSize = 0x00001000u;

#pragma pack(push, 1)
struct GuestRecordPool {
    char name[0x20];
    std::int32_t maximum_count;
    std::int32_t size;
    std::uint8_t alignment_bits;
    std::uint8_t valid;
    std::uint16_t flags;
    std::uint32_t signature;
    std::uint32_t allocator;
    std::int32_t first_free_index;
    std::int32_t high_water_index;
    std::int32_t actual_count;
    std::int16_t next_salt;
    std::uint16_t unknown42;
    std::uint32_t data;
    std::uint32_t bitmap;
};
static_assert(sizeof(GuestRecordPool) == 0x4c);

struct GuestPhysicalObject {
    char name[0x20];
    std::uint32_t delete_proc;
    std::uint32_t busy_proc;
    std::uint32_t state_proc;
    std::int32_t state;
    std::int32_t page_count;
    std::int32_t page_shift;
    std::int32_t time;
    std::int32_t first;
    std::int32_t last;
    std::int32_t limits[8];
    std::uint32_t blocks;
    std::uint32_t signature;
    std::uint32_t allocator;
};
static_assert(sizeof(GuestPhysicalObject) == 0x70);
#pragma pack(pop)

void copy_name(char (&destination)[0x20], const char* source) {
    std::size_t index = 0;
    while (index + 1 < sizeof(destination) && source[index]) {
        destination[index] = source[index];
        ++index;
    }
}

GuestRecordPool make_pool(const char* name, std::int32_t entry_size,
                          std::uint32_t data, std::uint32_t bitmap) {
    GuestRecordPool pool{};
    copy_name(pool.name, name);
    pool.maximum_count = h2::port::NativeTexturePoolStorage::maximum_entries;
    pool.size = entry_size;
    pool.valid = 1;
    pool.signature = kDataArraySignature;
    pool.first_free_index = 0;
    pool.next_salt = static_cast<std::int16_t>(0x8000u);
    pool.data = data;
    pool.bitmap = bitmap;
    return pool;
}

}  // namespace

namespace h2::port {

NativeTexturePoolStorage::NativeTexturePoolStorage(
    GuestArena& arena, NativeAsyncLifecycle& lifecycle)
    : arena_(arena), lifecycle_(lifecycle) {}

bool NativeTexturePoolStorage::publish(
    std::uint32_t texture_pool, std::uint32_t upload_buffer,
    std::uint32_t physical_object, std::uint32_t upload_count,
    std::string& error) {
    if (!arena_.protect(kGlobalsAddress, kGlobalsSize,
                        GuestMemoryProtection::read_write, error)) {
        return false;
    }
    std::memcpy(arena_.pointer(global_texture_pool), &texture_pool,
                sizeof(texture_pool));
    std::memcpy(arena_.pointer(global_upload_buffer), &upload_buffer,
                sizeof(upload_buffer));
    std::memcpy(arena_.pointer(global_physical_object), &physical_object,
                sizeof(physical_object));
    std::memcpy(arena_.pointer(global_upload_count), &upload_count,
                sizeof(upload_count));
    return true;
}

bool NativeTexturePoolStorage::install(std::string& error) {
    error.clear();
    if (lifecycle_.state() != NativeAsyncLifecycle::State::stopped) {
        error = "native texture pools require a stopped async lifecycle";
        return false;
    }
    if (installed_) {
        error = "native texture pools are already installed";
        return false;
    }
    if (!arena_.protect(kStorageAddress, kStorageSize,
                        GuestMemoryProtection::read_write, error) ||
        !arena_.protect(upload_buffer_address, upload_buffer_size,
                        GuestMemoryProtection::read_write, error)) {
        return false;
    }
    std::memset(arena_.pointer(kStorageAddress), 0, kStorageSize);
    std::memset(arena_.pointer(upload_buffer_address), 0, upload_buffer_size);

    const GuestRecordPool physical_pool = make_pool(
        "texture physical", 0x18, physical_entries_address,
        physical_bitmap_address);
    const GuestRecordPool texture_pool = make_pool(
        "texture records", 0x28, texture_entries_address,
        texture_bitmap_address);
    GuestPhysicalObject physical{};
    copy_name(physical.name, "texture cache");
    physical.state = 2;
    physical.page_count = upload_page_count;
    physical.page_shift = 12;
    physical.time = 1;
    physical.first = -1;
    physical.last = -1;
    for (auto& limit : physical.limits)
        limit = std::numeric_limits<std::int32_t>::max();
    physical.blocks = physical_pool_address;

    std::memcpy(arena_.pointer(physical_pool_address), &physical_pool,
                sizeof(physical_pool));
    std::memcpy(arena_.pointer(texture_pool_address), &texture_pool,
                sizeof(texture_pool));
    std::memcpy(arena_.pointer(physical_object_address), &physical,
                sizeof(physical));
    if (!publish(texture_pool_address, upload_buffer_address,
                 physical_object_address, 0, error)) {
        std::memset(arena_.pointer(kStorageAddress), 0, kStorageSize);
        std::memset(arena_.pointer(upload_buffer_address), 0,
                    upload_buffer_size);
        return false;
    }
    installed_ = true;
    return true;
}

bool NativeTexturePoolStorage::clear(std::string& error) {
    error.clear();
    if (lifecycle_.state() != NativeAsyncLifecycle::State::stopped) {
        error = "native texture pools require a stopped async lifecycle";
        return false;
    }
    if (!installed_)
        return true;
    if (!publish(0, 0, 0, 0, error))
        return false;
    std::memset(arena_.pointer(kStorageAddress), 0, kStorageSize);
    std::memset(arena_.pointer(upload_buffer_address), 0, upload_buffer_size);
    installed_ = false;
    return true;
}

bool NativeTexturePoolStorage::installed() const {
    return installed_;
}

}  // namespace h2::port
