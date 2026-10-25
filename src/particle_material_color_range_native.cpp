// Source-local native reconstruction of packed material color-range expansion.
// The Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"   /* color4f: alpha first, as pack_color4f reads it */

extern dword const g_467068[8];
bool function_1b2c0(
    byte const *state, long index, long component, long texture, long mode, real *out);
dword __cdecl pack_color4f(color4f const *color);

PRIVATE __forceinline void material_store_shader_mask(
    byte *state, word packed, long index, dword mask, dword value)
{
    if ((packed & 0x03c0) != 0x0200)
    {
        if (!(packed & 0x0400))
        {
            dword *destination = (dword *)(state + 0x144c + index * 4);
            *destination = (*destination & ~mask) | (value & mask);
            *(dword *)(state + 0x1518) |= 1u << index;
        }
        else
        {
            dword *destination = (dword *)(state + 0x146c + index * 4);
            *destination = (*destination & ~mask) | (value & mask);
            *(dword *)(state + 0x151c) |= 1u << index;
        }
    }
    else if (!(packed & 0x0400))
    {
        dword *destination = (dword *)(state + 0x14d0);
        *destination = (*destination & ~mask) | (value & mask);
        *(dword *)(state + 0x1518) |= 0x100;
    }
    else
    {
        dword *destination = (dword *)(state + 0x14d4);
        *destination = (*destination & ~mask) | (value & mask);
        *(dword *)(state + 0x151c) |= 0x100;
    }
}

PRIVATE __forceinline dword material_color_value(real const values[4])
{
    color4f color;
    color.red = values[0];
    color.green = values[1];
    color.blue = values[2];
    color.alpha = values[3];
    return pack_color4f(&color);
}

PRIVATE __forceinline void material_remap_and_clamp_color(real values[4], bool remap)
{
    for (long index = 0; index < 4; ++index)
    {
        if (remap)
            values[index] = (values[index] + 1.0f) * 0.5f;
        if (values[index] < 0.0f) values[index] = 0.0f;
        else if (values[index] > 1.0f) values[index] = 1.0f;
    }
}

// @retail 0x19ca0
void function_19ca0(byte *state, word const *range)
{
    byte const *definition = *(byte **)(state + 0x20);
    byte const *entry = *(byte **)(definition + 0x3c) + (*range & 0x01ff) * 4;
    word remapping = *(word *)(*(byte **)(state + 0x24) + 0xf6);
    real values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            dword mask = g_467068[entry[2] & 7];
            function_1b2c0(state, entry[3], entry[2] & 7, 0,
                (entry[2] >> 3) & 7, values);
            material_remap_and_clamp_color(values, (*(word const *)entry & 0x0800) != 0);
            word packed = *(word const *)entry;
            material_store_shader_mask(state, packed, (packed >> 6) & 15,
                mask, material_color_value(values));
        }
    }
    else
    {
        byte const *mapping = *(byte **)(definition + 0x24) +
            (remapping & 0x01ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            word packed = *(word const *)entry;
            long slot = packed & 7;
            short source = *(short const *)(mapping + slot * 4);
            if (source != NONE &&
                ((packed >> 3) & 7) != *(long *)(state + 0x300 + source * 4))
                continue;
            dword mask = g_467068[entry[2] & 7];
            function_1b2c0(state, entry[3], entry[2] & 7, 0,
                (entry[2] >> 3) & 7, values);
            material_remap_and_clamp_color(values, (*(word const *)entry & 0x0800) != 0);
            packed = *(word const *)entry;
            long index = (packed >> 6) & 15;
            if ((packed & 0x03c0) != 0x0200)
                index += *(signed char *)(state + 0x1520 + slot);
            material_store_shader_mask(state, packed, index, mask,
                material_color_value(values));
        }
    }
}
