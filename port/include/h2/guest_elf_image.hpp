#pragma once

#include "h2/guest_arena.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace h2::port {

struct GuestElfSegment {
    std::uint32_t address;
    std::uint32_t size;
    GuestMemoryProtection protection;
};

class GuestElfImage {
public:
    static std::optional<GuestElfImage> load(const std::filesystem::path& path,
                                             GuestArena& arena,
                                             std::string& error);

    std::optional<std::uint32_t> symbol_address(std::string_view name) const;
    bool patch_host_import(GuestArena& arena, std::string_view symbol,
                           std::uintptr_t target, std::string& error) const;
    const std::vector<GuestElfSegment>& segments() const { return segments_; }

private:
    std::vector<GuestElfSegment> segments_;
    std::unordered_map<std::string, std::uint32_t> symbols_;
};

}  // namespace h2::port
