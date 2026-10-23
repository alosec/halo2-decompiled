#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_1fb7e0.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

void __stdcall function_1fbac0(long unknown, long unit_index, bool unknown2, long unknown3, s_1fbac0_event *event);
struct s_1ca130
{
 byte field_0[0xc];
 long field_c;
 long field_10;
 byte field_14[0xaa - 0x14];
 byte field_aa;
 byte field_ab[0x12c - 0xab];
 long field_12c;
 byte field_130[0xa];
 short field_13a;
 byte field_13c[8];
 long field_144;
};

PRIVATE __forceinline s_1ca130 *function_1ca131(long arg_0)
{
 return (s_1ca130 *)havok_object_get(arg_0);
}

// @retail 0x1ca130
void function_1ca130(long arg_0, long arg_1, long arg_2)
{
 if (g_4f55d0->active && arg_0 != NONE && arg_2 > 0)
 {
  s_1ca130 *local_0 = function_1ca131(arg_0);
  long local_1 = local_0->field_13a;
  long local_2 = g_510c54->game_time;
  if (arg_1 <= local_1)
  {
   real local_3 = (real)g_510c54->field_2_3;
   long local_4;
   __asm { fld local_3 }
   __asm { fistp local_4 }
   if (local_2 <= local_0->field_144 + local_4)
    return;
  }
  s_1fbac0_event local_5;
  memset(&local_5.data, 0, 12);
  local_0->field_144 = local_2;
  byte local_8 = *(volatile byte const *)&local_0->field_aa;
  local_5.unknown00 = (short)arg_1;
  local_5.unknown02 = (short)arg_2;
  memset((byte *)&local_5.data + 12, 0, sizeof(local_5.data) - 12);
  local_0->field_13a = (short)arg_1;
  if (local_8 == 1)
  {
   long local_6 = local_0->field_10;
   while (local_6 != NONE)
   {
    s_1ca130 *local_7 = function_1ca131(local_6);
    if (local_7->field_aa == 0)
     ((void (__stdcall *)(long, long, long, long, s_1fbac0_event *))function_1fbac0)(local_7->field_12c, local_6, 3, 1, &local_5);
    local_6 = local_7->field_c;
   }
  }
  else if (local_8 == 0)
   ((void (__stdcall *)(long, long, long, long, s_1fbac0_event *))function_1fbac0)(local_0->field_12c, arg_0, 3, 1, &local_5);
 }
}
