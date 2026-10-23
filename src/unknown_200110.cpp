#include "unknown_11c920.h"
#include "globals.h"
#include "props.h"
#include "object_markers.h"
#include "unknown_20fe20.h"

// @flags /O2 /arch:SSE /Gr

void __stdcall function_26c2d0(long arg_0);
long function_baf80(long arg_0);

PRIVATE __forceinline bool function_200111(s_prop_datum *arg_0, long arg_1, s_object_marker *arg_2)
{
    long local_0 = function_baf80(*(long *)((byte *)arg_0 + 0x20));
    return function_b8d30(local_0, arg_1, arg_2, 1, false);
}

// @retail 0x200110
bool function_200110(long arg_0, long arg_1, point3f *arg_2, s_type_c3b527 *arg_3)
{
    point3f *const *local_0 = &arg_2;
    s_type_c3b527 *const *local_1 = &arg_3;
    byte *local_2 = g_4f55f0->data + (arg_0 & 0xffff) * 0x888;
    s_prop_datum *local_3 = prop_ref_get(arg_1);
    byte *local_4 = (byte *)function_25d690(local_3);
    short local_5 = NONE;
    if (*local_1)
    {
        function_26c2d0(arg_1);
        local_5 = *(short *)(local_4 + 0x54);
    }
    point3f local_6;
    s_object_marker local_7;
    bool local_8;
    if (local_2[0x340] && arg_1 == *(long *)(local_2 + 0x338) &&
        *(long *)(local_2 + 0x33c) &&
        function_200111(local_3, *(long *)(local_2 + 0x33c), &local_7))
    {
        local_6 = local_7.matrix.position;
        local_8 = true;
    }
    else
    {
        local_6 = *(point3f *)((byte *)function_25d690(local_3) + 0x10);
        local_8 = true;
    }
    if (*local_0)
        **local_0 = local_6;
    if (*local_1)
    {
        local_8 = false;
        if (function_210690((short)local_5, &local_6, &(*local_1)->point))
        {
            (*local_1)->output_index = (short)local_5;
            local_8 = true;
        }
    }
    return local_8;
}
