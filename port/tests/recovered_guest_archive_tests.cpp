#include <cmath>
#include <cstdint>
#include <cstdio>
#include <sys/mman.h>

extern float function_259a0(unsigned long* seed);
extern void function_07a9a0(long unused);

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

}  // namespace

int main() {
    constexpr std::uintptr_t kBias = 0x10000000000ULL;
    constexpr std::size_t kPageSize = 0x4000;
    void* const random_page = map_exact(kBias + 0x10000, kPageSize);
    void* const globals_page = map_exact(kBias + 0x4cc000, kPageSize);
    if (!random_page || !globals_page) {
        std::fputs("could not map multi-unit guest test pages\n", stderr);
        return 1;
    }

    auto* const seed = static_cast<std::uint32_t*>(random_page);
    *seed = 1;
    const float value = function_259a0(reinterpret_cast<unsigned long*>(0x10000));
    auto* const first = reinterpret_cast<unsigned char*>(kBias + 0x4cf791);
    auto* const second = reinterpret_cast<unsigned char*>(kBias + 0x4cf792);
    *first = 1;
    *second = 1;
    function_07a9a0(0);

    constexpr std::uint32_t expected_seed = 1u * 1'664'525u + 1'013'904'223u;
    const float expected_value = static_cast<float>(expected_seed >> 16) * (1.0f / 65'535.0f);
    const bool passed = *seed == expected_seed &&
                        std::fabs(value - expected_value) < 0.0000001f &&
                        *first == 0 && *second == 0;
    munmap(random_page, kPageSize);
    munmap(globals_page, kPageSize);
    if (!passed) {
        std::fputs("multi-unit recovered guest archive failed\n", stderr);
        return 1;
    }
    std::puts("two-unit recovered guest archive executed successfully");
    return 0;
}
