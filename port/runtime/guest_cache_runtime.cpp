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
