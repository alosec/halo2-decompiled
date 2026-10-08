#include "h2/retail_data.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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
    std::filesystem::remove_all(root, error);

    if (wrong.ready || wrong.xbe_sha256.empty() || wrong.errors.size() != 4) {
        std::cerr << "size/hash validation did not reject synthetic retail files\n";
        return 1;
    }
    return 0;
}
