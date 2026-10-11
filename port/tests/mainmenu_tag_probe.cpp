#include "h2/host_abi.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#include <sys/mman.h>

extern bool cache_files_load_map(const char* map_name);
extern void* cache_tag_instance_get(long tag_index);
extern "C" int h2_call_guest_bool_1(bool (*function)(const char*),
                                     std::uint32_t argument, void* stack_top);
extern "C" std::uintptr_t h2_call_guest_pointer_1(void* (*function)(long),
                                                   std::uint32_t argument,
                                                   void* stack_top);

namespace {
constexpr std::uintptr_t kBias = 0x10000000000ULL;
constexpr std::uint32_t kPhysicalBase = 0x80061000;
std::vector<unsigned char> map_data;

void* map_exact(std::uint32_t guest_address, std::size_t size) {
    void* const requested = reinterpret_cast<void*>(kBias + guest_address);
    void* const mapped = mmap(requested, size, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mapped != requested) {
        if (mapped != MAP_FAILED) munmap(mapped, size);
        return nullptr;
    }
    return mapped;
}

std::uint32_t read_guest_u32(std::uint32_t address) {
    return *reinterpret_cast<const std::uint32_t*>(kBias + address);
}

}  // namespace

extern "C" unsigned int host_h2_abi_version() { return H2_HOST_ABI_VERSION; }
extern "C" void host_h2_log(const char*) {}
extern "C" void* host_h2_physical_memory_malloc_low(int size, unsigned int) {
    return size > 0 && size <= 0x1d3000 ? reinterpret_cast<void*>(kPhysicalBase) : nullptr;
}
extern "C" void* host_h2_physical_alloc(unsigned int size,
                                          unsigned int physical_address,
                                          unsigned int alignment,
                                          unsigned int protection) {
    return size == 0x30e4000u && physical_address == 0x61000u && !alignment &&
                   protection == 0x04u
               ? reinterpret_cast<void*>(kPhysicalBase)
               : nullptr;
}
extern "C" int host_h2_physical_protect(void*, unsigned int,
                                         unsigned int) {
    return 1;
}
extern "C" void* host_h2_virtual_alloc(void*, unsigned int size,
                                         unsigned int allocation_type,
                                         unsigned int protection) {
    return size == 0x23000u && allocation_type == 0x1000u && protection == 0x04u
               ? reinterpret_cast<void*>(0x30000000u)
               : nullptr;
}
extern "C" int host_h2_virtual_free(void*, unsigned int size,
                                      unsigned int free_type) {
    return size == 0 && free_type == 0x8000u;
}
extern "C" int host_h2_cache_read(unsigned int offset, int size, void* buffer) {
    if (size < 0 || offset > map_data.size() ||
        static_cast<std::size_t>(size) > map_data.size() - offset) {
        return 0;
    }
    std::memcpy(buffer, map_data.data() + offset, static_cast<std::size_t>(size));
    return 1;
}
extern "C" void GetSystemTimeAsFileTime(void*) {}

long cache_file_get_maximum_size(long) { return 0x20800000; }
long cache_file_find(const char*) { return 0; }
bool version_is_compatible(const char*) { return true; }
void function_213890() {}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fputs("usage: h2-mainmenu-tag-probe /path/to/mainmenu.map\n", stderr);
        return 2;
    }
    std::ifstream input(argv[1], std::ios::binary);
    map_data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    if (map_data.size() != 59'670'016) {
        std::fputs("unsupported mainmenu.map size\n", stderr);
        return 1;
    }

    struct Mapping { std::uint32_t address; std::size_t size; void* pointer; };
    std::array<Mapping, 8> mappings{{
        {0x468000, 0x4000, nullptr}, {0x4e0000, 0x4000, nullptr},
        {0x510000, 0x4000, nullptr}, {0x544000, 0x4000, nullptr},
        {0x554000, 0x4000, nullptr}, {0x558000, 0x4000, nullptr},
        {0x800000, 0x40000, nullptr}, {0x80060000, 0x1d4000, nullptr},
    }};
    for (auto& mapping : mappings) {
        mapping.pointer = map_exact(mapping.address, mapping.size);
        if (!mapping.pointer) {
            std::fputs("could not reserve a guest probe mapping\n", stderr);
            for (const auto& previous : mappings)
                if (previous.pointer) munmap(previous.pointer, previous.size);
            return 1;
        }
    }

    std::memcpy(reinterpret_cast<void*>(kBias + 0x557c90 + 4), map_data.data(), 0x800);
    void* const stack_top = reinterpret_cast<void*>(kBias + 0x840000 - 16);
    const bool loaded = h2_call_guest_bool_1(cache_files_load_map, 0, stack_top) != 0;

    const std::uint32_t tags = read_guest_u32(0x5476f8 + 0x808);
    const std::uint32_t instances = read_guest_u32(0x4e3b44);
    const std::uint32_t scenario = read_guest_u32(0x4686c0);
    const std::uint32_t group_count = tags ? read_guest_u32(tags + 4) : 0;
    const std::uint32_t globals = tags ? read_guest_u32(tags + 0x10) : 0;
    const std::uint32_t instance_count = tags ? read_guest_u32(tags + 0x18) : 0;
    const std::uint32_t signature = tags ? read_guest_u32(tags + 0x1c) : 0;
    const std::uint32_t scenario_instance = static_cast<std::uint32_t>(
        h2_call_guest_pointer_1(cache_tag_instance_get, scenario, stack_top));
    const std::uint32_t globals_instance = static_cast<std::uint32_t>(
        h2_call_guest_pointer_1(cache_tag_instance_get, globals, stack_top));
    const std::uint32_t scenario_address = scenario_instance ? read_guest_u32(scenario_instance + 8) : 0;
    const std::uint32_t globals_address = globals_instance ? read_guest_u32(globals_instance + 8) : 0;
    const bool valid = loaded && *reinterpret_cast<unsigned char*>(kBias + 0x5476f8) &&
                       tags == kPhysicalBase && instances != 0 && scenario != 0xffffffff &&
                       instance_count > 0 && signature == 0x74616773 &&
                       scenario_instance != 0 && globals_instance != 0 &&
                       scenario_address != 0 && globals_address != 0;

    std::printf("loaded=%d tags=%#x groups=%u instances=%u scenario=%#x@%#x globals=%#x@%#x\n",
                loaded, tags, group_count, instance_count, scenario, scenario_address,
                globals, globals_address);
    for (const auto& mapping : mappings) munmap(mapping.pointer, mapping.size);
    return valid ? 0 : 1;
}
