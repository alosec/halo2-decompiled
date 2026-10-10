// Source-local native reconstruction of material texture residency refresh.
// The Xbox body remains in unknown_020560.cpp.

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

extern bool g_4ba019;
bool function_143c0(long tag, short bitmap, short stage, real priority);

PRIVATE __forceinline long material_texture_floor(real value)
{
    long result = (long)value;
    if (value < 0.0f && value != (real)result)
        --result;
    return result;
}

// @retail 0x1b0a0
void function_1b0a0(byte *state)
{
    dword remaining = *(dword *)(state + 0x1420);
    while (remaining)
    {
        long index = (long)__builtin_ctzl(remaining);
        byte *entry = state + 0x1320 + index * 0x40;
        long tag = *(long *)entry;
        if (tag != NONE)
        {
            long bitmap = 0;
            real time = *(real *)(entry + 0x18);
            if (fabs(time) >= 0.0001f)
            {
                long count = *(long *)(g_4e3b44[tag & 0xffff].bytes + 0x44);
                bitmap = material_texture_floor((real)fmod(time, count));
            }
            real bias = 0.0f;
            if (!(entry[4] & 1) && !g_4ba019)
            {
                bias = *(real *)(entry + 0x14) + *(real *)(state + 0x2c) - 0.5f;
                if (bias < 0.0f) bias = 0.0f;
                else if (bias > 4.0f) bias = 4.0f;
            }
            real level = (real)material_texture_floor(bias);
            if (level < 0.0f) level = 0.0f;
            else if (level > 2.0f) level = 2.0f;
            level = (real)(long)level;
            if (*(real *)(entry + 0x10) > level)
            {
                function_143c0(tag, (short)bitmap, (short)index, bias);
                *(real *)(entry + 0x10) = level;
            }
        }
        remaining &= ~(1u << index);
    }
}
