// Source-local native reconstruction of retail function_1cf50. The matching
// Xbox definition remains in unknown_01cf50.cpp beside unrelated x86 and D3D
// implementation detail. This provider remains dormant and unlinked.

#include "unknown_11c920.h"
#include <xtl.h>

extern void *g_51f3c8[2][4];

// @retail 0x1cf50
void function_1cf50(void)
{
    void **wanted = g_51f3c8[1];
    for (long stage = 0; stage < 4; ++stage, ++wanted)
    {
        void **current = wanted - 4;
        if (*current != *wanted)
        {
            D3DDevice_SetTexture(stage, *wanted);
            *current = *wanted;
        }
    }
}
