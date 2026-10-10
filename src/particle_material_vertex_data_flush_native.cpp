// Source-local native reconstruction of the material immediate vertex-data
// dirty flush used by function_16b90. Final submission remains a genuine D3D
// boundary.

#include "unknown_11c920.h"

struct s_18d70_state
{
    byte unknown00[0x1530];
    real values[16][4];
    byte unknown1630[0x100];
    dword changed;
};
static_assert(__builtin_offsetof(s_18d70_state, values) == 0x1530,
    "retail material vertex-data offset");
static_assert(__builtin_offsetof(s_18d70_state, changed) == 0x1730,
    "retail material vertex-data dirty mask offset");

extern "C" void __stdcall D3DDevice_SetVertexData4f(
    int register_index, real x, real y, real z, real w);

// @retail 0x18d70
void function_18d70(s_18d70_state *state)
{
    dword remaining = state->changed;
    while (remaining)
    {
        long index = __builtin_ctz(remaining);
        D3DDevice_SetVertexData4f(index, state->values[index][0], state->values[index][1],
            state->values[index][2], state->values[index][3]);
        remaining &= ~(1u << index);
        state->changed &= ~(1u << index);
    }
}
