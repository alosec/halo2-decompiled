// Source-local native reconstruction of packed material parameter expansion.
// The Xbox bodies remain in unknown_0167a0.cpp.

#include "unknown_11c920.h"
#include <math.h>

// Keep the recovered valid-case control flow explicit. Disabling optimization
// for this leaf prevents a compiler-generated switch table from masquerading
// as another material provider in selectively extracted objects.
// @retail 0x1b2c0
__attribute__((optnone))
bool function_1b2c0(
    byte const *state, long index, long component, long texture, long mode, real *out)
{
    if (component < 4)
    {
        real value = ((real const *)(state + 0x5c))[index];
        if (mode == 1) value = 1.0f - value;
        else if (mode == 2)
        {
            value = (real)((cos(value * 6.2831854820251465f) + 1.0f) * 0.5f);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
        }
        else if (mode == 3)
        {
            value = (real)(0.5f - cos(value * 6.2831854820251465f) * 0.5f);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
        }
        else if (mode == 4)
        {
            value = (real)((sin(value * 6.2831854820251465f) + 1.0f) * 0.5f);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
        }
        else if (mode == 5)
        {
            value = (real)(0.5f - sin(value * 6.2831854820251465f) * 0.5f);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
        }
        out[component] = value;
    }
    else if (component == 4)
    {
        real const *color = (real const *)(state + 0x108) + index * 3;
        out[0] = color[0];
        out[1] = color[1];
        out[2] = color[2];
    }
    else
    {
        real width = 1.0f;
        real height = 1.0f;
        if (texture)
        {
            width = (real)*(long const *)(state + 0x12e8 + texture * 64);
            height = (real)*(long const *)(state + 0x12ec + texture * 64);
        }
        if (component >= 5 && component <= 8) out[component - 5] = width;
        else if (component == 9) { out[0] = width; out[1] = height; }
        else if (component == 10) { out[2] = width; out[3] = height; }
        else if (component == 11) out[0] = out[1] = 0.0f;
        else if (component == 12) out[2] = out[3] = 0.0f;
        else if (component == 13) { out[0] = width; out[1] = height; out[2] = out[3] = 0.0f; }
        else if (component == 14 || component == 18) { out[0] = width; out[1] = out[2] = out[3] = 0.0f; }
        else if (component == 15 || component == 19) { out[1] = height; out[0] = out[2] = out[3] = 0.0f; }
        else if (component == 16) { out[0] = width; out[1] = height; out[2] = 1.0f; }
        else if (component == 17) out[0] = out[1] = out[2] = 0.0f;
        else if (component == 20) out[0] = out[1] = out[2] = out[3] = 0.0f;
    }
    return true;
}

// @retail 0x19be0
void __stdcall function_19be0(byte *state, word const *range)
{
    real *values[2] = { (real *)(state + 0x1530), (real *)(state + 0x1630) };
    dword *masks[2] = { (dword *)(state + 0x1730), (dword *)(state + 0x1734) };
    byte const *entry = *(byte **)(*(byte **)(state + 0x20) + 0x3c)
        + (*range & 0x01ff) * 4;
    for (long i = 0; i < (*range >> 9); ++i, entry += 4)
    {
        word packed = *(word const *)entry;
        function_1b2c0(state, entry[3], (packed >> 5) & 31, packed >> 13, 0,
            values[(packed >> 4) & 1] + (packed & 15) * 4);
        packed = *(word const *)entry;
        *masks[(packed >> 4) & 1] |= 1u << (packed & 15);
    }
}
