#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
// @flags /O2 /Gr

bool function_e0dc0(long unit_index);
long __stdcall function_1d5940(s_havok_component *component, bool a, bool b, bool c);
void __stdcall function_1d01c0(s_havok_component *component);
void __stdcall function_1d6d00(s_havok_component *arg_0, void *arg_1, byte *arg_2,
 long arg_3, long arg_4, bool arg_5, bool arg_6);

// @retail 0x1d6b80
void function_1d6b80(s_havok_component *arg_0)
{
 long local_0 = arg_0->object_index;
 byte *local_1 = (byte *)havok_object_get(local_0);
 byte *local_2 = g_4e3b44[*(long *)local_1 & 0xffff].bytes;
 byte *local_3 = g_4e3b44[*(long *)(local_2 + 0x38) & 0xffff].bytes;
 bool local_4;
#pragma inline_depth(0)
 local_4 = (bool)((*(dword *)(local_1 + 0x134) >> 23) & 1) || function_e0dc0(local_0);
#pragma inline_depth()
 byte *local_5 = local_1 + 0x3dc;
 bool local_6 = *local_5 == 3;
 if (local_6 && *(long *)(local_3 + 0x24) != NONE &&
  (local_1[0x34b] == 1 || (bool)((*(dword *)(local_2 + 0x1f0) >> 11) & 1)))
 {
  arg_0->unknown1c = function_1d5940(arg_0, 0, 0, 1);
  if (!arg_0->rigid_bodies.size)
  {
   function_1d01c0(arg_0);
   function_1d6d00(arg_0, local_2 + 0x264, local_5, 9, 10, local_4, local_6);
  }
  else
   arg_0->unknown04 |= 0x40000;
 }
 else
  function_1d6d00(arg_0, local_2 + 0x264, local_5, 9, 10 + (local_1[0x34b] == 2), local_4, local_6);
}
