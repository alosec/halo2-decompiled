#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
// @flags /O2 /arch:SSE /Gr

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
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
 long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
 long ignore_unit_index, s_collision_result_1697c0 *result);
bool function_11c010(short row, short column);
real function_30bf0(vector3f *v);
real magnitude3d(vector3f const *v);
extern vector3f *g_4687bc;

// score 170 (was 175): frame 0x8c versus 0x94; secondary-result home and probe-tail normalization differ.
// Word category 225; native clear state 170; direct origin 172; distance category 175; side workspace 175.
// Native clear with direct origin 171, word category 234, short return result 178.
// score 170 (was 170): frame 0x8c versus 0x94; secondary-result home and probe tail differ.
// Native clear with byte probe result 74 (not matched); fraction/index names 170/170;
// earlier native clear declaration 170; short clear state 237; short clear with byte result 240 (not matched).
// score 170 (was 170): frame 0x8c versus 0x94; raw secondary-result home and common probe tail still differ.
// Early clear/blocked/triple test results 221/173/179; all early test results 241;
// shared preliminary predicate 186; floating consistency 415; size policy 402; target pointer 187.
// frame 0x8c versus 0x94; secondary byte and quarter-point scheduling remain.
// Shared byte plus product-first X/Z/XZ/XYZ/Y 241 each; native point 241;
// axis-wise points 250; offset vector 244; original-arm publication 26 (not matched);
// native category with publication 28; probe/result names 26/26 (not matched).
// score 170 (was 170): frame 0x8c versus 0x94 and secondary-result/point schedules remain;
// first-point copy/add 273; displacement then add 5.
// center then add 5; first-point initializer 26.
// shared scalar offsets 47; external origin reference 26.
// center snapshot 329; indexed point loop 86.
// positive indexed add 42; positive indexed center 232.
// separate point loops 34; positive indexed expression 34.
// shared X offset 26; shared Z center 46.
// shared X/Z scalars 46; indexed scale then translate 62.
// All secondary-byte forms above remained unmatched and were restored.
// frame 0x8c versus 0x94 and secondary-result tail remain;
// direct primary miss 207; entry fraction zero 185; early probe bit 181; arithmetic clear bit 288;
// early bit with shared probe and Boolean/native clear 44/85 (not matched);
// shared probe with entry fraction/direct primary miss 44/71 (not matched).
// Separate copied probe output for all/quarter/triple/both arms 428/498/468/397;
// same output scopes with Boolean primary clear 422/492/462/391. Output copies grow frames.
// Staged byte-flow positive/negative/both point names 5/5/5; native positive/
// negative component arrays 5/5; existing shared scale for positive/both points
// 276/210; direct quarter constant 5. All unmatched added-byte forms restored.
// score 170 (was 170): Frame 0x8c/0x94 and secondary-result tail remain; byte-flow quarter-point
// XZY/YXZ/YZX/ZXY/ZYX completion 12/23/20/21/18; XYZ products then XYZ/ZXY centers 36/37; ZYX
// products then XYZ centers 47. All added-byte forms restored without a match.
// score 170 (was 170): frame 0x8c/0x94 and secondary-result tail remain;
// staged-byte negative point first 230; negative XZY/YXZ/YZX/ZXY/ZYX 11/17/14/18/18;
// staged paired axes 53; both points staged 5. All added-byte forms restored.
// @retail 0x1c8df0
short __stdcall function_1c8df0(long arg_0, point3f const *arg_4, short arg_3, short arg_2,
 void const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9)
{
 long category;
 real local_1;
 long local_2 = NONE;
 point3f const *local_3 = (point3f const *)arg_1;
 if (arg_9)
  *arg_9 = NONE;
 if (arg_2 != NONE && arg_3 != NONE && !(char)function_11c010(arg_2, arg_3))
 {
  category = 4;
  local_1 = 0.0f;
 }
 else
 {
  s_collision_result_1697c0 local_6;
  local_6.field_24 = NONE;
  long local_4 = arg_8 ? 0x15808c2f : 0x5808c2f;
  if (arg_6)
   local_4 |= 0x8000000;
  if (arg_7)
   local_4 &= ~0x20;
  long local_7;
  {
   vector3f local_5;
   vector3d_from_points3d(local_3, arg_4, &local_5);
   if (!function_1697c0(local_4, local_3, &local_5, arg_0, NONE, &local_6))
    local_7 = true;
   else
   {
    local_7 = false;
    local_1 = local_6.field_4;
    local_2 = local_6.field_40;
   }
  }
  if ((short)arg_5 != 0)
  {
   vector3f local_8;
   local_8.i = local_3->y - arg_4->y;
   local_8.j = arg_4->x - local_3->x;
   local_8.k = 0.0f;
   if (function_30bf0(&local_8) == 0.0f)
    local_8 = *g_4687a8;
   local_4 |= 0x10000000;
   /* retail calls the probe test at each of these */
#pragma inline_depth(0)
   if ((short)arg_5 == 1)
   {
    point3f local_9;
    point3f local_10;
    real const spacing = 0.25f;
    local_9.x = local_3->x + local_8.i * spacing;
    local_9.y = local_3->y + local_8.j * spacing;
    local_9.z = local_3->z + local_8.k * spacing;
    local_10.x = local_3->x - local_8.i * spacing;
    local_10.y = local_3->y - local_8.j * spacing;
    local_10.z = local_3->z - local_8.k * spacing;
    if (local_7)
    {
     category = function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (category == 0)
      category = function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
     if (category != 0)
     {
      local_2 = local_6.field_40;
      local_1 = local_6.field_4;
     }
    }
    else
    {
     category = !function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (category == 0)
      category = !function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
    }
   }
   else if (local_7)
   {
    point3f local_9;
    point3f local_10;
    point3f local_11;
    local_9.x = arg_4->x + local_8.i * 0.1f;
    local_9.y = arg_4->y + local_8.j * 0.1f;
    local_9.z = arg_4->z + local_8.k * 0.1f;
    local_10.x = arg_4->x - local_8.i * 0.1f;
    local_10.y = arg_4->y - local_8.j * 0.1f;
    local_10.z = arg_4->z - local_8.k * 0.1f;
    local_11.x = arg_4->x + g_4687bc->i * 0.1f;
    local_11.y = arg_4->y + g_4687bc->j * 0.1f;
    local_11.z = arg_4->z + g_4687bc->k * 0.1f;
    category = function_16a040(local_4, &local_9, local_3, arg_0, NONE, &local_6);
    if (category == 0)
     category = function_16a040(local_4, &local_10, local_3, arg_0, NONE, &local_6);
    if (category == 0)
     category = function_16a040(local_4, &local_11, local_3, arg_0, NONE, &local_6);
    if (category != 0)
    {
     local_2 = local_6.field_40;
     local_1 = local_6.field_4;
    }
   }
   else
    category = 0;
   if (category != 0)
   {
    goto local_14;
   }
#pragma inline_depth()
  }
  if (local_7)
   category = 0;
  else
  {
   vector3f local_13;
   vector3d_from_points3d(local_3, arg_4, &local_13);
   real local_12 = magnitude3d(&local_13);
   if (local_12 < 1.0f)
    category = 4;
   else if (local_12 * local_1 < 1.0f)
    category = 2;
   else if (!arg_8 && (1.0f - local_1) * local_12 < 4.0f)
    category = 3;
   else
    category = 4;
  }
 }
local_14:
 if (arg_9 && !arg_8 && category >= 1)
 {
  vector3f local_15;
  vector3d_from_points3d(local_3, arg_4, &local_15);
  if ((1.0f - local_1) * magnitude3d(&local_15) < 4.0f)
   *arg_9 = local_2;
 }
 return category;
}
