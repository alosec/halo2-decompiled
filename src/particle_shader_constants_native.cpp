// Source-local native reconstruction of retail particle shader constants.
// The fixed trigonometric ring and mutable camera basis are preserved; final
// vertex-constant submission remains a genuine D3D boundary.

#include "unknown_11c920.h"
#include <math.h>

extern byte g_485af0;
extern real g_4856c4[9];
extern "C" void __stdcall D3DDevice_SetVertexShaderConstant(
    int register_index, void const *constant_data, dword constant_count);

// @retail 0x23e460
void function_23e460(void)
{
    if (!g_485af0)
        return;

    real constants[22][4] =
    {
        {53.0f, 15.0f, 0.0f, 0.15915494f},
        {(real)sin(0.0f), (real)cos(0.0f), 0.0f, 0.0f},
        {(real)sin(0.41887903f), (real)cos(0.41887903f), 0.0f, 0.0f},
        {(real)sin(0.83775806f), (real)cos(0.83775806f), 0.0f, 0.0f},
        {(real)sin(1.2566371f), (real)cos(1.2566371f), 0.0f, 0.0f},
        {(real)sin(1.6755161f), (real)cos(1.6755161f), 0.0f, 0.0f},
        {(real)sin(2.0943952f), (real)cos(2.0943952f), 0.0f, 0.0f},
        {(real)sin(2.5132742f), (real)cos(2.5132742f), 0.0f, 0.0f},
        {(real)sin(2.9321532f), (real)cos(2.9321532f), 0.0f, 0.0f},
        {(real)sin(3.3510323f), (real)cos(3.3510323f), 0.0f, 0.0f},
        {(real)sin(3.7699113f), (real)cos(3.7699113f), 0.0f, 0.0f},
        {(real)sin(4.1887903f), (real)cos(4.1887903f), 0.0f, 0.0f},
        {(real)sin(4.6076694f), (real)cos(4.6076694f), 0.0f, 0.0f},
        {(real)sin(5.0265484f), (real)cos(5.0265484f), 0.0f, 0.0f},
        {(real)sin(5.4454274f), (real)cos(5.4454274f), 0.0f, 0.0f},
        {(real)sin(5.8643064f), (real)cos(5.8643064f), 0.0f, 0.0f},
        {(real)sin(6.2831855f), (real)cos(6.2831855f), 0.0f, 0.0f},
        {g_4856c4[0], g_4856c4[3], g_4856c4[6], 0.0f},
        {g_4856c4[1], g_4856c4[4], g_4856c4[7], 0.0f},
        {g_4856c4[2], g_4856c4[5], g_4856c4[8], 0.0f},
        {1.0f, 0.0f, 0.99f, 42.0f},
        {2.0f, 3.051851e-05f, 1.0f, 42.0f}
    };

    for (long i = 1; i < 17; ++i)
    {
        long next = i == 16 ? 1 : i + 1;
        constants[i][2] = constants[next][0] - constants[i][0];
        constants[i][3] = constants[next][1] - constants[i][1];
    }

    D3DDevice_SetVertexShaderConstant(52, constants, 22);
    g_485af0 = 0;
}
