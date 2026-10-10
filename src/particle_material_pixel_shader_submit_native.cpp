// Source-local native reconstruction of the material pixel-shader terminal
// submission used by function_16b90. Final submission remains a genuine D3D
// boundary.

#include "unknown_11c920.h"

struct s_01b050_shader_state
{
    byte unknown00[0x1424];
    byte program[0xf0];
    bool changed;
};
static_assert(__builtin_offsetof(s_01b050_shader_state, program) == 0x1424,
    "retail material pixel shader program offset");
static_assert(__builtin_offsetof(s_01b050_shader_state, changed) == 0x1514,
    "retail material pixel shader dirty offset");

extern "C" void __stdcall D3DDevice_SetPixelShaderProgram(void const *program);

// @retail 0x1b050
void function_1b050(s_01b050_shader_state *state)
{
    D3DDevice_SetPixelShaderProgram(state->program);
    state->changed = false;
}
