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

// score 220 (was 220): Frame 0x90 agrees; output EBX/EBP and vector/SSE homes remain.
// Delayed vertical zero 274; with center difference 271; native 2D perpendicular 286/285;
// complete aggregate 277/274; delayed zero and early three-use clearance 220; with center 259.
// Delayed zero and early clearance kept; other executable forms restored.
// Shared perpendicular 265; split squared/root distances 220; both 265; clean output with shared/split/both
// 282/279/282; shared perpendicular with coherent point source 225; with split distances 225.
// Staged second/first/both direction outputs 231/227/262; center-staged second alone/with first
// 240/273; copied-point staged second/both/center pair 231/262/276. Opening directions restored.
// Expanded radius and existing point-setter first/second/both combinations all 220;
// native squared-vector helper 273. Original point construction restored.
// Round 204: shared far-output store 252; native collision-test result 282; both 284;
// later adjusted-point copy 267; with shared far store/collision result/both 272/283/285;
// all with clean output access 301. Original output, spill and point lifetime restored.
// Output EBX/EBP, point/vector homes and SSE scheduling remain.
// No output self-address access 295; copied input/native object/both 281/295/281;
// shared collision storage alone/with input/object/both 295/282/295/282;
// copied input with entry/after-actor common result 287/287.
// frame 0x90 agrees; output EBX/EBP, vector homes and SSE scheduling differ.
// Vector/center/point/direction names 277 each; named clearance 277; shared actor-to-object difference
// 274; center memcpy 277; names/clearance/difference 274. Three-point gains rejected.
// Round 147: Planar scalar differences 255; complete projection products 220; both side-product
// sets 220; all squared terms 220; projection/side 220; planar/projection/side 255; all products
// 220; all products with planar differences 255.
// Round 150: Coherent first/second/both point inputs 215/217/264; second producer first with
// original/first/second/both 220/216/217/270; early uninitialized directions with both inputs 264.
// Inline collision provider on original/first/second/both inputs 220/215/217/264, identical with an
// explicit out-of-line declaration. Best five-point gain rejected; original body restored.
// Partial original-point input forms are exact-match-only; all trial bodies are restored.
// Round 174: center/radius/both per-use sources 291/242/296; complete actor-position snapshot
// alone/with center/radius/both 267/283/262/246; complete sources with clean output access 296.
// Original observations, early clearance, delayed zero, output access and scalar spill restored.
// Direction declaration/call ordering and collision-storage lifetime combinations 220 in seven forms;
// those together with radius before center copy 214 (six-point gain restored).
// Same-size point/vector name swaps, separately and in combinations, 220 in eight forms.
// Far planar separation through the existing 2D/3D helpers 228/228; complete
// native 2D/3D initializers 220/220; each with entry collision storage 228/220/228/220.
// All restored; original scalar geometry and collision-storage lifetime remain.
// Round 191: Direct-output mode first with original returns, Boolean/long/byte
// common completion results 261/261/236/242; all four with radius before center
// 261/261/236/242. Original mode/result graph and executable body restored.
// Round 345: Native original/center/both CRT copies 220/220/220; entry point
// storage alone/with original/center/both copies 218/218/218/218; both copies
// and entry storage with radius before center 213 (seven-point gain below gate). All restored.
// Round 254: entry collision-sentinel initialization with original/first-point/center/both
// entry storage 269/269/269/269; with radius-before-center 269/269/269/269.
// Original late collision initialization, point storage and executable body restored.
// Round 195: Normalizer inline/noinline/combined/forceinline declarations 220 each;
// with radius before center 215 each. Five-point best restored; original output access,
// scalar spill, radius lifetime and body retained, output/vector homes unresolved.
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
                        if (0.0f > local_10.j * local_6.i + local_12.i * local_10.i)
                        {
                            local_12.i = 0.0f - local_12.i;
                            local_12.j = 0.0f - local_12.j;
                        }
                        local_12.k = 0.0f;
                        real local_13 = 2.0f - local_11;
                        if (normalize2d((point2f *)&local_12) > 0.0f)
                        {
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
