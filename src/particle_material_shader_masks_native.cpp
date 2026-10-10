// Source-local native reconstruction of the packed material shader-mask loader
// used by function_16b90. The matching Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"

// @retail 0x467068
extern const dword g_467068[8] = {
    0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000,
    0x00ffffff, 0x00ff0000, 0x0000ff00, 0x000000ff,
};

static __forceinline void store_material_shader_mask(
    byte *state, word packed, long index, dword mask, dword value)
{
    if ((packed & 0x03c0) != 0x0200)
    {
        dword offset = packed & 0x0400 ? 0x146c : 0x144c;
        dword *slot = (dword *)(state + offset + index * 4);
        *slot = (*slot & ~mask) | (value & mask);
        *(dword *)(state + (packed & 0x0400 ? 0x151c : 0x1518)) |= 1u << index;
    }
    else
    {
        dword *slot = (dword *)(state + (packed & 0x0400 ? 0x14d4 : 0x14d0));
        *slot = (*slot & ~mask) | (value & mask);
        *(dword *)(state + (packed & 0x0400 ? 0x151c : 0x1518)) |= 0x100;
    }
}

// @retail 0x17170
void function_17170(byte *state, word const *range)
{
    dword const *values = *(dword **)(*(byte **)(state + 0x0c) + 0x10);
    byte const *entry = *(byte **)(*(byte **)(state + 0x10) + 0x24)
        + (*range & 0x01ff) * 4;
    word remapping = *(word *)(*(byte **)(state + 0x24) + 0xf6);
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            word packed = *(word const *)entry;
            store_material_shader_mask(state, packed, (packed >> 6) & 15,
                g_467068[entry[2] & 7], values[entry[3]]);
        }
    }
    else
    {
        byte const *mapping = *(byte **)(*(byte **)(state + 0x20) + 0x24)
            + (remapping & 0x01ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            word packed = *(word const *)entry;
            long slot = packed & 7;
            short source = *(short const *)(mapping + slot * 4);
            if (source == -1 || ((packed >> 3) & 7) == *(long *)(state + 0x300 + source * 4))
            {
                long index = (packed >> 6) & 15;
                if ((packed & 0x03c0) != 0x0200)
                    index += *(signed char *)(state + 0x1520 + slot);
                store_material_shader_mask(state, packed, index,
                    g_467068[entry[2] & 7], values[entry[3]]);
            }
        }
    }
}
