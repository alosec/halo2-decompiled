#include <cmath>
#include <cstdint>
#include <cstdio>
#include <sys/mman.h>

// The ILP32 source uses unsigned long. On the host that spelling preserves the
// C++ symbol name; the guest function itself performs 32-bit loads and stores.
extern float function_259a0(unsigned long* seed);

int main() {
    constexpr std::uintptr_t kGuestBias = 0x10000000000ULL;
    constexpr std::uintptr_t kGuestOffset = 0x10000;
    constexpr std::size_t kPageSize = 0x4000;
    void* const requested = reinterpret_cast<void*>(kGuestBias + kGuestOffset);
    void* const mapped = mmap(requested, kPageSize, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped == MAP_FAILED || mapped != requested) {
        if (mapped != MAP_FAILED) {
            munmap(mapped, kPageSize);
        }
        std::fputs("could not reserve the guest test page at its rebased address\n", stderr);
        return 1;
    }

    auto* const physical_seed = static_cast<std::uint32_t*>(mapped);
    *physical_seed = 1;
    const float value = function_259a0(reinterpret_cast<unsigned long*>(kGuestOffset));
    constexpr std::uint32_t expected_seed = 1u * 1'664'525u + 1'013'904'223u;
    const float expected_value = static_cast<float>(expected_seed >> 16) * (1.0f / 65'535.0f);
    const bool passed = *physical_seed == expected_seed &&
                        std::fabs(value - expected_value) < 0.0000001f;
    munmap(mapped, kPageSize);
    if (!passed) {
        std::fputs("recovered random-number function returned the wrong result\n", stderr);
        return 1;
    }
    std::printf("recovered ARM guest executed: seed=%u value=%.9f\n", expected_seed, value);
    return 0;
}
