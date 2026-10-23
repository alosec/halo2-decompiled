#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
// @flags /O2 /arch:SSE /Gr

struct s_1badc0
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0x5c - 0x26];
};

real normalize2d(point2f *arg_0);

// @retail 0x1badc0
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6)
{
    s_type_c3b527 *const *output_reference = &arg_3;
    arg_3 = *output_reference;
    s_actor_view *local_0 = actor_get(arg_0);
    if (arg_6)
        *arg_6 = false;
    if (!arg_5)
    {
        point3f local_1 = *arg_2;
        byte *local_2 = (byte *)object_get(arg_1);
        point3f local_3 = *(point3f *)(local_2 + 0x30);
        real local_4 = *(real *)(local_2 + 0x3c);
        vector3f local_5;
        vector3d_from_points3d(&local_0->position, arg_2, &local_5);
        vector3f local_6;
        vector3d_from_points3d(arg_2, &local_3, &local_6);
        real local_7 = local_5.j * local_5.j + local_5.i * local_5.i;
        if (local_7 > 0.0f)
        {
            real local_8 = ((local_3.y - local_0->position.y) * local_5.j +
                (local_3.x - local_0->position.x) * local_5.i) / local_7;
            if (local_8 > 0.0f && local_8 < 1.2f)
            {
                point2f local_9;
                local_9.x = 0.0f - local_5.j;
                local_9.y = local_5.i;
                if ((local_3.y - local_0->position.y) * local_5.i +
                    (local_3.x - local_0->position.x) * local_9.x > 0.0f)
                {
                    local_9.x = 0.0f - local_9.x;
                    local_9.y = 0.0f - local_9.y;
                }
                if (normalize2d(&local_9) > 0.0f)
                {
                    local_1.x = local_9.x * (local_4 * 1.1f) + local_3.x;
                    local_1.y = local_9.y * (local_4 * 1.1f) + local_3.y;
                    vector3f local_10;
                    vector3d_from_points3d(&local_0->position, &local_1, &local_10);
                    real local_11 = local_10.k * local_10.k + local_10.j * local_10.j + local_10.i * local_10.i;
                    if (local_11 > 0.0001f && local_11 < 4.0f)
                    {
                        local_11 = (real)sqrt(local_11);
                        vector3f local_12;
                        local_12.i = 0.0f - local_6.j;
                        local_12.j = local_6.i;
                        local_12.k = 0.0f;
                        if (0.0f > local_10.j * local_6.i + local_12.i * local_10.i)
                        {
                            local_12.i = 0.0f - local_12.i;
                            local_12.j = 0.0f - local_12.j;
                        }
                        if (normalize2d((point2f *)&local_12) > 0.0f)
                        {
                            real local_13 = 2.0f - local_11;
                            local_1.x += local_12.i * local_13;
                            local_1.y += local_12.j * local_13;
                            local_1.z += local_12.k * local_13;
                        }
                    }
                    if (arg_6)
                        *arg_6 = true;
                }
            }
        }
        byte *local_14 = (byte *)g_4e0348;
        volatile long local_15 = 0;
        if (*(long *)(local_14 + 0xc4) > 0)
            local_15 = *(long *)(local_14 + 0xc8);
        s_1badc0 local_16;
        local_16.field_24 = NONE;
        long local_17 = function_26d100(g_4687b0, (long *)arg_3, (s_collision_result_1697c0 *)&local_16, &local_1);
        if (local_17 != NONE)
        {
            if (arg_4)
                *arg_4 = local_17;
            return true;
        }
        return false;
    }
    real local_18 = local_0->position.x - arg_2->x;
    real local_19 = local_0->position.y - arg_2->y;
    real local_20 = (real)sqrt(local_19 * local_19 + local_18 * local_18);
    if (local_20 > 2.0f)
    {
        arg_3->point.x = arg_2->x + g_4687b0->i;
        arg_3->point.y = arg_2->y + g_4687b0->j;
        arg_3->point.z = arg_2->z + g_4687b0->k;
        arg_3->output_index = NONE;
        if (arg_6)
            *arg_6 = true;
    }
    else
    {
        arg_3->point = *arg_2;
        arg_3->output_index = NONE;
    }
    return true;
}
