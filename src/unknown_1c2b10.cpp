#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_iterator.h"
#include <float.h>
// @flags /O2 /arch:SSE /Gr

struct s_278300_references;
void function_278300(s_278300_references *references);
void function_146bf0();
void function_278f00();
extern byte *g_51eca8;
extern hkWorld *g_51e9a4;
extern long *g_51e9a0;
struct s_1c2b10
{
 byte field_0[0x24];
 long field_24;
};
struct s_1c2b11
{
 byte field_0[0xb4];
 long field_b4;
 byte field_b8[8];
 byte field_c0;
};
struct s_1c2b12
{
 s_object *field_0;
 s_type_f1af8e field_4;
};
struct s_311100
{
 real field_0;
 real field_4;
};
class c_311100
{
public:
 void function_311100(s_311100 const *arg_0);
};

PRIVATE __forceinline void havok_object_uncount_1c2b10(long object_index)
{
 s_havok_object *object = havok_object_get(object_index);
 if (TEST_FIELD_BIT(object->havok_flag))
 {
  object->havok_flag = 0;
  (*g_51e9a0)--;
 }
}

PRIVATE __forceinline void function_1c2b13(long arg_0)
{
 s_havok_component *local_0 = havok_component_get(arg_0);
 long local_1 = local_0->object_index;
 local_0->~s_havok_component();
 record_pool_release(g_51e9b8, arg_0);
 havok_object_uncount_1c2b10(local_1);
}

// @retail 0x1c2b10
void function_1c2b10()
{
 function_278300((s_278300_references *)g_51eca8);
 function_146bf0();
 if (((s_1c2b10 *)g_51e9a4)->field_24 > 0)
  ((s_1c2b10 *)g_51e9a4)->field_24 = 0;
 {
  long local_0 = 0;
  while (local_0 < 3)
  {
   s_1c2b12 local_1;
   local_1.field_4.type_mask = NONE;
   local_1.field_4.object_index = NONE;
   local_1.field_4.signature = 0x86868686;
   local_1.field_4.flags = 0;
   local_1.field_4.index = 0;
   s_type_f1af8e *iterator = &local_1.field_4;
   while ((local_1.field_0 = function_baeb0(iterator)) != NULL)
   {
    long local_2 = iterator->object_index;
    s_1c2b11 *local_3 = (s_1c2b11 *)havok_object_get(local_2);
    bool local_4 = (bool)(((dword)local_3->field_c0 >> 6) & 1);
    if (local_4)
     function_146bf0();
    s_havok_object *local_5 = havok_object_get(local_2);
    if (local_5->havok_component_index != NONE)
    {
     function_1c2b13(local_5->havok_component_index);
     local_5->havok_component_index = NONE;
    }
    havok_object_uncount_1c2b10(local_2);
    if (local_4)
    {
     function_278f00();
     function_146bf0();
    }
   }
   ++local_0;
   if (g_51e9b8 && g_51e9b8->actual_count)
    continue;
   break;
  }
 }
 {
  s_311100 local_7;
  _control87(0x9001f, 0x8001f);
  _mm_setcsr(_mm_getcsr() | 0x1f80);
  local_7.field_0 = 0.01f;
  local_7.field_4 = 100.0f;
  ((c_311100 *)g_51e9a4)->function_311100(&local_7);
  _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 }
 _clearfp();
 _control87(0x9001f, 0xfffff);
}
