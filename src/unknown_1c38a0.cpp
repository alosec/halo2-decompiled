#include "unknown_11c920.h"
#include "unknown_1cec30.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_1c38a0
{
 word field_0;
 byte field_2;
 byte field_3;
 dword field_4;
 byte *field_8;
};
void __stdcall function_e18d0(long arg_0, bool arg_1);
void __stdcall function_1c3770(long arg_0, dword arg_1);

// @retail 0x1c38a0
void __stdcall function_1c38a0(long arg_0)
{
 long const *local_5 = &arg_0;
 long local_6 = *local_5 & 0xffff;
 byte *local_2 = ((s_1c38a0 *)g_4e0300->data)[local_6].field_8;
 s_1c38a0 *local_1 = &((s_1c38a0 *)g_4e0300->data)[local_6];
 if ((1 << local_1->field_3) & 1)
 {
  byte *local_3 = ((s_1c38a0 *)g_4e0300->data)[local_6].field_8;
  if (local_3[0x34b] == 1 && local_3[0x3dc] == 3)
   function_e18d0(*local_5, 0);
 }
 long local_4 = *(long *)(local_2 + 0xb4);
 if (local_4 != NONE && (bool)((havok_component_get(local_4)->unknown04 >> 17) & 1))
  function_1c3770(*local_5, 0);
}
