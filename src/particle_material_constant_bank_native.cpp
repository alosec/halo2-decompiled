// Source-local native reconstruction of the packed material constant-bank
// loader used by function_16b90. The matching Xbox body remains in
// unknown_0167a0.cpp.

#include "unknown_11c920.h"

// @retail 0x17000
void function_17000(byte *context, word const *range)
{
    dword *banks[2] = {
        (dword *)(context + 0x1530),
        (dword *)(context + 0x1630),
    };
    dword *masks[2] = {
        (dword *)(context + 0x1730),
        (dword *)(context + 0x1734),
    };
    byte *definition = *(byte **)(context + 0x0c);
    byte *instance = *(byte **)(context + 0x10);
    dword *values = *(dword **)(definition + 0x18);
    byte *record = *(byte **)(instance + 0x24) + (*range & 0x1ff) * 4;
    for (long i = 0; i < (*range >> 9); ++i, record += 4)
    {
        word packed = *(word *)record;
        long bank = (packed >> 4) & 1;
        long slot = packed & 15;
        dword *out = banks[bank] + slot * 4;
        *masks[bank] |= 1u << slot;
        dword *source = values + record[3] * 4;
        dword type = (packed >> 5) & 31;

        // These masks are the exact component writes represented by retail's
        // valid type cases, expressed without a compiler-generated switch table.
        if ((0x001fea31u >> type) & 1u) out[0] = source[0];
        if ((0x001fea52u >> type) & 1u) out[1] = source[1];
        if ((0x001f3494u >> type) & 1u) out[2] = source[2];
        if ((0x001cf508u >> type) & 1u) out[3] = source[3];
    }
}
