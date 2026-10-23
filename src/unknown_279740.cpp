// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_122870.h"


#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

PRIVATE inline long loaded_size_279740()
{
    if (cache_file_globals.loaded)
        return cache_file_globals.header.unknown1c;
    return 0;
}

// @retail 0x279740
bool __cdecl function_279740(long value)
{
    long bounded = value < (long)0x80061000 ? (long)0x80061000 :
        value > (long)0x80061000 + loaded_size_279740() ?
        (long)0x80061000 + loaded_size_279740() : value;
    return bounded == value;
}
