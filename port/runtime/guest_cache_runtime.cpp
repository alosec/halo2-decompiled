#include <xtl.h>
#include "h2/host_abi.h"
#include "unknown_122870.h"

/* The native host owns one validated active map and no asynchronous Xbox
 * cache-file handles. These narrow services preserve the recovered map loader
 * while making that ownership explicit. */
long cache_file_get_maximum_size(long)
{
    return 0x20800000;
}

long cache_file_find(char const *map_name)
{
    /* The recovered loader passes null for the already configured map. */
    return !map_name || host_h2_map_is_active(map_name) ? 0 : NONE;
}

extern "C" long h2_native_cache_file_find(char const *map_name)
{
    return cache_file_find(map_name);
}

bool __stdcall version_is_compatible(char const *)
{
    return true;
}

void function_213890()
{
}

bool cache_files_load_map(char const *map_name);
extern long g_4686c0;
extern void *g_4e0350;
extern void *g_4e034c;
extern void *g_4e3b44;

struct s_native_cache_tag_instance
{
    long group_tag;
    long datum_index;
    void *address;
    long size;
};
static_assert(sizeof(s_native_cache_tag_instance) == 0x10,
              "cache tag instance layout");

extern "C" int h2_native_campaign_bsp_metadata()
{
    if (!cache_file_globals.loaded || !cache_file_globals.tags || !g_4e0350 ||
        !g_4e3b44 || cache_file_globals.header.type != 0)
        return 0;
    unsigned char const *const scenario =
        reinterpret_cast<unsigned char const *>(g_4e0350);
    const long count = *reinterpret_cast<long const *>(scenario + 0x210);
    unsigned char const *const references =
        *reinterpret_cast<unsigned char const *const *>(scenario + 0x214);
    if (count <= 0 || count > 32 || !references)
        return 0;
    s_native_cache_tag_instance const *const instances =
        reinterpret_cast<s_native_cache_tag_instance const *>(g_4e3b44);
    for (long index = 0; index < count; ++index)
    {
        unsigned char const *const reference = references + index * 0x44;
        const unsigned long offset =
            *reinterpret_cast<unsigned long const *>(reference);
        const long size = *reinterpret_cast<long const *>(reference + 4);
        const long bsp = *reinterpret_cast<long const *>(reference + 0x14);
        const long lightmap = *reinterpret_cast<long const *>(reference + 0x1c);
        const short bsp_index = (short)bsp;
        if (size <= 0 || offset > (unsigned long)cache_file_globals.header.size ||
            (unsigned long)size > (unsigned long)cache_file_globals.header.size - offset ||
            bsp_index < 0 || bsp_index >= cache_file_globals.tags->instance_count ||
            instances[bsp_index].datum_index != bsp ||
            instances[bsp_index].group_tag != 'sbsp')
            return 0;
        if (lightmap != NONE)
        {
            const short lightmap_index = (short)lightmap;
            if (lightmap_index < 0 ||
                lightmap_index >= cache_file_globals.tags->instance_count ||
                instances[lightmap_index].datum_index != lightmap ||
                instances[lightmap_index].group_tag != 'ltmp')
                return 0;
        }
    }
    return count;
}

struct s_structure_bsp_reference
{
    unsigned long offset;
    long size;
    s_structure_bsp_header *address;
    unsigned char unknown0c[8];
    long bsp_tag_index;
    unsigned char unknown18[4];
    long lightmap_tag_index;
};
static_assert(sizeof(s_structure_bsp_reference) == 0x20,
              "structure BSP reference layout");

bool cache_files_load_structure_bsp(s_structure_bsp_reference *reference);

extern "C" int h2_native_campaign_load_first_bsp()
{
    if (h2_native_campaign_bsp_metadata() <= 0)
        return 0;
    unsigned char const *const scenario =
        reinterpret_cast<unsigned char const *>(g_4e0350);
    s_structure_bsp_reference *const references =
        *reinterpret_cast<s_structure_bsp_reference *const *>(scenario + 0x214);
    s_structure_bsp_reference *const reference = &references[0];
    if (!cache_files_load_structure_bsp(reference) ||
        cache_file_globals.bsp != reference->address ||
        !reference->address || reference->address->signature != 'sbsp')
        return 0;
    s_native_cache_tag_instance const *const instances =
        reinterpret_cast<s_native_cache_tag_instance const *>(g_4e3b44);
    if (instances[(short)reference->bsp_tag_index].address !=
        reference->address->bsp_address)
        return 0;
    if (reference->lightmap_tag_index != NONE &&
        instances[(short)reference->lightmap_tag_index].address !=
            reference->address->lightmap_address)
        return 0;
    return 1;
}

extern "C" int h2_native_load_campaign_map(char const *map_name)
{
    if (!map_name || cache_file_find(map_name) == NONE ||
        !cache_files_load_map(map_name))
        return 0;
    return cache_file_globals.loaded && cache_file_globals.tags &&
        cache_file_globals.header.type == 0 &&
        cache_file_globals.tags->instance_count > 0 && g_4686c0 != NONE &&
        g_4e0350 && g_4e034c;
}

extern "C" void __stdcall GetSystemTimeAsFileTime(FILETIME *time)
{
    if (time)
    {
        time->dwLowDateTime = 0;
        time->dwHighDateTime = 0;
    }
}
