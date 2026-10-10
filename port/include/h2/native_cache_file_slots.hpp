#ifndef H2_PORT_NATIVE_CACHE_FILE_SLOTS_HPP
#define H2_PORT_NATIVE_CACHE_FILE_SLOTS_HPP

#include "h2/native_async_platform.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

namespace h2::port {

class NativeAsyncLifecycle;

class NativeCacheFileSlots {
public:
    static constexpr std::size_t slot_count = 6;
    static constexpr std::size_t header_size = 0x800;
    static constexpr std::int32_t no_active_slot = -1;

    NativeCacheFileSlots(NativeAsyncPlatform& platform,
                         NativeAsyncLifecycle& lifecycle);
    ~NativeCacheFileSlots();
    NativeCacheFileSlots(const NativeCacheFileSlots&) = delete;
    NativeCacheFileSlots& operator=(const NativeCacheFileSlots&) = delete;

    bool install(const std::filesystem::path& path, std::int16_t expected_type,
                 const std::string& expected_name, bool make_active,
                 std::string& error);
    bool clear(std::string& error);

    std::int32_t active_slot() const;
    NativeAsyncPlatform::Handle slot_handle(std::size_t index) const;
    std::array<std::uint8_t, header_size> slot_header(std::size_t index) const;

private:
    struct Data;
    std::unique_ptr<Data> data_;
};

} // namespace h2::port

#endif
