// Source-local native reconstruction of compact bitmap mip helpers.
// The Xbox bodies remain in unknown_0359e0.cpp and unknown_13e210.cpp.

#include "unknown_11c920.h"

// @retail 0x13e3a0
long log2_floor(dword value)
{
    long result = 0;
    if (value > 0)
    {
        while (value != 1)
        {
            value >>= 1;
            ++result;
        }
    }
    return result;
}

PRIVATE __forceinline bool texture_is_power_of_two(short value)
{
    return value > 0 && !(value & (value - 1));
}

PRIVATE __forceinline long texture_maximum(long first, long second)
{
    return first > second ? first : second;
}

// @retail 0x359e0
short bitmap_get_mipmap_count(short width, short height, short depth,
    short format, bool linear, short maximum_levels)
{
    short result = 0;
    if (texture_is_power_of_two(width) && texture_is_power_of_two(height) &&
        texture_is_power_of_two(depth))
    {
        if (linear)
            result = 0;
        else if (format >= 14 && format <= 16)
            result = (short)log2_floor((dword)texture_maximum(width / 4,
                texture_maximum(height / 4, depth)));
        else
            result = (short)log2_floor((dword)texture_maximum(width,
                texture_maximum(height, depth)));
    }
    return maximum_levels > result ? result : maximum_levels;
}
