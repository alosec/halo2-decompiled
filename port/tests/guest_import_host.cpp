#include "h2/host_abi.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>

extern "C" int h2_guest_import_probe(const char* message);

namespace {
bool received_expected_message = false;
}

extern "C" unsigned int host_h2_abi_version() {
    return H2_HOST_ABI_VERSION;
}

extern "C" void host_h2_log(const char* message) {
    received_expected_message = message && std::strcmp(message, "guest import ready") == 0;
}

int main() {
    constexpr std::uintptr_t kGuestBias = 0x10000000000ULL;
    constexpr std::uintptr_t kGuestOffset = 0x14000;
    constexpr std::size_t kPageSize = 0x4000;
    void* const requested = reinterpret_cast<void*>(kGuestBias + kGuestOffset);
    void* const mapped = mmap(requested, kPageSize, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped == MAP_FAILED || mapped != requested) {
        if (mapped != MAP_FAILED) {
            munmap(mapped, kPageSize);
        }
        std::fputs("could not reserve the guest import test page\n", stderr);
        return 1;
    }

    std::strcpy(static_cast<char*>(mapped), "guest import ready");
    const int result = h2_guest_import_probe(reinterpret_cast<const char*>(kGuestOffset));
    munmap(mapped, kPageSize);
    if (result != 0 || !received_expected_message) {
        std::fputs("versioned guest-to-host import failed\n", stderr);
        return 1;
    }
    std::puts("guest-to-host ABI v1 pointer rebasing passed");
    return 0;
}
