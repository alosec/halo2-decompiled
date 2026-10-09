#include <xtl.h>
#include "unknown_122870.h"

/* The native bootstrap owns one validated mainmenu.map and no asynchronous
 * Xbox cache-file handles.  These narrow services preserve the recovered map
 * loader while making that ownership explicit. */
long cache_file_get_maximum_size(long)
{
    return 0x20800000;
}

long cache_file_find(char const *)
{
    return 0;
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
    if (!map_name || !cache_files_load_map(map_name))
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
