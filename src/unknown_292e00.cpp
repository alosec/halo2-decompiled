#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "data_array.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

extern s_record_pool *g_51ecb4;
void __stdcall function_b8540(long);

struct s_292e00 { short field_0; short field_2; long field_4; byte field_8[0x20]; };
struct s_292e01 { byte field_0[0x134]; long field_134; byte field_138[2]; short field_13a; };
struct s_292e02 { byte field_0[8]; s_292e01 *field_8; };
struct s_292e03 { long field_0; long field_4; long field_8; };
struct s_292e04 { s_292e00 *field_0; s_record_pool_iterator field_4; long field_10; };

PRIVATE __forceinline s_292e03 *function_292ed3(s_292e01 *arg_0)
{
    return arg_0->field_134 == 1 ? (s_292e03 *)((byte *)arg_0 + arg_0->field_13a) : NULL;
}
PRIVATE __forceinline void function_292e19(s_292e04 *arg_0)
{
    if (g_4f55d0->active)
    {
        arg_0->field_4.data = g_51ecb4;
        arg_0->field_4.index = NONE;
    }
}
PRIVATE __forceinline s_292e00 *function_292e30(s_292e04 *arg_0)
{
    arg_0->field_0 = NULL;
    if (g_4f55d0->active)
    {
        arg_0->field_0 = (s_292e00 *)data_iterator_next_inlined(&arg_0->field_4);
        arg_0->field_10 = arg_0->field_4.datum_index;
    }
    return arg_0->field_0;
}
PRIVATE __forceinline void function_292ea2(long arg_0)
{
    bool done = false;
    while (!done)
    {
        long local_0 = arg_0;
        if (local_0 == NONE)
        {
            done = true;
        }
        else
        {
            s_292e01 *local_1 = ((s_292e02 *)g_4e0300->data)[arg_0 & 0xffff].field_8;
            s_292e03 *local_2 = function_292ed3(local_1);
            arg_0 = local_2 ? local_2->field_8 : NONE;
            local_2 = function_292ed3(local_1);
            if (local_2) local_2->field_0 = NONE;
            function_b8540(local_0);
        }
    }
}

// @retail 0x292e00
void function_292e00()
{
    s_292e04 local_0;
    function_292e19(&local_0);
    while (function_292e30(&local_0))
        function_292ea2(((s_292e00 *)g_51ecb4->data)[local_0.field_10 & 0xffff].field_4);
    g_51ecb4->valid = false;
}

// @retail 0x292f60
void function_292f60()
{
    s_292e04 local_0;
    function_292e19(&local_0);
    while (function_292e30(&local_0))
    {
        function_292ea2(((s_292e00 *)g_51ecb4->data)[local_0.field_10 & 0xffff].field_4);
        record_pool_release(g_51ecb4, local_0.field_10);
    }
}
