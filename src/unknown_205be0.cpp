// @flags /O2 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "normalize3d.h"
#include <string.h>

struct s_vehicle_physics_state;

struct s_205be0_record
{
    transform4x3f field_0;
    point3f field_34;
    byte field_40[0x41];
    bool field_81;
    byte field_82[2];
    point3f field_84;
    byte field_90[0x18];
};

struct s_205be0
{
    long field_0;
    byte *field_4;
    transform4x3f field_8;
    matrix3x3 field_3c;
    vector3f field_60;
    real field_6c;
    real field_70;
    vector3f field_74;
    vector3f field_80;
    real field_8c;
    byte field_90[16 * 0xa8];
    byte field_b10[16 * 0xd8];
};

void function_ba1d0(long arg_0, vector3f *arg_1, vector3f *arg_2);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
real function_30bf0(vector3f *arg_0);
void havok_component_rigid_body_inertia_get(long arg_0, s_havok_component *arg_1, matrix3x3 *arg_2);
real havok_component_rigid_body_mass_get(long arg_0, s_havok_component *arg_1);
void havok_component_transform_set(s_havok_component *arg_0, transform4x3f const *arg_1);
void function_1cfb90(s_havok_component *arg_0, transform4x3f const *arg_1, void const *arg_2);

// @retail 0x205be0
bool __stdcall function_205be0(s_vehicle_physics_state *arg_0, long arg_1)
{
    bool result;
    {
        long local_0 = (arg_1 & 0xffff) * 12;
        byte *local_1 = *(byte **)(g_4e0300->data + local_0 + 8);
        byte *local_2 = g_4e3b44[*(long *)local_1 & 0xffff].bytes + 0x2ac;
        long local_3 = *(long *)(local_1 + 0xb4);
        result = false;
        if (local_3 == NONE)
            goto done;
        s_havok_component *local_4 = havok_component_get(local_3);
        if ((local_2[0] & 1) || local_4->rigid_bodies.size <= 0)
            goto done;
        vector3f local_5;
        function_ba1d0(arg_1, &local_5, NULL);
        s_205be0 *local_6 = (s_205be0 *)arg_0;
        havok_component_rigid_body_inertia_get(0, local_4, &local_6->field_3c);
        byte *local_7 = (byte *)local_4->rigid_bodies.data[0].rigid_body->m_motion;
        vector3f const *motion = (vector3f const *)(local_7 + 0x70);
        real_point3d_set((point3f *)&local_6->field_60, motion->i, motion->j, motion->k);
        local_6->field_0 = arg_1;
        local_6->field_4 = local_2;
        local_6->field_6c = havok_component_rigid_body_mass_get(0, local_4);
        local_6->field_70 = 0.0f;
        transform4x3f *local_8 = &local_6->field_8;
        function_ba160(arg_1, local_8);
        if (local_8->scale != 1.0f)
        {
            real local_9 = 1.0f / local_8->scale;
            vector3f_scale(&local_5, local_9, &local_5);
        }
        local_6->field_80.i = local_8->forward.k * local_5.k +
            local_8->forward.j * local_5.j + local_8->forward.i * local_5.i;
        local_6->field_80.j = local_8->left.k * local_5.k +
            local_8->left.j * local_5.j + local_8->left.i * local_5.i;
        local_6->field_80.k = local_8->up.k * local_5.k +
            local_8->up.j * local_5.j + local_8->up.i * local_5.i;
        function_30bf0(&local_6->field_80);
        local_6->field_8c = 0.0f;
        local_6->field_74 = *(vector3f *)(local_1 + 0x1b0);
        memset(local_6->field_90, 0, *(long *)(local_2 + 0x3c) * 0xa8);
        memset(local_6->field_b10, 0, *(long *)(local_2 + 0x44) * 0xd8);
        byte *local_10 = *(byte **)(g_4e0300->data + local_0 + 8);
        char *local_11 = (char *)(local_10 + *(short *)(local_10 + 0x11a) +
            (*(short *)(local_10 + 0x118) / 10) * 2);
        s_object_marker local_12;
        s_205be0_record *record = (s_205be0_record *)local_6->field_90;
        for (long local_13 = 0; local_13 < *(long *)(local_2 + 0x3c); local_13++)
        {
            byte *local_14 = *(byte **)(local_2 + 0x40) + local_13 * 0x4c;
            function_b8d30(arg_1, *(long *)local_14, &local_12, 1, false);
            record->field_0 = *local_8;
            record->field_34 = local_12.matrix.position;
            record->field_84 = local_12.node_matrix.position;
            record->field_81 = local_12.node_index == 0;
            ++record;
        }
        for (long local_16 = 0; local_16 < *(long *)(local_2 + 0x44); local_16++)
        {
            byte *local_17 = *(byte **)(local_2 + 0x48) + local_16 * 0x4c;
            byte *local_18 = local_6->field_b10 + local_16 * 0xd8;
            function_b8d30(arg_1, *(long *)local_17, &local_12, 1, false);
            *(transform4x3f *)local_18 = *local_8;
            *(point3f *)(local_18 + 0x34) = local_12.matrix.position;
            *(bool *)(local_18 + 0x82) = (*(long *)(local_17 + 4) & 0x10) &&
                (local_1[0x348] & 0x20);
            if ((*(long *)(local_17 + 4) & 0x20) && *(long *)(local_17 + 0x48) != NONE &&
                local_11[*(long *)(local_17 + 0x48) * 8 + 1] >= *(short *)(local_17 + 0x42))
                *(bool *)(local_18 + 0xb3) = true;
        }
        havok_component_transform_set(local_4, local_8);
        if (*(long *)(local_2 + 0x4c))
        {
            byte *local_19 = *(byte **)(local_2 + 0x50);
            if (local_19[0x3c] & 1)
            {
                real local_20[6];
                memcpy(local_20, local_19 + 0x48, sizeof(local_20));
                function_1cfb90(local_4, local_8, local_20);
            }
        }
        result = true;
    }
done:
    return result;
}
