#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/profile_store.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace {

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
    }
    return condition;
}

struct TemporaryDirectory {
    std::filesystem::path path;
    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
};

}  // namespace

int main() {
    using namespace h2::port;
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    TemporaryDirectory temporary{
        std::filesystem::temp_directory_path() /
        ("h2-native-profile-store-" + std::to_string(nonce))};

    bool ok = true;
    std::string error;
    NativeProfileStore store;
    ok &= check(!store.is_open(), "new profile store unexpectedly open");
    ok &= check(store.open(temporary.path, error), "profile store did not open");
    ok &= check(store.is_open(), "opened profile store reports closed");

    std::vector<std::uint32_t> ids;
    ok &= check(store.list(ids, error) && ids.empty(),
                "new profile store was not empty");

    std::array<std::byte, kNativeProfileRecordSize> first{};
    for (std::size_t index = 0; index < first.size(); ++index) {
        first[index] = static_cast<std::byte>(index & 0xffu);
    }
    std::uint32_t first_id = 99;
    ok &= check(store.create(first, first_id, error) && first_id == 0,
                "first profile did not receive identifier zero");

    std::array<std::byte, kNativeProfileRecordSize> loaded{};
    ok &= check(store.read(first_id, loaded, error) && loaded == first,
                "created profile did not round trip");

    auto second = first;
    second[0x40] = std::byte{0xa5};
    std::uint32_t second_id = 99;
    ok &= check(store.create(second, second_id, error) && second_id == 1,
                "second profile did not receive identifier one");
    ok &= check(store.list(ids, error) && ids == std::vector<std::uint32_t>({0, 1}),
                "profile identifiers were not sorted");

    first[0x1df] = std::byte{0x5a};
    ok &= check(store.write(first_id, first, error) &&
                    store.read(first_id, loaded, error) && loaded == first,
                "updated profile did not round trip");
    ok &= check(!store.write(5, first, error),
                "profile store updated a missing identifier");
    ok &= check(!store.create(std::span<const std::byte>(first.data(), first.size() - 1),
                              first_id, error),
                "profile store accepted a short record");

    ok &= check(store.erase(first_id, error), "profile deletion failed");
    std::uint32_t reused_id = 99;
    ok &= check(store.create(first, reused_id, error) && reused_id == 0,
                "profile store did not reuse the lowest free identifier");

    const auto corrupted_path = temporary.path / "profile-00000001.h2p";
    {
        std::fstream file(corrupted_path, std::ios::binary | std::ios::in | std::ios::out);
        file.seekp(-1, std::ios::end);
        const char changed = 0x33;
        file.write(&changed, 1);
    }
    ok &= check(!store.read(second_id, loaded, error) &&
                    error.find("checksum") != std::string::npos,
                "profile checksum corruption was not rejected");
    ok &= check(!store.list(ids, error),
                "profile enumeration did not fail closed on corruption");

    ok &= check(store.erase(second_id, error), "corrupt profile deletion failed");
    {
        std::ofstream unrelated(temporary.path / "README.txt");
        unrelated << "not a profile";
    }
    ok &= check(store.list(ids, error) && ids == std::vector<std::uint32_t>({0}),
                "unrelated files affected profile enumeration");

    store.close();
    ok &= check(!store.is_open() && !store.list(ids, error),
                "closed profile store accepted enumeration");

    const auto service_path = temporary.path / "service";
    ok &= check(configure_profile_services(service_path, error),
                "profile host service did not configure");
    std::uint32_t service_id = 99;
    ok &= check(create_native_profile(first, service_id, error) && service_id == 0,
                "profile host service did not create a record");
    loaded.fill(std::byte{0});
    ok &= check(read_native_profile(service_id, loaded, error) && loaded == first,
                "profile host service did not read a record");
    first[0x20] = std::byte{0x7b};
    ok &= check(write_native_profile(service_id, first, error) &&
                    list_native_profiles(ids, error) &&
                    ids == std::vector<std::uint32_t>({0}),
                "profile host service did not update/list a record");
    unsigned int abi_ids[2] = {99, 99};
    ok &= check(host_h2_abi_version() == H2_HOST_ABI_VERSION &&
                    host_h2_profile_list(nullptr, 0) == 1 &&
                    host_h2_profile_list(abi_ids, 2) == 1 && abi_ids[0] == 0,
                "profile host ABI did not enumerate records");
    loaded.fill(std::byte{0});
    ok &= check(host_h2_profile_read(service_id, loaded.data(), loaded.size()) == 1 &&
                    loaded == first &&
                    host_h2_profile_read(service_id, loaded.data(), loaded.size() - 1) == 0,
                "profile host ABI did not enforce bounded reads");
    first[0x21] = std::byte{0x6c};
    ok &= check(host_h2_profile_write(service_id, first.data(), first.size()) == 1 &&
                    host_h2_profile_delete(service_id) == 1,
                "profile host ABI did not update/delete a record");
    const int abi_id = host_h2_profile_create(first.data(), first.size());
    ok &= check(abi_id == 0 && host_h2_profile_create(first.data(), first.size() - 1) == -1 &&
                    host_h2_profile_delete(static_cast<unsigned int>(abi_id)) == 1,
                "profile host ABI did not create a bounded record");
    reset_profile_services();
    ok &= check(!list_native_profiles(ids, error),
                "reset profile host service accepted enumeration");
    return ok ? 0 : 1;
}
