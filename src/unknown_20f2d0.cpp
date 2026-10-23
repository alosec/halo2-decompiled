// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1fb7e0.h"
#include <string.h>

struct s_20d0c0
{
    short field_0;
    short field_2;
    long field_4;
    s_1fb7e0_data field_8;
};
struct s_node_owner;
struct s_audio_queue;
extern s_audio_queue *g_4f939c;
long __declspec(noinline) function_20f040(short arg_0);
void sound_choose_permutation(long arg_0, s_sound_permutation_reference *arg_1, bool *arg_2);
bool function_20b450(long arg_0, real arg_1);
short function_20afb0(long arg_0, dword *arg_1);
bool function_20b050(long arg_0, dword arg_1);
short function_1145f0(long arg_0, s_sound_permutation_reference const *arg_1, short arg_2);
long function_114440(long arg_0);
bool function_20f6a0(s_node_owner *arg_0, long arg_1);

PRIVATE __forceinline long function_20f2d1(real arg_0)
{
    long local_0;
    __asm
    {
        fld arg_0
        fistp local_0
    }
    return local_0;
}

// @retail 0x20f2d0
long function_20f2d0(short arg_1, long arg_0, long arg_2, long arg_3, long arg_4, s_20d0c0 const *arg_5)
{
    (void)&arg_2;
    (void)&arg_3;
    (void)&arg_4;
    (void)&arg_5;
    long local_0 = record_pool_allocate(g_4f9398);
    if (local_0 != NONE)
    {
        byte *local_1 = NULL;
        byte *local_2 = (byte *)g_4e034c;
        if (local_2 && *(long *)(local_2 + 0xc8) > 0)
        {
            long local_3 = *(long *)(*(byte **)(local_2 + 0xcc) + 0x64);
            if (local_3 != NONE)
                local_1 = g_4e3b44[local_3 & 0xffff].bytes;
        }
        byte *local_4 = g_4f9398->data + (local_0 & 0xffff) * 0x54;
        byte *local_5 = *(byte **)(local_1 + 4) + arg_1 * 0x60;
        byte *local_6 = *(byte **)(g_4e0300->data + (arg_2 & 0xffff) * 12 + 8);
        *(long *)(local_4 + 0x50) = NONE;
        *(long *)(local_4 + 4) = arg_2;
        *(short *)(local_4 + 2) = arg_1;
        *(long *)(local_4 + 8) = arg_0;
        local_4[0xe] = false;
        bool local_7 = false;
        short local_8 = (short)function_20f040(*(short *)(local_6 + 0x138));
        *(short *)(local_4 + 0xc) = local_8;
        if (arg_4 != NONE)
            local_4[0xe] = *(short *)(g_4f9398->data + (arg_4 & 0xffff) * 0x54 + 0xc) == local_8;
        local_4[0x40] = false;
        local_4[0x41] = false;
        *(long *)(local_4 + 0x44) = arg_4;
        local_4[0x48] = false;
        *(short *)(local_4 + 0x22) = *(short *)(local_5 + 0xa);
        s_sound_permutation_reference *local_9 = (s_sound_permutation_reference *)(local_4 + 0x20);
        sound_choose_permutation(arg_0, local_9, &local_7);
        if (local_7)
        {
            dword local_10 = 0;
            function_20b450(arg_0, *(real *)(local_5 + 0x1c) * 60.0f);
            short local_11 = function_20afb0(arg_0, &local_10);
            if (((~local_10) & ((1 << local_11) - 1)) && *(short *)(local_5 + 0xa) < 10)
            {
                function_20b050(arg_0, 0);
                goto local_19;
            }
        }
        *(long *)(local_4 + 0x10) = arg_3 + function_20f2d1(g_510c54->field_2_3 * *(real *)(local_5 + 0x24));
        *(long *)(local_4 + 0x14) = *(long *)(local_4 + 0x10);
        *(short *)(local_4 + 0x18) = (short)function_20f2d1(g_510c54->field_2_3 * *(real *)(local_5 + 0x28));
        *(short *)(local_4 + 0x1a) = (short)function_20f2d1(g_510c54->field_2_3 * *(real *)(local_5 + 0x2c));
        *(short *)(local_4 + 0x1e) = (short)function_20f2d1(g_510c54->field_2_3 * *(real *)(local_5 + 0x20));
        *(short *)(local_4 + 0x1c) = function_1145f0(*(long *)(local_4 + 8), local_9, *(short *)(local_4 + 0x22)) + *(short *)(local_4 + 0x1a);
        *(s_20d0c0 *)(local_4 + 0x24) = *arg_5;
        if (arg_4 != NONE && !local_4[0xe])
            *(short *)(local_4 + 0x1e) = (short)function_20f2d1(g_510c54->field_2_3 * 0.25f);
        *(long *)(local_4 + 0x4c) = function_114440(arg_2);
        short local_14 = *(short *)(local_6 + 0x138);
        s_node_owner *local_13 = (s_node_owner *)((byte *)g_4f939c + (short)function_20f040(local_14) * 0x7dc);
        if (function_20f6a0(local_13, local_0))
        {
            goto local_20;
        }
local_19:
        record_pool_release(g_4f9398, local_0);
        local_0 = NONE;
    }
local_20:
    return local_0;
}
