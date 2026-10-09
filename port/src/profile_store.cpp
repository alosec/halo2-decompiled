#include "h2/profile_store.hpp"

#include <openssl/evp.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {

constexpr std::array<unsigned char, 8> kMagic = {'H', '2', 'N', 'P', 'R', 'O', 'F', 0};
constexpr std::uint32_t kVersion = 1;
constexpr std::size_t kHeaderSize = 8 + 4 + 4 + 4 + 32;

void store_u32(unsigned char* destination, std::uint32_t value) {
    destination[0] = static_cast<unsigned char>(value);
    destination[1] = static_cast<unsigned char>(value >> 8);
    destination[2] = static_cast<unsigned char>(value >> 16);
    destination[3] = static_cast<unsigned char>(value >> 24);
}

std::uint32_t load_u32(const unsigned char* source) {
    return static_cast<std::uint32_t>(source[0]) |
           (static_cast<std::uint32_t>(source[1]) << 8) |
           (static_cast<std::uint32_t>(source[2]) << 16) |
           (static_cast<std::uint32_t>(source[3]) << 24);
}

bool digest(std::span<const std::byte> record, std::array<unsigned char, 32>& result) {
    unsigned int size = 0;
    return EVP_Digest(record.data(), record.size(), result.data(), &size,
                      EVP_sha256(), nullptr) == 1 && size == result.size();
}

bool parse_filename(const std::filesystem::path& path, std::uint32_t& id) {
    const std::string name = path.filename().string();
    constexpr std::string_view prefix = "profile-";
    constexpr std::string_view suffix = ".h2p";
    if (name.size() != prefix.size() + 8 + suffix.size() ||
        name.compare(0, prefix.size(), prefix) != 0 ||
        name.compare(name.size() - suffix.size(), suffix.size(), suffix) != 0) {
        return false;
    }
    const char* begin = name.data() + prefix.size();
    const char* end = begin + 8;
    for (const char* current = begin; current != end; ++current) {
        if (!((*current >= '0' && *current <= '9') ||
              (*current >= 'a' && *current <= 'f'))) {
            return false;
        }
    }
    const auto converted = std::from_chars(begin, end, id, 16);
    return converted.ec == std::errc{} && converted.ptr == end &&
           id <= h2::port::kNativeProfileMaximumId;
}

}  // namespace

namespace h2::port {

bool NativeProfileStore::open(const std::filesystem::path& directory,
                              std::string& error) {
    close();
    if (directory.empty()) {
        error = "native profile directory is empty";
        return false;
    }
    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    if (ec || !std::filesystem::is_directory(directory, ec) || ec) {
        error = "could not create native profile directory: " + directory.string();
        return false;
    }
    std::filesystem::permissions(
        directory, std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace, ec);
    if (ec) {
        error = "could not secure native profile directory: " + directory.string();
        return false;
    }
    directory_ = directory;
    return true;
}

void NativeProfileStore::close() {
    directory_.clear();
}

bool NativeProfileStore::is_open() const {
    return !directory_.empty();
}

std::filesystem::path NativeProfileStore::path_for(std::uint32_t id) const {
    std::ostringstream name;
    name << "profile-" << std::hex << std::nouppercase << std::setw(8)
         << std::setfill('0') << id << ".h2p";
    return directory_ / name.str();
}

bool NativeProfileStore::list(std::vector<std::uint32_t>& ids,
                              std::string& error) const {
    ids.clear();
    if (!is_open()) {
        error = "native profile store is not open";
        return false;
    }
    std::error_code ec;
    for (std::filesystem::directory_iterator iterator(directory_, ec), end;
         !ec && iterator != end; iterator.increment(ec)) {
        std::uint32_t id = 0;
        if (!parse_filename(iterator->path(), id)) {
            continue;
        }
        const auto status = iterator->symlink_status(ec);
        if (ec || !std::filesystem::is_regular_file(status)) {
            error = "native profile entry is not a regular file: " +
                    iterator->path().string();
            return false;
        }
        std::array<std::byte, kNativeProfileRecordSize> record{};
        if (!read(id, record, error)) {
            return false;
        }
        ids.push_back(id);
    }
    if (ec) {
        error = "could not enumerate native profile directory: " + directory_.string();
        return false;
    }
    std::sort(ids.begin(), ids.end());
    if (std::adjacent_find(ids.begin(), ids.end()) != ids.end()) {
        error = "native profile directory contains duplicate identifiers";
        return false;
    }
    return true;
}

bool NativeProfileStore::create(std::span<const std::byte> record, std::uint32_t& id,
                                std::string& error) {
    if (record.size() != kNativeProfileRecordSize) {
        error = "native profile record has the wrong size";
        return false;
    }
    std::vector<std::uint32_t> ids;
    if (!list(ids, error)) {
        return false;
    }
    for (id = 0; id <= kNativeProfileMaximumId; ++id) {
        if (!std::binary_search(ids.begin(), ids.end(), id)) {
            return write_file(path_for(id), id, record, error);
        }
    }
    error = "native profile store is full";
    return false;
}

bool NativeProfileStore::read(
    std::uint32_t id, std::array<std::byte, kNativeProfileRecordSize>& record,
    std::string& error) const {
    if (!is_open() || id > kNativeProfileMaximumId) {
        error = "native profile identifier is invalid";
        return false;
    }
    const auto path = path_for(id);
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || !std::filesystem::is_regular_file(status)) {
        error = "native profile does not exist: " + path.string();
        return false;
    }
    std::ifstream input(path, std::ios::binary);
    std::array<unsigned char, kHeaderSize> header{};
    if (!input.read(reinterpret_cast<char*>(header.data()), header.size()) ||
        !input.read(reinterpret_cast<char*>(record.data()), record.size())) {
        error = "native profile is truncated: " + path.string();
        return false;
    }
    char trailing = 0;
    if (input.read(&trailing, 1)) {
        error = "native profile has trailing data: " + path.string();
        return false;
    }
    if (!std::equal(kMagic.begin(), kMagic.end(), header.begin()) ||
        load_u32(header.data() + 8) != kVersion ||
        load_u32(header.data() + 12) != id ||
        load_u32(header.data() + 16) != kNativeProfileRecordSize) {
        error = "native profile header is invalid: " + path.string();
        return false;
    }
    std::array<unsigned char, 32> expected{};
    if (!digest(record, expected) ||
        std::memcmp(header.data() + 20, expected.data(), expected.size()) != 0) {
        error = "native profile checksum is invalid: " + path.string();
        return false;
    }
    return true;
}

bool NativeProfileStore::write(std::uint32_t id, std::span<const std::byte> record,
                               std::string& error) {
    if (record.size() != kNativeProfileRecordSize || id > kNativeProfileMaximumId) {
        error = "native profile write parameters are invalid";
        return false;
    }
    const auto path = path_for(id);
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || !std::filesystem::is_regular_file(status)) {
        error = "native profile does not exist: " + path.string();
        return false;
    }
    return write_file(path, id, record, error);
}

bool NativeProfileStore::erase(std::uint32_t id, std::string& error) {
    if (!is_open() || id > kNativeProfileMaximumId) {
        error = "native profile identifier is invalid";
        return false;
    }
    const auto path = path_for(id);
    std::error_code ec;
    const auto status = std::filesystem::symlink_status(path, ec);
    if (ec || !std::filesystem::is_regular_file(status) ||
        !std::filesystem::remove(path, ec) || ec) {
        error = "could not remove native profile: " + path.string();
        return false;
    }
    return true;
}

bool NativeProfileStore::write_file(const std::filesystem::path& path,
                                    std::uint32_t id,
                                    std::span<const std::byte> record,
                                    std::string& error) const {
    if (!is_open() || record.size() != kNativeProfileRecordSize) {
        error = "native profile store write is invalid";
        return false;
    }
    std::array<unsigned char, kHeaderSize> header{};
    std::copy(kMagic.begin(), kMagic.end(), header.begin());
    store_u32(header.data() + 8, kVersion);
    store_u32(header.data() + 12, id);
    store_u32(header.data() + 16, static_cast<std::uint32_t>(record.size()));
    std::array<unsigned char, 32> checksum{};
    if (!digest(record, checksum)) {
        error = "could not checksum native profile";
        return false;
    }
    std::copy(checksum.begin(), checksum.end(), header.begin() + 20);

    const auto temporary = path.string() + ".tmp";
    std::error_code ec;
    std::filesystem::remove(temporary, ec);
    ec.clear();
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        output.write(reinterpret_cast<const char*>(header.data()), header.size());
        output.write(reinterpret_cast<const char*>(record.data()), record.size());
        output.flush();
        if (!output) {
            error = "could not write native profile: " + temporary;
            output.close();
            std::filesystem::remove(temporary, ec);
            return false;
        }
    }
    std::filesystem::permissions(
        temporary,
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace, ec);
    if (ec) {
        error = "could not secure native profile: " + temporary;
        std::filesystem::remove(temporary, ec);
        return false;
    }
    std::filesystem::rename(temporary, path, ec);
    if (ec) {
        error = "could not commit native profile: " + path.string();
        std::filesystem::remove(temporary, ec);
        return false;
    }
    return true;
}

}  // namespace h2::port
