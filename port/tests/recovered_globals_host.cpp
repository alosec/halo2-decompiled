#include <cstdint>
#include <cstdio>
#include <sys/mman.h>

extern void function_07a9a0(long unused);

int main() {
    constexpr std::uintptr_t kGuestBias = 0x10000000000ULL;
    constexpr std::uintptr_t kPage = 0x4cc000;
    constexpr std::size_t kPageSize = 0x4000;
    void* const requested = reinterpret_cast<void*>(kGuestBias + kPage);
    void* const mapped = mmap(requested, kPageSize, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped == MAP_FAILED || mapped != requested) {
        if (mapped != MAP_FAILED) {
            munmap(mapped, kPageSize);
        }
        std::fputs("could not reserve the recovered-global test page\n", stderr);
        return 1;
    }

    auto* const first = reinterpret_cast<unsigned char*>(kGuestBias + 0x4cf791);
    auto* const second = reinterpret_cast<unsigned char*>(kGuestBias + 0x4cf792);
    *first = 1;
    *second = 1;
    function_07a9a0(0);
    const bool passed = *first == 0 && *second == 0;
    munmap(mapped, kPageSize);
    if (!passed) {
        std::fputs("recovered function did not update its original globals\n", stderr);
        return 1;
    }
    std::puts("recovered ARM guest updated globals 0x4cf791 and 0x4cf792");
    return 0;
}
