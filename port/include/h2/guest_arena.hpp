#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace h2::port {

inline constexpr std::uintptr_t kGuestBias = 0x10000000000ULL;
inline constexpr std::size_t kGuestAddressSpaceSize = 0x1'0000'0000ULL;

struct XbeSection {
    std::string name;
    std::uint32_t virtual_address = 0;
    std::uint32_t virtual_size = 0;
    std::uint32_t raw_offset = 0;
    std::uint32_t raw_size = 0;
    std::uint32_t flags = 0;
};

struct XbeLoadReport {
    std::uint32_t image_base = 0;
    std::uint32_t encoded_entry = 0;
    std::vector<XbeSection> sections;
};

class GuestArena {
public:
    static std::unique_ptr<GuestArena> create(std::string& error);
    ~GuestArena();

    GuestArena(const GuestArena&) = delete;
    GuestArena& operator=(const GuestArena&) = delete;

    bool load_xbe(const std::filesystem::path& path, XbeLoadReport& report,
                  std::string& error);

    void* pointer(std::uint32_t guest_address) const;
    const void* pointer_const(std::uint32_t guest_address) const;

private:
    explicit GuestArena(void* reservation);
    bool make_writable(std::uint32_t address, std::uint32_t size, std::string& error);

    void* reservation_ = nullptr;
    std::size_t page_size_ = 0;
};

}  // namespace h2::port
