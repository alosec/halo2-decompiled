#include "h2/retail_data.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string_view>

#include <openssl/evp.h>

namespace h2::port {
namespace {

const std::array<RetailFile, 3> kMenuMilestoneFiles{{
    {"default.xbe", 4'861'952},
    {"maps/mainmenu.map", 59'670'016},
    {"maps/shared.map", 183'370'752},
}};

struct CacheHeaderInfo {
    std::string build;
    std::string name;
};

std::uint32_t read_u32(const std::array<unsigned char, 0x800>& data, std::size_t offset) {
    return static_cast<std::uint32_t>(data[offset]) |
           (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(data[offset + 3]) << 24);
}

std::string fixed_string(const std::array<unsigned char, 0x800>& data,
                         std::size_t offset, std::size_t capacity) {
    std::size_t length = 0;
    while (length < capacity && data[offset + length]) {
        ++length;
    }
    return std::string(reinterpret_cast<const char*>(data.data() + offset), length);
}

bool inspect_cache_header(const std::filesystem::path& path, std::uintmax_t expected_size,
                          std::uint32_t expected_type, std::string_view expected_name,
                          CacheHeaderInfo& info, std::string& error) {
    std::error_code filesystem_error;
    if (!std::filesystem::is_regular_file(path, filesystem_error) ||
        std::filesystem::file_size(path, filesystem_error) != expected_size) {
        return false;
    }
    std::ifstream input(path, std::ios::binary);
    std::array<unsigned char, 0x800> header{};
    if (!input.read(reinterpret_cast<char*>(header.data()),
                    static_cast<std::streamsize>(header.size()))) {
        error = "could not read cache header: " + path.filename().string();
        return false;
    }
    info.build = fixed_string(header, 0x120, 0x20);
    info.name = fixed_string(header, 0x198, 0x20);
    if (read_u32(header, 0) != 0x68656164 || read_u32(header, 4) != 8 ||
        read_u32(header, 8) != expected_size || read_u32(header, 0x140) != expected_type ||
        read_u32(header, 0x7fc) != 0x666f6f74 || info.name != expected_name ||
        info.build != "02.09.27.09809") {
        error = "unsupported or malformed cache header: " + path.filename().string();
        return false;
    }
    return true;
}

std::string sha256_file(const std::filesystem::path& path) {
    using Context = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;
    Context context(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (!context || EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr) != 1) {
        return {};
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return {};
    }

    std::array<char, 1 << 16> buffer{};
    while (input) {
        input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const auto count = input.gcount();
        if (count > 0 &&
            EVP_DigestUpdate(context.get(), buffer.data(), static_cast<std::size_t>(count)) != 1) {
            return {};
        }
    }
    if (!input.eof()) {
        return {};
    }

    std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(context.get(), digest.data(), &length) != 1) {
        return {};
    }

    std::ostringstream hex;
    hex << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < length; ++i) {
        hex << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    return hex.str();
}

}  // namespace

RetailDataReport inspect_retail_data(const std::filesystem::path& root) {
    RetailDataReport report;

    std::error_code error;
    if (!std::filesystem::is_directory(root, error)) {
        report.errors.push_back("data root is not a readable directory: " + root.string());
        return report;
    }

    for (const auto& file : kMenuMilestoneFiles) {
        const auto path = root / file.relative_path;
        error.clear();
        if (!std::filesystem::is_regular_file(path, error)) {
            report.errors.push_back("missing retail file: " + file.relative_path.string());
            continue;
        }
        const auto size = std::filesystem::file_size(path, error);
        if (error || size != file.expected_size) {
            report.errors.push_back("unexpected size for " + file.relative_path.string());
        }
    }

    const auto xbe = root / "default.xbe";
    if (std::filesystem::is_regular_file(xbe)) {
        report.xbe_sha256 = sha256_file(xbe);
        if (report.xbe_sha256.empty()) {
            report.errors.push_back("could not hash default.xbe");
        } else if (report.xbe_sha256 != kRetailXbeSha256) {
            report.errors.push_back("default.xbe is not the supported retail build");
        }
    }

    CacheHeaderInfo main_menu;
    CacheHeaderInfo shared;
    std::string cache_error;
    const bool main_menu_valid = inspect_cache_header(
        root / "maps/mainmenu.map", 59'670'016, 2, "mainmenu", main_menu, cache_error);
    if (!main_menu_valid && !cache_error.empty()) {
        report.errors.push_back(cache_error);
    }
    cache_error.clear();
    const bool shared_valid = inspect_cache_header(
        root / "maps/shared.map", 183'370'752, 3, "shared", shared, cache_error);
    if (!shared_valid && !cache_error.empty()) {
        report.errors.push_back(cache_error);
    }
    if (main_menu_valid && shared_valid) {
        report.cache_build = main_menu.build;
        report.main_menu_name = main_menu.name;
        if (main_menu.build != shared.build) {
            report.errors.push_back("mainmenu.map and shared.map use different builds");
        }
    }

    report.ready = report.errors.empty();
    return report;
}

}  // namespace h2::port
