// Native source split of recovered particle interface-state selection at retail
// 0x1bd50. The matching Xbox definition remains in unknown_0167a0.cpp,
// whose unrelated renderer paths use x86 SSE/MMX, inline assembly, and D3D.

#include "unknown_11c920.h"
#include <stddef.h>

struct input_bit_vectors
{
    dword v58, v5c, v60, v64, v68;
    dword v6c[2], v74[2], v7c[2], v84[2], v8c[2], v94[2];
    dword v9c[2], va4[2], vac[2], vb4[2], vbc[2];
};

struct s_33a0b_view;
extern input_bit_vectors g_485058;
extern s_33a0b_view *g_485a58;
extern dword g_485b78[5];

static_assert(offsetof(input_bit_vectors, v58) == 0, "input flags v58 offset");
static_assert(offsetof(input_bit_vectors, v60) == 8, "input flags v60 offset");
static_assert(offsetof(input_bit_vectors, v68) == 0x10, "input flags v68 offset");
static_assert(offsetof(input_bit_vectors, v7c) == 0x24, "input flags v7c offset");
static_assert(offsetof(input_bit_vectors, v8c) == 0x34, "input flags v8c offset");
static_assert(offsetof(input_bit_vectors, vac) == 0x54, "input flags vac offset");
static_assert(offsetof(input_bit_vectors, vbc) == 0x64, "input flags vbc offset");
static_assert(sizeof(input_bit_vectors) == 0x6c, "input flags layout");

// @retail 0x1bd50
void function_1bd50(void *state)
{
    if (g_485a58 != state || !state)
    {
        g_485a58 = (s_33a0b_view *)state;
        g_485b78[0] |= g_485058.v8c[0];
        g_485b78[1] |= g_485058.v8c[1];
        g_485b78[2] |= g_485058.vbc[0];
        g_485b78[3] |= g_485058.vbc[1];
        g_485b78[4] |= g_485058.v68;
        g_485b78[0] |= g_485058.v7c[0];
        g_485b78[1] |= g_485058.v7c[1];
        g_485b78[2] |= g_485058.vac[0];
        g_485b78[3] |= g_485058.vac[1];
        g_485b78[4] |= g_485058.v60;
    }
}
