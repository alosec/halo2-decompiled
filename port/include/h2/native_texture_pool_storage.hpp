#ifndef H2_PORT_NATIVE_TEXTURE_POOL_STORAGE_HPP
#define H2_PORT_NATIVE_TEXTURE_POOL_STORAGE_HPP

#include <cstdint>
#include <string>

namespace h2::port {

class GuestArena;
class NativeAsyncLifecycle;

class NativeTexturePoolStorage {
public:
    static constexpr std::uint32_t global_texture_pool = 0x004e6454u;
    static constexpr std::uint32_t global_upload_buffer = 0x004e6460u;
    static constexpr std::uint32_t global_physical_object = 0x004e6464u;
    static constexpr std::uint32_t global_upload_count = 0x004e6484u;

    static constexpr std::uint32_t physical_object_address = 0x20400000u;
    static constexpr std::uint32_t physical_pool_address = 0x20400100u;
    static constexpr std::uint32_t physical_entries_address = 0x20400200u;
    static constexpr std::uint32_t physical_bitmap_address = 0x20400800u;
    static constexpr std::uint32_t texture_pool_address = 0x20400900u;
    static constexpr std::uint32_t texture_entries_address = 0x20400a00u;
    static constexpr std::uint32_t texture_bitmap_address = 0x20401400u;
    static constexpr std::uint32_t upload_buffer_address = 0x20500000u;
    static constexpr std::uint32_t maximum_entries = 64u;
    static constexpr std::uint32_t upload_page_count = 16u;
    static constexpr std::uint32_t upload_buffer_size = upload_page_count << 12;

    NativeTexturePoolStorage(GuestArena& arena, NativeAsyncLifecycle& lifecycle);

    bool install(std::uint32_t delete_callback,
                 std::uint32_t busy_callback,
                 std::uint32_t state_callback,
                 std::string& error);
    bool clear(std::string& error);
    bool installed() const;

private:
    bool publish(std::uint32_t texture_pool, std::uint32_t upload_buffer,
                 std::uint32_t physical_object, std::uint32_t upload_count,
                 std::string& error);

    GuestArena& arena_;
    NativeAsyncLifecycle& lifecycle_;
    bool installed_ = false;
};

}  // namespace h2::port

#endif
