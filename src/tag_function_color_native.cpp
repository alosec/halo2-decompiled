// Native source split of recovered packed tag-function colour conversion at
// retail 0x13bc00/0x13be80. The matching Xbox definitions remain in
// unknown_13b390.cpp, whose implementation uses x86 MMX/SSE and inline
// assembly. This scalar path preserves its nearest-even conversion and signed
// high-word interpolation exactly.

#include "unknown_11c920.h"
#include "effects.h"
#include <stddef.h>

struct s_tag_data
{
    long size;
    byte *address;
};

struct s_function_color_header
{
    byte type;
    byte flags;
    byte function_type[2];
    dword colors[4];
};

static_assert(sizeof(s_tag_data) == 8, "tag data layout");
static_assert(sizeof(s_function_color_header) == 20, "function color header layout");
static_assert(offsetof(s_function_color_header, colors) == 4, "function colors offset");

color3f *unpack_color3f(dword pixel, color3f *color);
extern color4f const *g_4686cc;

static inline long round_nearest_even_nonnegative(real value)
{
    long result = (long)value;
    double const fraction = (double)value - (double)result;
    if (fraction > 0.5 || (fraction == 0.5 && (result & 1)))
        ++result;
    return result;
}

static inline long floor_divide(long value, long divisor)
{
    long result = value / divisor;
    if (value < 0 && value % divisor)
        --result;
    return result;
}

static inline byte interpolate_channel(byte a, byte b, long factor)
{
    long const shifted_difference = ((long)b - (long)a) * 8;
    long const high_word = floor_divide(shifted_difference * factor, 65536);
    long value = (long)a + floor_divide(high_word + 1, 2);
    if (value < 0)
        value = 0;
    else if (value > 255)
        value = 255;
    return (byte)value;
}

static dword pixel32_interpolate_native(dword a, dword b, real t)
{
    long const factor = round_nearest_even_nonnegative(t * 16384.0f);
    dword result = 0;
    for (long channel = 0; channel < 4; ++channel)
    {
        byte const value = interpolate_channel(
            (byte)(a >> (channel * 8)), (byte)(b >> (channel * 8)), factor);
        result |= (dword)value << (channel * 8);
    }
    return result;
}

static inline long floor_to_long(real value)
{
    // Every caller clamps to a nonnegative range before conversion.
    return (long)value;
}

enum
{
    function_constant = 1,
    output_scalar = 0,
    output_color_constant,
    output_color_2,
    output_color_3,
    output_color_4
};

// @retail 0x13bc00
dword function_13bc00(s_tag_data const *function, real input)
{
    dword result = 0xffffffff;
    s_function_color_header const *header =
        function ? (s_function_color_header const *)function->address : 0;
    if (!header || function->size <= 0)
        return result;

    if (header->type == function_constant &&
        (!(header->flags & 1) || (header->flags & 0xf0) <= 0x10))
        return header->colors[0];

    input = 0.0f > input ? 0.0f : (input > 1.0f ? 1.0f : input);
    switch (header->flags >> 4)
    {
    case output_scalar:
    {
        byte const intensity = (byte)round_nearest_even_nonnegative(input * 255.0f);
        return 0xff000000 | ((dword)intensity << 16) |
            ((dword)intensity << 8) | intensity;
    }
    case output_color_constant:
        return header->colors[0];
    case output_color_2:
        return pixel32_interpolate_native(header->colors[0], header->colors[3], input);
    case output_color_3:
    {
        input *= 2.0f;
        long const index = floor_to_long(input);
        input -= (real)index;
        if (index >= 2)
            input = 1.0f;
        dword const a = index == 0 ? header->colors[0] : header->colors[1];
        dword const b = index == 0 ? header->colors[1] : header->colors[3];
        return pixel32_interpolate_native(a, b, input);
    }
    case output_color_4:
    {
        input *= 3.0f;
        long index = floor_to_long(input);
        input -= (real)index;
        if (index >= 3)
        {
            input = 1.0f;
            index = 2;
        }
        return pixel32_interpolate_native(header->colors[index], header->colors[index + 1], input);
    }
    default:
        return result;
    }
}

// @retail 0x13be80
void function_13be80(s_tag_data const *function, real input, color3f *color)
{
    if (function->address && function->size > 0)
        unpack_color3f(function_13bc00(function, input), color);
    else
        *color = *(color3f const *)&g_4686cc->red;
}
