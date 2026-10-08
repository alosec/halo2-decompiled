#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sys/mman.h>

struct s_cache_header {
    std::int32_t header_signature;
    std::int32_t version;
    std::int32_t size;
    std::uint32_t unknown0c;
    std::uint32_t tag_data_offset;
    std::uint32_t tag_data_size;
    std::uint32_t unknown18;
    std::int32_t unknown1c;
    unsigned char unknown20[0x100];
    char build_version[0x20];
    std::int16_t type;
    unsigned char unknown142[0x56];
    char name[32];
    unsigned char unknown1b8[0x644];
    std::int32_t footer_signature;
};
static_assert(sizeof(s_cache_header) == 0x800);

extern bool cache_header_verify(const s_cache_header* header);

extern "C" void GetSystemTimeAsFileTime(void*) {}

long cache_file_get_maximum_size(long) {
    return 0x20800000;
}

int main(int argc, char** argv) {
    constexpr std::uintptr_t kBias = 0x10000000000ULL;
    constexpr std::uintptr_t kOffset = 0x700000;
    constexpr std::size_t kPageSize = 0x4000;
    void* const requested = reinterpret_cast<void*>(kBias + kOffset);
    void* const mapped = mmap(requested, kPageSize, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped == MAP_FAILED || mapped != requested) {
        if (mapped != MAP_FAILED) munmap(mapped, kPageSize);
        std::fputs("could not map recovered cache-header test page\n", stderr);
        return 1;
    }

    auto* const header = static_cast<s_cache_header*>(mapped);
    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input.read(reinterpret_cast<char*>(header), sizeof(*header))) {
            std::fputs("could not read the supplied retail cache header\n", stderr);
            munmap(mapped, kPageSize);
            return 1;
        }
    } else if (argc == 1) {
        header->header_signature = 0x68656164;
        header->footer_signature = 0x666f6f74;
        header->version = 8;
        header->size = 59'670'016;
        header->type = 2;
        std::strcpy(header->build_version, "02.09.27.09809");
        std::strcpy(header->name, "mainmenu");
    } else {
        std::fputs("usage: h2-recovered-cache-header-tests [mainmenu.map]\n", stderr);
        munmap(mapped, kPageSize);
        return 2;
    }

    auto* const guest_header = reinterpret_cast<const s_cache_header*>(kOffset);
    const bool accepted = cache_header_verify(guest_header);
    header->version = 7;
    const bool rejected = !cache_header_verify(guest_header);
    munmap(mapped, kPageSize);
    if (!accepted || !rejected) {
        std::fputs("recovered cache-header verifier returned the wrong result\n", stderr);
        return 1;
    }
    std::puts("recovered retail cache-header verification passed");
    return 0;
}
