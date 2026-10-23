#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
// @flags /O2 /arch:SSE /Gr

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
void __stdcall function_1fc210(long arg_0, long arg_1);

// @retail 0x1c9c00
void function_1c9c00(long arg_0, long arg_1)
{
 if (g_4f55d0->active && arg_0 != NONE)
 {
  byte *local_0 = (byte *)function_badc0(arg_0, 3);
  if (local_0)
  {
   long local_1 = *(long *)(local_0 + 0x248) == NONE ? arg_0 : *(long *)(local_0 + 0x248);
   if (local_1 != NONE)
   {
    byte *local_2 = (byte *)havok_object_get(local_1);
    byte *local_3 = (byte *)havok_object_get(arg_1);
    if (*(long *)(local_3 + 0x12c) != NONE)
     function_1fc210(local_1, *(long *)(local_3 + 0x12c));
    if (*(long *)(local_2 + 0x12c) != NONE)
     function_1fc210(arg_1, *(long *)(local_2 + 0x12c));
   }
  }
 }
}
