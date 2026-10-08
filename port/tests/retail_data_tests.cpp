#include "h2/retail_data.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

void write_u32(std::array<unsigned char, 0x800>& header, std::size_t offset,
               std::uint32_t value) {
    header[offset] = static_cast<unsigned char>(value);
    header[offset + 1] = static_cast<unsigned char>(value >> 8);
    header[offset + 2] = static_cast<unsigned char>(value >> 16);
    header[offset + 3] = static_cast<unsigned char>(value >> 24);
}

void write_cache(const std::filesystem::path& path, std::uint32_t size,
                 std::uint32_t type, const std::string& name) {
    std::array<unsigned char, 0x800> header{};
    write_u32(header, 0, 0x68656164);
    write_u32(header, 4, 8);
    write_u32(header, 8, size);
    write_u32(header, 0x140, type);
    write_u32(header, 0x7fc, 0x666f6f74);
    const std::string build = "02.09.27.09809";
    std::copy(build.begin(), build.end(), header.begin() + 0x120);
    std::copy(name.begin(), name.end(), header.begin() + 0x198);

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char*>(header.data()),
                 static_cast<std::streamsize>(header.size()));
    output.seekp(size - 1);
    output.put('\0');
}

}  // namespace

int main() {
    const auto root = std::filesystem::temp_directory_path() / "h2-retail-data-test";
    std::error_code error;
    std::filesystem::remove_all(root, error);
    std::filesystem::create_directories(root / "maps");

    const auto empty = h2::port::inspect_retail_data(root);
    if (empty.ready || empty.errors.size() != 3) {
        std::cerr << "missing-file validation did not report the three required files\n";
        return 1;
    }

    std::ofstream(root / "default.xbe", std::ios::binary).put('\0');
    std::ofstream(root / "maps/mainmenu.map", std::ios::binary).put('\0');
    std::ofstream(root / "maps/shared.map", std::ios::binary).put('\0');
    const auto wrong = h2::port::inspect_retail_data(root);
    if (wrong.ready || wrong.xbe_sha256.empty() || wrong.errors.size() != 4) {
        std::cerr << "size/hash validation did not reject synthetic retail files\n";
        std::filesystem::remove_all(root, error);
        return 1;
    }

    write_cache(root / "maps/mainmenu.map", 59'670'016, 2, "mainmenu");
    write_cache(root / "maps/shared.map", 183'370'752, 3, "shared");
    const auto maps = h2::port::inspect_retail_data(root);
    std::filesystem::remove_all(root, error);
    if (maps.ready || maps.errors.size() != 2 ||
        maps.cache_build != "02.09.27.09809" || maps.main_menu_name != "mainmenu") {
        std::cerr << "cache header validation did not accept supported synthetic maps\n";
        return 1;
    }
    return 0;
}
