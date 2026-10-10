// Source-local native reconstruction of material policy-state selection.
// The Xbox body remains in unknown_03f2b0.cpp.

#include "unknown_11c920.h"

extern byte *g_4858c4;
extern long g_485a2c;
extern long g_485a64;
extern long g_485a68;
extern dword g_4ba014;

// @retail 0x34060
__attribute__((optnone))
long function_34060(long mode, dword flags)
{
    if (mode == 0 || mode == 1)
        return g_485a2c;
    if (mode == 2)
        return *(short *)(g_4858c4 + 0x8a);
    if (mode == 3)
        return *(short *)(g_4858c4 + 0x80);
    if (mode == 4)
        return g_485a64;
    if (mode == 5)
        return g_485a68;
    if (mode == 6)
    {
        if (!(*g_4858c4 & 2) && ((g_4ba014 >> 1) & 1) && (flags & 0x2000))
            return 1;
        return 0;
    }
    if (mode == 7)
        return !((*(dword *)g_4858c4 >> 1) & 1);
    __assume(0);
}
