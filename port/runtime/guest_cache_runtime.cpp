#include <xtl.h>

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

extern "C" void __stdcall GetSystemTimeAsFileTime(FILETIME *time)
{
    if (time)
    {
        time->dwLowDateTime = 0;
        time->dwHighDateTime = 0;
    }
}
