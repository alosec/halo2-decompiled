#include "unknown_11c920.h"
#include "ai_actor.h"

// @flags /O2 /arch:SSE /Gr

struct s_follow_search_state;
struct s_1aff10
{
    short field_0;
    byte field_2;
    bool field_3;
    s_reference field_4;
    bool field_8;
    bool field_9;
    bool field_a;
    byte field_b[5];
    long field_10;
};

bool function_1b02a0(long arg_0);
short __stdcall function_1afde0(long arg_0);
bool function_1b0400(long arg_0, s_follow_search_state *arg_1);
void function_1b0540(long arg_0, s_follow_search_state *arg_1);

#pragma inline_depth(0)
// @retail 0x1aff10
long __stdcall function_1aff10(long arg_0, s_slot *arg_1)
{
    long local_2 = 1;
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    s_1aff10 *local_1 = (s_1aff10 *)((byte *)arg_1 + 0xc);
    if (local_1->field_10 == NONE && local_0->unknown344 == NONE)
        goto local_3;
    if (REFERENCE_EQUAL(local_1->field_4, g_470fa0))
    {
        local_1->field_3 = true;
        goto local_4;
    }
    if (REFERENCE_EQUAL(local_0->unknown418, g_470fa0))
    {
        local_1->field_4 = g_470fa0;
        local_1->field_3 = true;
        goto local_4;
    }
    if (function_1b02a0(arg_0))
    {
        if (local_1->field_0 != 0)
        {
            local_1->field_3 = true;
            goto local_4;
        }
        if (function_1afde0(arg_0) > 0)
        {
            local_1->field_3 = true;
            goto local_4;
        }
        local_1->field_4 = local_0->unknown418;
        local_1->field_8 = local_0->unknown3f0;
        local_1->field_a = true;
        local_1->field_3 = false;
    }
local_4:
    if (local_0->unknown040 && !local_1->field_a)
    {
        if (!REFERENCE_EQUAL(local_1->field_4, g_470fa0) && local_1->field_0 == 0 &&
            function_1b0400(arg_0, (s_follow_search_state *)local_1))
        {
            local_1->field_4 = g_470fa0;
            local_1->field_3 = true;
        }
        if (local_1->field_3)
        {
            function_1b0540(arg_0, (s_follow_search_state *)local_1);
            if (REFERENCE_EQUAL(local_1->field_4, g_470fa0))
                local_1->field_9 = true;
        }
    }
    if (!local_1->field_9)
        return local_2;
local_3:
    return 0;
}
#pragma inline_depth(255)
