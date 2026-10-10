// Source-local native reconstruction of the compact vertex-register mapping
// initializer used by retail particle sprite setup. Packet submission and the
// mutable register table remain renderer-owned and dormant.

#include "unknown_11c920.h"

extern long g_470a3c[8];

// @retail 0x23ecd0
void function_23ecd0(void)
{
    for (long index = 0; index < 8; ++index)
        g_470a3c[index] = index + 1;
}
