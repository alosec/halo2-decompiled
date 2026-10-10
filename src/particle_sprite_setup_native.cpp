// Source-local native reconstruction of retail function_23ea80. This keeps
// the recovered tag/shader selection and Xbox renderer calls visible for
// closure scoring without parsing unrelated D3D texture and push-buffer code
// from unknown_23d970.cpp. It is dormant and does not establish renderer state.

#include "unknown_11c920.h"
#include "globals.h"
#include "effects.h"

struct s_shader_cache;

void function_1bbf0(long, long, long, long, long, real);
void function_1cf50(void);
void function_1c590(s_shader_cache *, long, long);
void function_1c6b0(void *);
void __stdcall function_1c710(void *);
void function_1ccb0(long);
void function_23e460(void);
void function_23ecd0(void);

extern "C" void __stdcall D3DDevice_SetRenderState(dword state, dword value);
extern "C" void __stdcall D3DDevice_SetVertexShaderConstant(
    int register_index, void const *constant_data, dword constant_count);

extern byte g_470a38;
extern byte g_470a2c;
extern byte g_470a39;
extern dword g_4b8348;
extern byte g_51f0f0[0x2d8];

PRIVATE byte const g_native_particle_shader_format[20] =
{
    0x2a, 0x00, 0x03, 0x01, 0x02, 0x02, 0x00, 0x03, 0x03, 0x04,
    0x02, 0x05, 0x01, 0x06, 0x03, 0x07, 0x03, 0x09, 0x11, 0xff
};

// @retail 0x23ea80
bool function_23ea80(long tag, transform4x3f const *matrix, long stage,
    long pass, long variant)
{
    bool result = false;
    dword kind = ((dword *)g_4e3b44)[(short)tag * 4];
    long const *reference;
    if (kind == 0x5052544d || kind == 0x70727433)
        reference = function_137bd0(tag)->function_x947334();
    else
        reference = *(long const **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
    byte *groups = *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
    long first = **(word **)(groups + 4) & 0x1ff;
    long second = (*(word **)(groups + 0xc))[first + stage] & 0x1ff;
    long shader_tag = *(long *)(*(byte **)(groups + 0x14) + (second + pass) * 10 + 4);
    byte *shader = *(byte **)(*(byte **)(g_4e3b44[shader_tag & 0xffff].bytes + 0x20) + 4);

    g_470a2c = 1;
    g_4b8348 = 0;
    // The retail XDK intrinsic lowers this state to method 0x4147c.
    D3DDevice_SetRenderState(0x4147c, 0);

    real constants[12];
    constants[0] = matrix->forward.i;
    constants[1] = matrix->left.i;
    constants[2] = matrix->up.i;
    constants[3] = matrix->position.x;
    constants[4] = matrix->forward.j;
    constants[5] = matrix->left.j;
    constants[6] = matrix->up.j;
    constants[7] = matrix->position.y;
    constants[8] = matrix->forward.k;
    constants[9] = matrix->left.k;
    constants[10] = matrix->up.k;
    constants[11] = matrix->position.z;
    D3DDevice_SetVertexShaderConstant(78, constants, 3);

    function_1bbf0(tag, 0, pass, variant, 0, 10000.0f);
    function_1cf50();
    long binding_tag = *(long *)(shader + 0x100);
    long entry = stage + 7;
    if (binding_tag != NONE)
    {
        byte *bindings = g_4e3b44[binding_tag & 0xffff].bytes;
        if ((dword)entry < *(dword *)(bindings + 4) &&
            (dword)(*(long *)(*(byte **)(bindings + 8) + entry * 0x1c + 0xc) >> 4) > 0)
        {
            result = true;
            function_1c590((s_shader_cache *)g_51f0f0, binding_tag, entry);
            if (g_470a38 && g_470a39)
                function_1ccb0(43);
            else
            {
                function_1c6b0(g_51f0f0);
                *(byte const **)(g_51f0f0 + 0x8c) = g_native_particle_shader_format;
                if (*(byte const **)(g_51f0f0 + 0xcc) != g_native_particle_shader_format)
                    g_51f0f0[0x20c] = 1;
                function_1c710(g_51f0f0);
            }
            function_1c710(g_51f0f0);
            function_23e460();
            function_23ecd0();
        }
    }
    return result;
}
