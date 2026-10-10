// Source-local native reconstruction of the compact material-value loader used
// by function_16b90. The matching Xbox body remains in unknown_020560.cpp.

#include "unknown_11c920.h"

extern byte *g_485a80;

// @retail 0x16f60
void function_16f60(byte *state, word const *range)
{
    byte *values = *(byte **)(*(byte **)(state + 0x0c) + 8);
    byte *entry = *(byte **)(*(byte **)(state + 0x10) + 0x24)
        + (*range & 0x1ff) * 4;
    for (long i = 0; i < (*range >> 9); ++i, entry += 4)
    {
        byte *slot = state + 0x320 + entry[0] * 0x40;
        long *source = (long *)(values + entry[3] * 12);
        if (source[0] != -1)
        {
            *(long *)slot = source[0];
            *(real *)(slot + 0x18) = (real)source[1];
            *(long *)(slot + 0x14) = source[2];
        }
        else
        {
            *(long *)slot = *(long *)(g_485a80 + 0x64);
            *(real *)(slot + 0x18) = 0.0f;
            *(real *)(slot + 0x14) = 0.0f;
        }
        *(real *)(slot + 0x10) = 3.0f;
    }
}
