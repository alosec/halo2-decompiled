#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace h2::port {

struct RetailFile {
    std::filesystem::path relative_path;
    std::uintmax_t expected_size;
};

struct RetailDataReport {
    bool ready = false;
    std::string xbe_sha256;
    std::vector<std::string> errors;
};

inline constexpr char kRetailXbeSha256[] =
    "03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d";

RetailDataReport inspect_retail_data(const std::filesystem::path& root);

}  // namespace h2::port
