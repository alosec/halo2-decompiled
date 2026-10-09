#ifndef H2_PORT_PROFILE_STORE_HPP
#define H2_PORT_PROFILE_STORE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace h2::port {

inline constexpr std::size_t kNativeProfileRecordSize = 0x1e0;
inline constexpr std::uint32_t kNativeProfileMaximumId = 0x0fff;

class NativeProfileStore {
public:
    bool open(const std::filesystem::path& directory, std::string& error);
    void close();
    bool is_open() const;

    bool list(std::vector<std::uint32_t>& ids, std::string& error) const;
    bool create(std::span<const std::byte> record, std::uint32_t& id,
                std::string& error);
    bool read(std::uint32_t id, std::array<std::byte, kNativeProfileRecordSize>& record,
              std::string& error) const;
    bool write(std::uint32_t id, std::span<const std::byte> record,
               std::string& error);
    bool erase(std::uint32_t id, std::string& error);

private:
    std::filesystem::path path_for(std::uint32_t id) const;
    bool write_file(const std::filesystem::path& path, std::uint32_t id,
                    std::span<const std::byte> record, std::string& error) const;

    std::filesystem::path directory_;
};

}  // namespace h2::port

#endif
