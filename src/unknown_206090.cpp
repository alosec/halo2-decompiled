// @flags /O2 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "impacts.h"
#include <math.h>
#include "slot_handler.h"
static __forceinline long real_to_long(real value);

#define MIN(arg_0, arg_1) ((arg_0) > (arg_1) ? (arg_1) : (arg_0))
#define MAX(arg_0, arg_1) ((arg_0) > (arg_1) ? (arg_0) : (arg_1))
#define PIN(arg_0, arg_1, arg_2) ((arg_0) < (arg_1) ? (arg_1) : ((arg_0) > (arg_2) ? (arg_2) : (arg_0)))

struct s_vehicle_physics_state;
struct s_vehicle_ray;
struct s_vehicle_contact_result;
struct s_query_batch_result;
struct s_impact;
struct s_unknown_1eb550;
extern s_unknown_1eb550 *g_51e9c4;
extern s_record_pool *g_51ebfc;
extern s_record_pool *g_51ec00;
extern vector3f *g_4687bc;

void function_ba1d0(long arg_0, vector3f *arg_1, vector3f *arg_2);
real function_17c900(short arg_0, real arg_1);
bool function_182800(s_vehicle_ray *arg_0, long arg_1, void *arg_2);
void __stdcall function_1821d0(long arg_0, real arg_1, s_query_batch_result *arg_2, long arg_3);
void function_207ab0(long arg_0, s_vehicle_physics_state *arg_1, s_vehicle_contact_result const *arg_2);
void havok_component_rigid_body_linear_velocity_change(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);
long function_2078f0(long const *arg_0, s_impact_data const *arg_1, vector3f const *arg_2, real arg_3, bool arg_4);
void function_2079f0(long const *arg_0, long arg_1, bool arg_2);
void impact_rigid_body_indices_get(s_impact const *arg_0, long arg_1, long *arg_2, long *arg_3);
void function_2294a0(long arg_0, long arg_1, long arg_2, s_impact *arg_3);
void impact_set_contact_from_component(s_impact *arg_0);

struct s_206090_contact
{
    byte padding_0[4];
    vector3f field_4;
    byte padding_10[0x24];
    point3f field_34;
    vector3f field_40, field_4c, field_58, field_64;
    byte field_70;
    byte padding_71[3];
    vector3f field_74;
    byte field_80, field_81, field_82, field_83;
    real field_84, field_88, field_8c, field_90;
    vector3f field_94, field_a0;
    byte padding_ac[4];
    short field_b0;
    byte field_b2, field_b3;
    byte padding_b4[0x24];
};
typedef char assert_206090_contact_size[(sizeof(s_206090_contact) == 0xd8) ? 1 : -1];

struct s_206090
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
    s_206090_contact field_b10[16];
};

struct s_206091
{
    point3f field_0;
    vector3f field_c;
    real field_18;
    vector3f field_1c;
};

PRIVATE __forceinline real function_206091(vector3f const *arg_0, vector3f const *arg_1)
{
    return arg_0->k * arg_1->k + arg_0->j * arg_1->j + arg_0->i * arg_1->i;
}

PRIVATE __forceinline void function_206092(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
    real second_i = arg_1->i;
    real second_j = arg_1->j;
    real second_k = arg_1->k;
    arg_2->k = arg_0->i * second_j - arg_0->j * second_i;
    arg_2->j = arg_0->k * second_i - arg_0->i * second_k;
    arg_2->i = arg_0->j * second_k - arg_0->k * second_j;
}

PRIVATE __forceinline real function_206093(vector3f const *arg_0)
{
    vector3f magnitude_input = *arg_0;
    return (real)sqrt((double)magnitude_input.k * magnitude_input.k + (double)magnitude_input.j * magnitude_input.j + (double)magnitude_input.i * magnitude_input.i);
}

PRIVATE __forceinline real function_206094(vector3f *arg_0)
{
    real local_0 = function_206093(arg_0);
    if (fabs(local_0) < 0.0001f)
        return 0.0f;
    real local_1 = 1.0f / local_0;
    arg_0->i *= local_1;
    arg_0->j *= local_1;
    arg_0->k *= local_1;
    return local_0;
}

PRIVATE __forceinline real function_206097(vector3f *arg_0)
{
    double local_2 = (double)arg_0->i * arg_0->i;
    local_2 += (double)arg_0->k * arg_0->k;
    local_2 += (double)arg_0->j * arg_0->j;
    real local_0 = (real)sqrt(local_2);
    if (fabs(local_0) < 0.0001f)
        return 0.0f;
    real local_1 = 1.0f / local_0;
    arg_0->i *= local_1;
    arg_0->j *= local_1;
    arg_0->k *= local_1;
    return local_0;
}

PRIVATE __forceinline void function_206095(vector3f *arg_0, vector3f const *arg_1)
{
    arg_0->i += arg_1->i;
    arg_0->j += arg_1->j;
    arg_0->k += arg_1->k;
}

PRIVATE __forceinline void function_206096(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
    for (long component = 0; component < 3; ++component)
        arg_2->n[component] = arg_0->n[component] * arg_1;
}

#pragma inline_depth(1)
#include "vector_products.h"
// score 1402 (was 1402): Frame 0x5cc versus 0x58c; component/velocity homes, contact induction and scheduling remain.
// Earlier 1585 (was 1596): Frame 0x590 versus 0x58c; component/velocity homes and contact induction remain.
// Earlier 1600 from 1600: frame 0x580 versus 0x58c; velocity homes and contact induction differ;
// frame pointer 2092; inline depth zero 3049, eight 1600; point/contact row tables 1600/1600;
// double magnitude accumulator 1654.
// frame 0x580 versus 0x58c; velocity homes and indexed contact records differ.
// Function-scope impact record 1655; function-scope contact direction 1600; function-scope projection 1600;
// short fixed initialization index 1664; duplicate search in loop condition 1600.
// Earlier 1600 from 1600: frame 0x580 versus 0x58c and indexed contact homes remain.
// Shared point subtraction first/contact/both 1625/1656/1676;
// indexed normalized-direction copy 1645; indexed tangential-velocity copy 1638.
// Earlier 1596 from 1600: frame 0x58c recovered; velocity homes and indexed contacts differ. Contact velocity
// 1604; point velocity 1663; both 1659; result normalizers 1641; cross j/k/i 1642. On contact form:
// normalizers 1646; scale loop 1596 kept; cross order 1608; velocity names 1604; add loop 1622; scale/add
// loops 1618. Original-frame scale loop 1626.
// On recovered frame: entry return result 1599; native query records 1596; both configuration
// counts read once 1644 (frame 0x590); shared first/contact point subtraction 1620/1616.
// Round 357: Typed contact/point/both records 1612/1596/1612; indexed typed contacts
// 1585 kept (plain), indexed typed point/contact records 1593 (three-point gain rejected).
// Native configuration header/point rows/contact rows 1585 each; header/point, header/contact,
// both row tables and all layouts 1585 each; all layouts with both tables indexed at each use 1640.
// Round 140: cross first/second/both scalar captures 1581/1570/1587. Native-argument and cross-result forms
// 1620/1570/1955/1423/2158/1426/1402/1456/1504/1655/1680/1402/1772 reverted for generated local address
// cancellations.
// Pointer-interface magnitude vector capture alone/with second cross 1423/1402; three scalar magnitude
// captures alone/with second cross 1577/1604. Vector magnitude snapshot plus second cross scalars kept, plain
// 1402; all original interfaces and call expressions preserved.
// Complete cross-vector second/first/both captures with magnitude snapshot 1655/1552/1765; rejected.
// Round 143: pointer-interface complete add/scale/both captures 1991/1892/2424;
// first/second/both dot inputs 1885/1772/2221; add+dot and scale+dot 2664/2554.
// On the retained magnitude/second-cross graph, velocity/buffers/magnitude home names and
// all four combinations 1402 each; all three second-cross component names 1402. All restored.
// Consistent configuration/object/both pointer observations 1569/1524/1522; separate first/second/both
// normalizer input/output contracts 1402 each; both contracts with configuration/object reads 1569/1524.
// Round 208: named squared magnitude 1435; first/second normalizer component loops 1432/1410;
// squared magnitude with first/second loops 1434/1411; both loops 1432; all three 1450;
// all three with bounded word impact count 1445 (6238 bytes, frame 0x5d0).
// Round 149: Existing scale/add/cross output-return masks 1474/1402/1474/1402/1474/1402/1474; returned
// scale outputs at two immediate consumers 1474. First/second/both single-exit normalizers
// 1456/1410/1402; matching plain component loops 1410/1432/1432; both with first/second result name
// 1402/1402. All restored.
// Round 166: Ordinary inline at existing magnitude/cross/paired normalizer boundaries,
// individually and jointly, then all with scale/add: all eight 1402 fast at 6239/6238.
// Original producer declarations, captures, bodies and all call expressions restored.
// Round 135 (aligned difflib counts in brackets; base [1470]): the contact query rows sit at
// 0x1d8 and the 32-entry list at 0x15c in retail, 0x40 higher in ours. Magnitude read through
// the pointer 1570 (frame 0x590) [1449]; normalizer only through the pointer 1480 (0x5b4)
// [1460]; cross product without the scalar captures 1423 [1463]; both pointer forms 1585
// (0x590) [1442]; contact arrays declared at function scope 1402 [1470].
// @retail 0x206090
bool __stdcall function_206090(s_vehicle_physics_state *arg_0, vector3f *arg_1, vector3f *arg_2)
{
    s_206090 *local_0 = (s_206090 *)arg_0;
    byte *local_1 = local_0->field_4;
    byte *local_2 = *(byte **)(g_4e0300->data + (local_0->field_0 & 0xffff) * 12 + 8);
    s_havok_component *local_3 = havok_component_get(*(long *)(local_2 + 0xb4));
    vector3f local_4, local_5;
    function_ba1d0(local_0->field_0, &local_4, &local_5);
    *arg_1 = *g_4687a4;
    *arg_2 = *g_4687a4;
    for (long local_6 = 0; local_6 < *(long *)(local_1 + 0x3c); local_6++)
    {
        byte *local_7 = local_0->field_90 + local_6 * 0xa8;
        byte *local_8 = *(byte **)(local_1 + 0x40) + local_6 * 0x4c;
        vector3f *local_9 = (vector3f *)(local_7 + 0x40);
        vector3f *local_10 = (vector3f *)(local_7 + 0x4c);
        local_9->i = *(real *)(local_7 + 0x34) - local_0->field_60.i;
        local_9->j = *(real *)(local_7 + 0x38) - local_0->field_60.j;
        local_9->k = *(real *)(local_7 + 0x3c) - local_0->field_60.k;
        cross3f(&local_5, local_9, local_10);
        function_206095(local_10, &local_4);
        real local_11 = PIN(*(real *)(local_1 + 0x30) * local_0->field_8c * 0.31830987f, -1.0f, 1.0f);
        local_11 = (local_0->field_74.i >= 0.0f ? -1.0f : 1.0f) * local_11;
        real local_12 = -(local_0->field_74.j * *(real *)(local_7 + 0x88));
        real local_13 = local_11 * local_11 * (local_11 != 0.0f ? (local_11 < 0.0f ? -1 : 1) : 0) * *(real *)(local_7 + 0x88);
        real local_95 = local_12 < 0.0f ? -local_12 : local_12;
        real local_96 = local_13 < 0.0f ? -local_13 : local_13;
        real local_14 = local_95 > local_96 ? local_12 : local_13;
        real local_15 = local_14 > 0.0f ? PIN(local_14 - 0.1f, 0.0f, 1.0f) : PIN(local_14 + 0.1f, -1.0f, 0.0f);
        real local_16 = *(real *)(local_1 + 0x2c) * local_15 + 1.0f;
        real local_17 = local_16 * *(real *)(local_8 + 0x20);
        real local_18 = local_16 * *(real *)(local_8 + 0x10);
        s_206091 local_19;
        local_19.field_0 = *(point3f *)(local_7 + 0x34);
        function_206096(g_4687bc, local_17 + local_18, &local_19.field_c);
        long local_20 = *(long *)(local_2 + 0xb4);
        if (local_7[0x81] && local_20 != NONE)
        {
            s_havok_component *local_21 = havok_component_get(local_20);
            if (TEST_FIELD_BIT(local_21->flag5) && local_21->unknown9c &&
                function_182800((s_vehicle_ray *)&local_19, NONE, (void *)local_21->unknown9c))
            {
                real local_22 = (local_17 + local_18) * local_19.field_18 - local_17;
                real local_23 = *(real *)(local_8 + 0x18);
                real local_24 = *(real *)(local_8 + 0x1c);
                real local_25;
                real local_26 = *(real *)(local_7 + 0x24);
                real local_97 = local_23 - local_24;
                real local_98 = local_24 - local_23;
                if ((local_97 < 0.0f ? -local_97 : local_97) < 0.001f ||
                    (local_98 < 0.0f ? -local_98 : local_98) < 0.001f)
                    local_25 = local_24 < local_26 ? 1.0f : 0.0f;
                else if (local_23 > local_24)
                    local_25 = local_24 < local_26 ? (local_26 < local_23 ? (local_26 - local_24) / local_97 : 1.0f) : 0.0f;
                else
                    local_25 = local_23 < local_26 ? (local_26 < local_24 ? (local_24 - local_26) / local_98 : 0.0f) : 1.0f;
                real local_27 = local_22 > 0.0f ? 1.0f - local_22 / local_18 : 1.0f;
                real local_28 = (*(real *)g_51e9c4 * local_27 * local_27 - *(real *)(local_8 + 0x14) * function_206091(local_10, &local_19.field_1c)) *
                    *(real *)(local_7 + 0x9c) * *(real *)(local_8 + 8) * local_0->field_6c * local_25;
                if (*(real *)(local_7 + 0xa0) > 0.0f)
                {
                    real local_29 = (real)(((double)((local_6 + 1) / *(long *)(local_1 + 0x3c)) * 0.10000002384185791f + 0.9f) *
                        g_510c54->game_time * g_510c54->rate * 0.3f + (double)local_6 * 42.0f);
                    local_28 *= 1.0f - *(real *)(local_7 + 0xa0) * (function_17c900(8, local_29) * 0.55f + 0.45f);
                }
                vector3f local_30;
                function_206096(&local_19.field_1c, local_28, &local_30);
                function_206095((vector3f *)(local_7 + 0x90), &local_30);
                local_7[0x80] = true;
                *(real *)(local_7 + 0xa4) = local_27;
            }
        }
        function_206095((vector3f *)(local_7 + 0x58), (vector3f *)(local_7 + 0x90));
        function_206092(local_9, (vector3f *)(local_7 + 0x58), (vector3f *)(local_7 + 0x64));
        function_206095(arg_1, (vector3f *)(local_7 + 0x58));
        function_206095(arg_2, (vector3f *)(local_7 + 0x64));
    }
    long local_31 = 0, local_32 = 0, local_33 = 0;
    bool local_34 = false;
    long local_35[32];
    byte local_36[16 * 0x3c];
    for (long local_37 = 0; local_37 < 16; local_37++)
        *(short *)(local_36 + local_37 * 0x3c + 8) = NONE;
    if (*(long *)(local_1 + 0x44) > 0)
        function_1821d0(*(long *)(local_2 + 0xb4), *(real *)(local_1 + 0x18), (s_query_batch_result *)local_36, *(long *)(local_1 + 0x44));
    for (long local_38 = 0; local_38 < *(long *)(local_1 + 0x44); local_38++)
    {
        if (!local_0->field_b10[local_38].field_b3)
        {
            byte *local_40 = *(byte **)(local_1 + 0x48) + local_38 * 0x4c;
            bool local_41 = (bool)((*(dword *)(local_40 + 4) >> 1) & 1);
            vector3f local_42 = *g_4687a4;
            vector3f *local_43 = &local_0->field_b10[local_38].field_40;
            vector3f *local_44 = &local_0->field_b10[local_38].field_4c;
            local_43->i = local_0->field_b10[local_38].field_34.x - local_0->field_60.i;
            local_43->j = local_0->field_b10[local_38].field_34.y - local_0->field_60.j;
            local_43->k = local_0->field_b10[local_38].field_34.z - local_0->field_60.k;
            vector3f contact_velocity;
            function_206092(&local_5, local_43, &contact_velocity);
            function_206095(&contact_velocity, &local_4);
            *local_44 = contact_velocity;
            local_32++;
            byte *local_45 = local_36 + local_38 * 0x3c;
            function_207ab0(local_38, arg_0, (s_vehicle_contact_result *)local_45);
            if (local_0->field_b10[local_38].field_70)
            {
                local_44->i -= local_0->field_b10[local_38].field_74.i;
                local_44->j -= local_0->field_b10[local_38].field_74.j;
                local_44->k -= local_0->field_b10[local_38].field_74.k;
                local_34 = true;
            }
            if (local_0->field_b10[local_38].field_84 > 0.0001f)
            {
                vector3f *local_46 = &local_0->field_b10[local_38].field_a0;
                real local_47 = function_206091(local_44, local_46);
                local_0->field_b10[local_38].field_88 = local_0->field_6c * (*(real *)g_51e9c4 / *(real *)(local_1 + 8) * local_0->field_b10[local_38].field_84 - *(real *)(local_1 + 0xc) * local_47);
                function_206096(local_46, local_0->field_b10[local_38].field_88, &local_0->field_b10[local_38].field_94);
                vector3f local_48 = *g_4687b0;
                if (*(long *)(local_45 + 0x24) != NONE && local_0->field_b10[local_38].field_88 > 0.0f)
                {
                    s_havok_component *local_49 = havok_component_get(*(long *)(local_45 + 0x24));
                    long local_50 = *(long *)(local_45 + 0x28);
                    hkRigidBody *local_51 = local_49->rigid_bodies.data[local_50].rigid_body;
                    if (!local_51->m_fixed && local_51->m_motion->kind_code() != 6)
                    {
                        real local_52 = MAX(1.0f, havok_component_rigid_body_get(local_50, local_49)->m_motion->getMass());
                        real local_53 = PIN(local_52, local_0->field_6c * 0.0625f, local_0->field_6c * 16.0f);
                        real local_54 = (local_46->k + 1.0f) * 0.5f / local_53 * local_0->field_b10[local_38].field_88;
                        local_54 *= local_54;
                        local_54 = MIN(local_54, MAX(8.0f, g_510c54->field_2_3 * local_47));
                        vector3f local_55;
                        function_206096(local_46, g_510c54->rate * local_54 * -1.0f, &local_55);
                        havok_component_rigid_body_linear_velocity_change(local_50, local_49, &local_55);
                    }
                }
                vector3f local_56 = local_0->field_b10[local_38].field_4;
                real local_57 = function_206091(&local_56, local_46);
                local_56.i -= local_46->i * local_57;
                local_56.j -= local_46->j * local_57;
                local_56.k -= local_46->k * local_57;
                if (function_206094(&local_56) != 0.0f)
                {
                    vector3f local_58 = *local_44;
                    real local_59 = function_206091(local_44, local_46);
                    local_58.i -= local_46->i * local_59;
                    local_58.j -= local_46->j * local_59;
                    local_58.k -= local_46->k * local_59;
                    vector3f local_60;
                    if (local_41)
                    {
                        function_206096(&local_56, -local_0->field_b10[local_38].field_8c, &local_60);
                        function_206095(&local_60, &local_58);
                    }
                    else
                    {
                        vector3f local_61;
                        function_206096(&local_56, function_206091(&local_58, &local_56), &local_61);
                        local_60.i = local_58.i - local_61.i;
                        local_60.j = local_58.j - local_61.j;
                        local_60.k = local_58.k - local_61.k;
                        local_0->field_b10[local_38].field_8c = function_206093(&local_61) * (function_206091(&local_61, &local_56) > 0.0f ? 1.0f : -1.0f);
                    }
                    real local_66 = function_206093(&local_60);
                    local_0->field_b10[local_38].field_90 = local_66;
                    real local_62 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x1c) : *(real *)(local_1 + 0x10);
                    real local_63 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x20) : *(real *)(local_1 + 4);
                    real local_64 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x24) : *(real *)(local_40 + 0x18);
                    vector3f *local_65 = &local_60;
                    if (local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83)
                    {
                        local_65 = &local_58;
                        local_66 = function_206093(local_65);
                    }
                    real local_67 = 1.0f - PIN((*(real *)(local_1 + 0x14) - local_46->k) / (*(real *)(local_1 + 0x14) - *(real *)(local_1 + 0x18)), 0.0f, 1.0f);
                    if (local_66 > local_64)
                    {
                        vector3f local_99 = *local_65;
                        function_206094(&local_99);
                        real local_68 = (real)sqrt(local_62 * 0.9f);
                        function_206096(&local_99, -(*(real *)(local_40 + 8) * local_68 * local_67 * local_0->field_b10[local_38].field_88), &local_42);
                        local_0->field_b10[local_38].field_81 = true;
                    }
                    else
                    {
                        real local_69 = (real)sqrt(local_63 * 0.9f);
                        function_206096(local_65, -(local_69 * *(real *)(local_40 + 8) * g_510c54->rate * local_67 * local_0->field_b10[local_38].field_88), &local_42);
                        local_31++;
                    }
                    local_48 = local_60;
                }
                local_0->field_b10[local_38].field_80 = true;
                if (*(real *)(local_40 + 0xc) > 0.0001f && local_46->k > *(real *)(local_1 + 0x18))
                {
                    real local_70 = MAX(0.0f, local_0->field_b10[local_38].field_90 - *(real *)(local_40 + 0x18));
                    if (function_206097(&local_48) == 0.0f)
                        local_48 = *g_4687b0;
                    real local_71 = -(*(real *)(local_40 + 0xc) - local_0->field_b10[local_38].field_84 - 0.0001f);
                    point3f local_100;
                    local_100.x = local_46->i * local_71 + local_0->field_b10[local_38].field_34.x;
                    local_100.y = local_46->j * local_71 + local_0->field_b10[local_38].field_34.y;
                    local_100.z = local_46->k * local_71 + local_0->field_b10[local_38].field_34.z;
                    s_impact_data local_72;
                    local_72.unknown00 = false;
                    local_72.component_a = *(long *)(local_2 + 0xb4);
                    local_72.unknown08 = 0;
                    local_72.material_a.m_index = *(short *)(local_40 + 0x40);
                    local_72.component_b = NONE;
                    local_72.unknown14 = NONE;
                    local_72.material_b.m_index = local_0->field_b10[local_38].field_b0;
                    local_72.position = local_100;
                    local_72.normal = *local_46;
                    local_72.type = local_38;
                    local_72.unknown38 = false;
                    local_72.shape.type = NONE;
                    local_72.shape.index = NONE;
                    function_2078f0(&local_0->field_0, &local_72, &local_48, local_70, local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83);
                    local_72.unknown38 = true;
                    long local_74 = function_2078f0(&local_0->field_0, &local_72, &local_48, local_70, local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83);
                    if (local_74 != NONE)
                    {
                        long local_75;
                        for (local_75 = 0; local_75 < local_33; local_75++)
                            if (local_35[local_75] == local_74)
                                break;
                        if (local_75 == local_33 && (dword)local_33 < 32)
                            local_35[local_33++] = local_74;
                    }
                }
            }
            else
                function_2079f0(&local_0->field_0, local_38, false);
            function_206095(&local_0->field_b10[local_38].field_58, &local_0->field_b10[local_38].field_94);
            function_206095(&local_0->field_b10[local_38].field_58, &local_42);
            function_206092(local_43, &local_0->field_b10[local_38].field_58, &local_0->field_b10[local_38].field_64);
            function_206095(arg_1, &local_0->field_b10[local_38].field_58);
            function_206095(arg_2, &local_0->field_b10[local_38].field_64);
            if (local_41 && local_0->field_b10[local_38].field_80)
            {
                vector3f *local_76 = &local_0->field_b10[local_38].field_4;
                real local_77 = local_42.i * local_76->i + local_76->k * local_42.k + local_76->j * local_42.j;
                vector3f local_78;
                function_206096(local_76, local_77, &local_78);
                real local_95 = (real)sqrt((double)local_78.i * local_78.i + (double)local_78.k * local_78.k + (double)local_78.j * local_78.j);
                if (function_206091(local_76, &local_42) > 0.0f)
                    local_0->field_70 -= local_95;
                else
                    local_0->field_70 += local_95;
            }
        }
        else
            function_2079f0(&local_0->field_0, local_38, false);
    }
    for (long local_79 = 0; ; local_79++)
    {
        long local_80 = local_3->unknown20;
        byte *local_81 = g_51ec00->data + (local_80 & 0xffff) * 0x40;
        long local_82 = local_80 == NONE ? 0 : *(short *)(g_51ec00->data + (local_80 & 0xffff) * 0x40 + 2);
        if (local_79 >= local_82)
            break;
        long local_83 = *(long *)(local_81 + 4 + local_79 * 4);
        byte *local_84 = g_51ebfc->data + (local_83 & 0xffff) * 0xa0;
        if (!local_84[0xe])
            continue;
        bool local_85 = false;
        for (long local_86 = 0; local_86 < local_33; local_86++)
        {
            if (local_35[local_86] == local_83)
            {
                for (long local_87 = 0; local_87 < (local_80 == NONE ? 0 : *(short *)(local_81 + 2)); local_87++)
                {
                    long local_88 = *(long *)(local_81 + 4 + local_87 * 4);
                    byte *local_89 = g_51ebfc->data + (local_88 & 0xffff) * 0xa0;
                    if ((char)local_89[0xd] != NONE && !local_89[0xe] && *(short *)(local_89 + 0x2c) == *(short *)(local_84 + 0x2c) &&
                        *(short *)(local_89 + 0x2e) == *(short *)(local_84 + 0x2e))
                    {
                        local_85 = true;
                        break;
                    }
                }
            }
        }
        if (!local_85)
        {
            while (*(short *)(local_84 + 8) > 0)
            {
                if (*(short *)(local_84 + 8) == 1 && PIN((char)local_84[0x10], 1, 1) == (char)local_84[0x10])
                {
                    long local_90, local_91;
                    impact_rigid_body_indices_get((s_impact *)local_84, local_83, &local_90, &local_91);
                    real local_92 = *(real *)(local_84 + 0x14);
                    if (!local_84[0x11] || local_92 > *(real *)(local_84 + 0x18))
                    {
                        real local_93 = g_510c54->field_2_3 * 0.2f;
                        long local_94 = real_to_long(local_93);
                        if (*(long *)(local_84 + 0x94) == NONE || g_510c54->game_time - *(long *)(local_84 + 0x94) > local_94 ||
                            local_92 > *(real *)(local_84 + 0x98) * 1.3f)
                        {
                            local_84[0x11] = true;
                            *(real *)(local_84 + 0x18) = local_92;
                        }
                    }
                    function_2294a0(local_83, local_90, local_91, (s_impact *)local_84);
                }
                (*(short *)(local_84 + 8))--;
            }
        }
        else
            impact_set_contact_from_component((s_impact *)local_84);
    }
    return !local_34 && *(long *)(local_1 + 0x44) && local_31 > (local_32 >> 1) && local_0->field_8.up.k > *(real *)(local_1 + 0x18);
}
#pragma inline_depth(255)
