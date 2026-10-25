#include "unknown_11c920.h"
#include "unknown_0259d0.h"
// @flags /O2 /arch:SSE /Gr

struct s_ai_capsule_view
{
 byte active;
 byte unknown01[3];
 point3f center;
 real value10;
 real value14;
 real height;
 long value1c;
 long object_index;
 real radius;
};
struct s_collision_result_1697c0
{
 long field_0;
 real field_4;
 point3f field_8;
 byte field_14[0x24 - 0x14];
 short field_24;
 byte field_26[0x40 - 0x26];
 long field_40;
 byte field_44[0x5c - 0x44];
};
short __stdcall function_1c89b0(long actor_index, short maximum_count, s_ai_capsule_view *capsules);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
 long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
long function_11ea30(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v, real radius);

PRIVATE __forceinline void function_1c9291(point3f const *arg_0, vector3f const *arg_1,
 real arg_2, real arg_3, point3f *arg_4)
{
 real arc = arg_2 * arg_2 * arg_3 * 0.5f;
 for (long component = 0; component < 3; ++component)
  arg_4->n[component] = arg_1->n[component] * arg_2 + arg_0->n[component];
 arg_4->z = arc + arg_4->z;
}

// score 6 (was 43): retail computes the x and y products before the arc, and
// squares the copied step (mulss xmm1, xmm1) where ours multiplies the copy by
// the step. Native point/velocity/both value inputs 105/146/170; array members 43;
// depth 0 242; global optimization off 220; size 279; frame pointer 218;
// shared scaler and setter 127; arc first 116; separate arc store 127; the arc
// after the component loop 18; the two-point ray test (0x16a040) called instead
// of the vector and 0x1697c0 16; horizontal loop then arc then vertical 45;
// one named step for both uses 72; plain inline 6.
// score 6 (was 6): Horizontal products still follow the arc square; copied-step operand differs.
// Helper orders (P point, V velocity, D step, G gravity, O output):
// OPVDG/PVODG/PDVGO/PVGDO/DPVGO/DGPVO/POVDG/PDGVO all 6.
// horizontal/square schedule remains; square/weighted producer
// split across output loop 6/6; split accumulators 6/6; delayed
// vertical consumers 6/6; split unsigned ordinal 6; shared step on
// component/sentinel form 72.
// @retail 0x1c9290
bool function_1c9290(long arg_0, point3f const *arg_1, vector3f const *arg_2,
 real arg_3, real arg_4, long arg_5, bool arg_6)
{
 s_ai_capsule_view local_0[32];
 short local_1 = function_1c89b0(arg_0, 32, local_0);
 long local_2 = 0x15808c2f;
 if (arg_6)
  local_2 = 0x15808c0f;
 arg_3 *= 0.9f;
 point3f local_3 = *arg_1;
 vector3f local_4 = *arg_2;
 real local_6 = 0.0f;
 real local_5 = arg_3 < 0.2f ? arg_3 : 0.2f;
 bool local_7;
 do
 {
  s_collision_result_1697c0 local_11;
  local_11.field_24 = NONE;
  point3f local_9;
  function_1c9291(&local_3, &local_4, local_5 - local_6, arg_4, &local_9);
  vector3f local_10;
  vector3d_from_points3d(&local_3, &local_9, &local_10);
  local_7 = !function_1697c0(local_2, &local_3, &local_10, arg_5, NONE, &local_11);
  if (!local_7)
   break;
  vector3f local_13;
  vector3d_from_points3d(&local_3, &local_9, &local_13);
  for (short local_12 = 0; local_12 < local_1; ++local_12)
   if ((char)function_11ea30(&local_3, &local_13, &local_0[local_12].center,
       (vector3f *)&local_0[local_12].value10, local_0[local_12].radius))
    return false;
  local_4.k += (local_5 - local_6) * arg_4;
  local_3 = local_9;
  local_6 = local_5;
  local_5 += 0.2f;
  if (local_5 > arg_3)
   local_5 = arg_3;
 } while (arg_3 > local_6);
 return local_7;
}
