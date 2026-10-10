// Source-local native reconstruction of the material shader-constant dirty
// flush used by function_16b90. Final submission remains a genuine D3D call.

#include "unknown_11c920.h"

struct s_shader_constant_state
{
    byte unknown00[0x1630];
    real constants[16][4];
    long unknown1730;
    dword changed;
};
static_assert(__builtin_offsetof(s_shader_constant_state, constants) == 0x1630,
    "retail material shader constants offset");
static_assert(__builtin_offsetof(s_shader_constant_state, changed) == 0x1734,
    "retail material shader dirty mask offset");

extern "C" void __stdcall D3DDevice_SetVertexShaderConstantFast(
    int register_index, void const *constant_data, dword constant_count);

// @retail 0x18e80
void function_18e80(s_shader_constant_state *state)
{
    dword remaining = state->changed;
    while (remaining)
    {
        long index = __builtin_ctz(remaining);
        D3DDevice_SetVertexShaderConstantFast(index - 78, state->constants[index], 1);
        remaining &= ~(1u << index);
        state->changed &= ~(1u << index);
    }
}
