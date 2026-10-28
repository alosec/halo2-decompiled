#include "unknown_11c920.h"
#include "unknown_1d5120.h"
#include "object_queries.h"
#include <float.h>
#include <math.h>
// @flags /O2 /arch:SSE /Gr

extern hkWorld *g_51e9a4;
struct s_vehicle_ray;
bool __stdcall function_168f40(long arg_0, s_vehicle_ray const *arg_1, long arg_2, long arg_3);
bool function_182800(s_vehicle_ray *arg_0, long arg_1, void *arg_2);
void function_1d43d0(s_havok_component *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);
void function_11bed0(s_location *arg_0, point3f const *arg_1);

class c_contact_shape_view
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void slot4() = 0;
 virtual long type() = 0;
 long unknown04;
 long metadata;
};
struct s_1d48f0
{
 point3f field_0;
 vector3f field_c;
 real field_18;
 vector3f field_1c;
};
const dword g_455978[81] = {
 0x0, 0x0, 0x0, 0x3f800000, 0x0, 0x0, 0x0, 0x0, 0x3f800000,
 0x3f3504f3, 0x0, 0x3f3504f3, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a,
 0x3f3504f3, 0x3f3504f3, 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0x0, 0x3f3504f3, 0x3f3504f3,
 0x0, 0xbf3504f3, 0x3f3504f3, 0xbf800000, 0x0, 0x0, 0x0, 0x3f800000, 0x0,
 0x0, 0xbf800000, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0x3f3504f3, 0x0,
 0xbf3504f3, 0x0, 0x3f3504f3, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0x3f13cd3a,
 0x0, 0x0, 0xbf800000, 0x3f3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3,
 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a,
 0x3f13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a
};
// score 321 (was 321): Aligned frame 0x1d4 agrees; the sample count now matches (retail's
// setne/dec/and/sub is `27 - (flag ? 0 : 9)`, which keeps the flag on the stack); optional input
// registers (esi/edi swapped), scalar homes and selective helper expansion remain.
// Round 8 (lane 10): noinline redeclaration of 0x11bed0 here 321 (ignored); a separate noinline
// declaration in 0x11bed0's own file 337 (called as retail, but 0x11bed0 and its caller 0x168f40 stop
// matching); bounds coordinate as a named local 360; table direction pointer for both samples 321;
// excluded index declared first, its test braced 323.
// Earlier: Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Earlier 350 (was 350): Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Frame 0x1d4 agrees; optional input registers, scalar homes and selective location/release expansion remain.
// Earlier: Aligned frame 0x1d4 agrees; optional-input registers, scalar homes and helper expansion remain.
// frame 0x1d4 agrees; optional-input registers, scalar homes and inline boundaries differ.
// aligned frame 0x1d4 agrees; optional input/register roles (arg_9/arg_10 in
// esi/edi swapped), scalar homes and selective helper expansion remain: ours inlines 0x11bed0 (retail
// calls it at all its sites) and calls 0x279740 at both releases (retail inlines the first).
// aligned frame 0x1d4 agrees; optional-input registers (ESI/EDI swapped), scalar homes
// and selective helper expansion remain: retail calls 0x11bed0 out of line and inlines the cache-range
// check of the first reference release (the form of 0x1d01c0), while ours does the opposite.
// Native/byte success 427/382; path-local excluded index 449; complete volume arms 487;
// both path lifetimes 485; count just before scan 366; read-only body 370; native transform copies 370.
// the presence callback at its retail 12-byte size, initialised by its constructor.
// Frame 0x1d4 agrees; optional input/scalar homes and helper expansion differ. Tried early
// bounds filter 430; early transform filter 361; both early filters 423; reset at query 361; direct ray rejection
// 393; per query callback 376; callback and direct ray 376; early filters and reset 423.
// Native callback context: conditional count 389; earlier conditional 495; earlier arithmetic count 354 kept (plain).
// Exact sample end 361; transform slot names 361; native ray endpoint 394; expanded radius name 361; unsigned sample ordinal 361.
// Candidate/endpoint/ray component loops 382/354/354; basis loop 524;
// translation loop 350 kept (plain); radial scaling/blend loops 354/354; combined producers 492.
// Direction/radial/both/endpoint/ray/all spatial declaration scopes 350 each;
// complete native endpoint value 403; paired bounds-axis producer 466. All restored.
// Complete candidate/blend/both product phases 350/350/350; both radial square terms 391;
// radial terms with candidate/all phases 391/391; six-use bounds origin 492, with candidate products 492. All restored.
// Complete lower/upper/both bound selections 413/466/484; complete horizontal square 350;
// bounds plus square 484; callback ending before cleanup 350; bounds/callback 484; square/callback 350. All restored.
// Round 187: bounds short/byte, sample long, translation long/short all 548 (2011 bytes);
// the four corresponding short-sample forms all 573 (1986 bytes). All eight ordinal-width forms restored.
// Sample-element reference 350; object-index field reference 357; both references 357;
// world snapshot alone/with sample 484/484; with object field/with both references 483/483;
// body/shape declaration homes at entry 350. All restored.
// Transform description after body/shape load/shape choice/pose copy 351/351/351/411;
// bounds description ends before add/exclusion 406/363; full shape selection 350; word success 415.
// Round 214: single native vector table, separate/reused radial sample and direct/shared ray difference
// 350/350/350/350; real-row table with the same producer grid 447/447/362/362. All restored.
// Noinline redeclaration of 0x11bed0 in this file 350 (ignored; still inlined); sample count as a
// 27/18 ternary 477 (retail's form, but the flag then goes in a register and caller 0xc5460 changes);
// ray endpoint adds as `x = x + i` 350; translation copy as three statements 354; upper bound as
// radius-plus-coordinate 350.
// Round 54: noinline declaration of 0x11bed0 447 (and 0x11bed0 itself 19 in --fast: CAVEAT on its
// callee 0x14a280); first release with the inline range check 442; both 401 (structural diff 36 vs 66);
// with an `arg_8 ? 27 : 18` count 427, count alone 477.
// Round 223: Both positive validation stages 350; result-terminated scan 395; optional failure
// first 351; both 395; zero rejection removed on these graphs 408/411/407/411. All restored.
// Round 148: native candidate value 486; native ray-offset value 467; both vector values 516;
// coordinated transform inline 350; inline/candidate 486; inline/offset 467; inline/both values
// 516; force-inline/both values 954. All executable forms restored.
// Lane 5 r77: `arg_8 ? 27 : 18` gives neg/sbb and makes the flag a register argument (477, caller
// 0xc5460 251 -> 255); `18`, then `27` if the flag 399; `27 - (arg_8 ? 0 : 9)` kept (350 -> 321).
// Then: volatile casts dropped from the radial length 388 (before the count change); length terms
// swapped 320; component index as a ternary 427; k cleared before the length, with or without its
// term 321/321; named lower/upper pointers in the bounds loop 321.
// Shared radial producers on the accepted count graph: in-place radius scale 321; complete blend setter 321.
// both shared producers 321; blend factor before normalization 331.
// factor with radius scale 331; factor with blend setter 331.
// all three producers 331; complete normalized-vector setter 321.
// All executable forms restored.
// @retail 0x1d48f0
bool function_1d48f0(point3f *arg_11, s_havok_component *arg_0, long arg_1, long arg_2, point3f const *arg_3,
 vector3f const *arg_4, s_location *arg_5, real arg_6, real arg_7, bool arg_8,
 point3f const *arg_9, long arg_10)
{
 c_contact_query_bounds_volume *local_1 = NULL;
 long local_2 = NONE;
 _control87(0x9001f, 0x8001f);
 _mm_setcsr(_mm_getcsr() | 0x1f80);
 if (arg_9)
 {
  c_contact_query_bounds_info local_3;
  real local_4 = arg_7 + 0.001f;
  local_3.lower.set(arg_3->x - local_4, arg_3->y - local_4, arg_3->z - local_4);
  local_3.upper.set(arg_3->x + local_4, arg_3->y + local_4, arg_3->z + local_4);
  for (long local_5 = 0; local_5 < 3; ++local_5)
  {
   if (((real *)&local_3.lower.m_quad)[local_5] + 0.001f > arg_9->n[local_5])
    ((real *)&local_3.lower.m_quad)[local_5] = arg_9->n[local_5] - 0.001f;
   if (arg_9->n[local_5] > ((real *)&local_3.upper.m_quad)[local_5] - 0.001f)
    ((real *)&local_3.upper.m_quad)[local_5] = arg_9->n[local_5] + 0.001f;
  }
  local_3.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_1 = new c_contact_query_bounds_volume(&local_3);
  ((c_contact_query_world *)g_51e9a4)->add(local_1);
  if (arg_10 != NONE) local_2 = havok_object_get(arg_10)->havok_component_index;
 }
 bool local_0 = false;
 long local_6 = 27 - (arg_8 ? 0 : 9); /* the table's 27 directions, or the first 18 */
 hkTransform local_7;
 c_contact_query_transform_volume *local_8;
 {
  c_contact_query_transform_info local_9;
  hkRigidBody *local_10 = havok_component_rigid_body_get(arg_1, arg_0);
  c_contact_shape_view *local_11 = *(c_contact_shape_view **)((byte *)local_10 + 0xc);
  if (local_11->type() == 0x17) local_11 = *(c_contact_shape_view **)((byte *)local_11 + 0x30);
  local_7.set(local_10->m_motion->m_transform);
  local_9.transform.set(local_7);
  local_9.shape = local_11;
  local_9.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_8 = new c_contact_query_transform_volume(&local_9);
 }
 ((c_contact_query_world *)g_51e9a4)->add(local_8);
 c_contact_presence local_12;
 for (long local_13 = 0; local_13 < local_6; ++local_13)
 {
  vector3f local_14 = ((vector3f const *)g_455978)[local_13];
  if (arg_6 > 0.0f)
  {
   vector3f local_15 = ((vector3f const *)g_455978)[local_13];
   real local_16 = (real)sqrt((double)*(real const volatile *)&local_15.i * local_15.i +
    (double)*(real const volatile *)&local_15.j * local_15.j);
   if (fabs(local_16) < 0.0001f) continue;
   real local_17 = 1.0f / local_16;
   local_15.i *= local_17;
   local_15.j *= local_17;
   local_15.k = 0.0f * local_17;
   if (local_16 == 0.0f) continue;
   local_15.i *= arg_6;
   local_15.j *= arg_6;
   local_15.k *= arg_6;
   real local_18 = (arg_7 - arg_6) / arg_7;
   local_14.i = local_14.i * local_18 + local_15.i;
   local_14.j = local_14.j * local_18 + local_15.j;
   local_14.k = local_14.k * local_18 + local_15.k;
  }
  arg_11->x = local_14.i * arg_7 + arg_3->x;
  arg_11->y = local_14.j * arg_7 + arg_3->y;
  arg_11->z = local_14.k * arg_7 + arg_3->z;
  local_7.m_translation.set(arg_11->x, arg_11->y, arg_11->z);
  local_12.found = false;
  ((hkRigidBody *)local_8)->pose_write(local_7);
  ((c_contact_query_dispatch *)local_8)->query(&local_12);
  if (local_12.found) continue;
  point3f local_19 = *arg_11;
  local_19.x += arg_4->i;
  local_19.y += arg_4->j;
  local_19.z += arg_4->k;
  if (function_168f40(0x1480040d, (s_vehicle_ray const *)&local_19, arg_0->object_index, NONE)) continue;
  if (local_1)
  {
   s_1d48f0 local_20;
   local_20.field_0 = *arg_9;
   local_20.field_c.i = local_19.x - local_20.field_0.x;
   local_20.field_c.j = local_19.y - local_20.field_0.y;
   local_20.field_c.k = local_19.z - local_20.field_0.z;
   if (function_182800((s_vehicle_ray *)&local_20, local_2, local_1) && !local_0) continue;
  }
  transform4x3f local_21;
  transform4x3f local_22;
  local_21.scale = 1.0f;
  local_21.forward.i = local_7.m_rotation.m_col0(0);
  local_21.forward.j = local_7.m_rotation.m_col0(1);
  local_21.forward.k = local_7.m_rotation.m_col0(2);
  local_21.left.i = local_7.m_rotation.m_col1(0);
  local_21.left.j = local_7.m_rotation.m_col1(1);
  local_21.left.k = local_7.m_rotation.m_col1(2);
  local_21.up.i = local_7.m_rotation.m_col2(0);
  local_21.up.j = local_7.m_rotation.m_col2(1);
  local_21.up.k = local_7.m_rotation.m_col2(2);
  for (long component = 0; component < 3; ++component)
   local_21.position.n[component] = local_7.m_translation(component);
  function_1d43d0(arg_0, &local_21, &local_22);
  *arg_11 = local_22.position;
  function_11bed0(arg_5, arg_11);
  local_0 = arg_5->cluster_index != NONE;
  if (local_0) break;
 }
 ((c_contact_query_world *)g_51e9a4)->remove(local_8);
 havok_reference_remove((c_havok_reference_counted *)local_8);
 if (local_1)
 {
  ((c_contact_query_world *)g_51e9a4)->remove(local_1);
  havok_reference_remove((c_havok_reference_counted *)local_1);
 }
 _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 _clearfp();
 _control87(0x9001f, 0xfffff);
 return local_0;
}
