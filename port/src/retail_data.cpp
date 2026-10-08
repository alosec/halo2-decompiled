#include "h2/retail_data.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>

#include <openssl/evp.h>

namespace h2::port {
namespace {

const std::array<RetailFile, 3> kMenuMilestoneFiles{{
    {"default.xbe", 4'861'952},
    {"maps/mainmenu.map", 59'670'016},
    {"maps/shared.map", 183'370'752},
}};

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

    report.ready = report.errors.empty();
    return report;
}

}  // namespace h2::port
