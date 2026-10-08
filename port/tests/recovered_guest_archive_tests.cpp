#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <sys/mman.h>

extern float function_259a0(unsigned long* seed);
extern void function_07a9a0(long unused);
extern void* cache_tag_instance_get(long tag_index);

namespace {

void* map_exact(std::uintptr_t address, std::size_t size) {
    void* const requested = reinterpret_cast<void*>(address);
    void* const mapped = mmap(requested, size, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped != requested) {
        if (mapped != MAP_FAILED) {
            munmap(mapped, size);
        }
        return nullptr;
    }
    return mapped;
}

void write_guest_u32(std::uintptr_t address, std::uint32_t value) {
    *reinterpret_cast<std::uint32_t*>(address) = value;
}

}  // namespace

int main() {
    constexpr std::uintptr_t kBias = 0x10000000000ULL;
    constexpr std::size_t kPageSize = 0x4000;
    constexpr std::array<std::uintptr_t, 6> kGuestPages{
        0x10000, 0x4cc000, 0x4e0000, 0x544000, 0x600000, 0x604000,
    };
    std::array<void*, kGuestPages.size()> mappings{};
    for (std::size_t index = 0; index < kGuestPages.size(); ++index) {
        mappings[index] = map_exact(kBias + kGuestPages[index], kPageSize);
        if (!mappings[index]) {
            std::fputs("could not map multi-unit guest test pages\n", stderr);
            for (std::size_t previous = 0; previous < index; ++previous) {
                munmap(mappings[previous], kPageSize);
            }
            return 1;
        }
    }

    auto* const seed = static_cast<std::uint32_t*>(mappings[0]);
    *seed = 1;
    const float value = function_259a0(reinterpret_cast<unsigned long*>(0x10000));
    auto* const first = reinterpret_cast<unsigned char*>(kBias + 0x4cf791);
    auto* const second = reinterpret_cast<unsigned char*>(kBias + 0x4cf792);
    *first = 1;
    *second = 1;
    function_07a9a0(0);

    constexpr std::uint32_t kTagsHeader = 0x600000;
    constexpr std::uint32_t kInstances = 0x604000;
    constexpr std::uint32_t kDatum = 0x12340001;
    write_guest_u32(kBias + 0x5476f8 + 0x808, kTagsHeader);
    write_guest_u32(kBias + 0x4e3b44, kInstances);
    write_guest_u32(kBias + kTagsHeader + 0x18, 2);
    write_guest_u32(kBias + kInstances + 0x10 + 0x4, kDatum);
    void* const instance = cache_tag_instance_get(kDatum);

    constexpr std::uint32_t expected_seed = 1u * 1'664'525u + 1'013'904'223u;
    const float expected_value = static_cast<float>(expected_seed >> 16) * (1.0f / 65'535.0f);
    const bool passed = *seed == expected_seed &&
                        std::fabs(value - expected_value) < 0.0000001f &&
                        *first == 0 && *second == 0 &&
                        reinterpret_cast<std::uintptr_t>(instance) == kInstances + 0x10;
    if (!passed) {
        std::fprintf(stderr,
                     "multi-unit recovered guest archive failed: seed=%u value=%.9f "
                     "globals=%u,%u instance=%#llx\n",
                     *seed, value, static_cast<unsigned>(*first), static_cast<unsigned>(*second),
                     static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(instance)));
    }
    for (void* mapping : mappings) {
        munmap(mapping, kPageSize);
    }
    if (!passed) {
        return 1;
    }
    std::puts("three-unit recovered guest archive and cache-tag lookup passed");
    return 0;
}
