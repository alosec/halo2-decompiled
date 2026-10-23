#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
// @flags /O2 /Gr

void __stdcall function_1d3fc0(long arg_0, s_havok_component *arg_1);
struct s_1c4a80
{
 long field_0;
 union
 {
  dword field_4;
  struct
  {
   dword field_400 : 1;
   dword field_401 : 1;
   dword field_402 : 5;
   dword field_407 : 1;
   dword field_408 : 1;
   dword field_409 : 23;
  };
 };
 byte field_8[0xa0 - 8];
};

// @retail 0x1c4a80
void function_1c4a80(long arg_0, bool arg_1, bool arg_2)
{
 long local_0 = havok_object_get(arg_0)->havok_component_index;
 if (local_0 != NONE)
 {
  s_1c4a80 *local_1 = &((s_1c4a80 *)g_51e9b8->data)[local_0 & 0xffff];
  if (!(bool)local_1->field_401 || !(byte)arg_1)
  {
   bool local_2 = (bool)local_1->field_408;
   if (!local_2 && !(bool)local_1->field_407 || (byte)arg_2 && !local_2)
    function_1d3fc0(local_0, (s_havok_component *)local_1);
  }
 }
}
