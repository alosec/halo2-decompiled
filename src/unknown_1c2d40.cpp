#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"
#include <new>
// @flags /O2 /arch:SSE /Gr

class c_1c2d41
{
public:
 virtual void function_1c2d41() = 0;
 virtual void function_1c2d42() = 0;
 virtual void function_1c2d43() = 0;
 virtual void function_1c2d44() = 0;
 virtual void *function_1c2d45(long arg_0, long arg_1) = 0;
};

PRIVATE __forceinline void *function_1c2d41(long arg_0, long arg_1)
{
 void *local_0 = ((c_1c2d41 *)g_480118)->function_1c2d45(arg_0, arg_1);
 ((c_havok_reference_counted *)local_0)->allocation_size = (word)arg_0;
 return local_0;
}

#include "pair_filter_class.h"
class c_1c3450;
void function_1c3450(c_1c3450 *arg_0);
class c_1c2d40 : public c_2dfbf0
{
public:
 virtual ~c_1c2d40() { function_1c3450((c_1c3450 *)this); }
 static void *operator new(size_t arg_0) { return function_1c2d41(arg_0, 0x22); }
 static void operator delete(void *arg_0)
 {
  g_480118->allocate((long)arg_0, ((c_havok_reference_counted *)arg_0)->allocation_size, 0x22);
 }
};

struct s_3111d0
{
 hkVector4 field_0;
 byte field_10[0x10];
 hkVector4 field_20;
 hkVector4 field_30;
 long field_40;
 real field_44;
 real field_48;
 long field_4c;
 long field_50;
 real field_54;
 byte field_58[8];
 real field_60;
 long field_64;
 real field_68;
 real field_6c;
 long field_70;
 real field_74;
 byte field_78[0x18];
 s_3111d0();
};
struct s_30cb70
{
 byte field_0;
 s_30cb70(bool arg_0) : field_0(arg_0) {}
 s_30cb70(s_30cb70 const &arg_0) : field_0(arg_0.field_0) {}
};
struct s_1c2d42
{
 byte field_0[0x40];
 s_havok_array field_40;
 s_havok_array field_4c[3];
};
struct s_1c3a40;
void function_1c3a40(s_1c3a40 *arg_0);
struct s_278da0_group;
class c_interface_278b40
{
public:
 virtual ~c_interface_278b40();
 virtual void slot1(s_278da0_group *group);
 virtual void slot2(s_278da0_group *group);
};
class c_3101c0
{
public:
 c_3101c0(s_3111d0 const &arg_0, long arg_1);
 void function_30c1a0(byte *arg_0);
 void function_30cb70(s_30cb70 arg_0);
 void function_30c500();
 void function_310900(c_2dfbf0 *arg_0, s_30cb70 arg_1);
 void function_30cab0(c_interface_278b40 *arg_0);
 byte field_0[0xc4];
 s_1c2d42 *field_c4;
 s_1c3a40 *field_c8;
 byte field_cc[0x260 - 0xcc];
};
class c_2784c4;
class c_2784c0
{
public:
 c_2784c0(c_2784c4 *arg_0);
 byte field_0[0x14];
};
extern hkWorld *g_51e9a4;
extern c_havok_reference_counted *g_51e9a8;
extern c_havok_reference_counted *g_51e9ac;
extern byte *g_51eca8;
extern byte g_47f06f;
extern real g_47f05c;
byte g_47f06e = 1;
byte g_47f07d = 1;
real g_47f074 = -0.009840000420808792f;
byte *__fastcall function_30c170(hkWorld *world);
void __cdecl function_2d9160(void *array, long capacity, long element_size);
void __cdecl function_2dadb0(hkWorld *arg_0);

PRIVATE __forceinline void function_1c2d42(s_havok_array *arg_0, long arg_1, long arg_2)
{
 long local_0 = arg_0->capacity_and_flags & 0x7fffffff;
 if (local_0 < arg_1)
  function_2d9160(arg_0, local_0 * 2 > arg_1 ? local_0 * 2 : arg_1, arg_2);
}

PRIVATE __forceinline bool function_1c2d43(hkEntity const *arg_0, dword arg_1)
{
 for (long local_0 = 0; local_0 < arg_0->m_property_count; ++local_0)
  if (arg_0->m_properties[local_0].m_key == arg_1)
   return true;
 return false;
}

// @retail 0x1c2d40
void function_1c2d40()
{
 c_1c2d40 *local_0 = new c_1c2d40;
 s_3111d0 local_1;
 local_1.field_0.m_quad = _mm_setzero_ps();
 real const *local_2 = (real const *)((byte *)g_4e0348 + 0x1ec);
 hkVector4 local_3;
 local_3.set(64.0f, 64.0f, 64.0f);
 local_1.field_20.set(local_2[0], local_2[1], local_2[2]);
 local_1.field_30.set(local_2[3], local_2[4], local_2[5]);
 local_1.field_20.m_quad = _mm_sub_ps(local_1.field_20.m_quad, local_3.m_quad);
 local_1.field_30.m_quad = _mm_add_ps(local_1.field_30.m_quad, local_3.m_quad);
 local_1.field_44 = 0.6f;
 local_1.field_48 = 1.0f;
 local_1.field_60 = g_47f05c;
 local_1.field_54 = g_47f074;
 local_1.field_6c *= 0.328f;
 local_1.field_4c = 8;
 local_1.field_68 *= 0.328f;
 local_1.field_74 = 0.995f;
 c_3101c0 *local_4 = new (function_1c2d41(0x260, 0x2a)) c_3101c0(local_1, 0x4f4c);
 g_51e9a4 = (hkWorld *)local_4;
 s_1c2d42 *local_5 = local_4->field_c4;
 function_1c2d42(&local_5->field_40, 0x400, 0x10);
 s_havok_array *local_6 = local_5->field_4c;
 long local_7 = 3;
 do
 {
  function_1c2d42(local_6, 0x800, 4);
  ++local_6;
 } while (--local_7);
 hkEntity *local_8 = (hkEntity *)function_30c170(g_51e9a4);
 s_havok_property_word local_9(NONE);
 if (function_1c2d43(local_8, 0x2001))
  local_8->entity_property_erase(0x2001);
 local_8->entity_property_put(0x2001, local_9);
 g_51e9ac = (c_havok_reference_counted *)new (function_1c2d41(0x14, 0x10)) c_2784c0((c_2784c4 *)g_51e9a4);
 if (g_47f06e)
 {
  ((c_3101c0 *)g_51e9a4)->function_30c1a0(g_51eca8 + 0x1c);
  ((c_3101c0 *)g_51e9a4)->function_30cb70(s_30cb70(true));
 }
 else
  ((c_3101c0 *)g_51e9a4)->function_30c500();
 local_0->function_2df950(NONE, NONE);
 local_0->function_2df990(4, 15);
 local_0->function_2df990(4, 9);
 local_0->function_2df990(4, 12);
 local_0->function_2df990(4, 13);
 local_0->function_2df990(4, 14);
 local_0->function_2df990(15, 15);
 local_0->function_2df990(15, 9);
 local_0->function_2df990(10, 10);
 local_0->function_2df990(10, 11);
 local_0->function_2df990(10, 9);
 local_0->function_2df990(10, 15);
 local_0->function_2df990(10, 12);
 local_0->function_2df990(10, 13);
 local_0->function_2df990(10, 4);
 local_0->function_2df990(10, 3);
 local_0->function_2df990(10, 14);
 local_0->function_2df990(11, 10);
 local_0->function_2df990(11, 11);
 local_0->function_2df990(11, 9);
 local_0->function_2df990(11, 15);
 local_0->function_2df990(11, 12);
 local_0->function_2df990(11, 13);
 local_0->function_2df990(11, 4);
 local_0->function_2df990(11, 3);
 local_0->function_2df990(11, 5);
 local_0->function_2df990(11, 14);
 local_0->function_2df990(11, 8);
 local_0->function_2df990(11, 6);
 local_0->function_2df990(11, 7);
 local_0->function_2df990(14, 1);
 local_0->function_2df990(8, 8);
 local_0->function_2df990(8, 1);
 local_0->function_2df990(7, 1);
 local_0->function_2df990(2, 11);
 local_0->function_2df990(2, 10);
 local_0->function_2df990(2, 9);
 local_0->function_2df990(2, 12);
 local_0->function_2df990(2, 13);
 local_0->function_2df990(2, 3);
 local_0->function_2df990(2, 5);
 local_0->function_2df990(2, 4);
 local_0->function_2df990(2, 15);
 local_0->function_2df990(2, 8);
 local_0->function_2df990(2, 6);
 local_0->function_2df990(2, 7);
 local_0->function_2df990(2, 14);
 local_0->function_2df990(13, 9);
 ((c_3101c0 *)g_51e9a4)->function_310900(local_0, s_30cb70(false));
 havok_reference_remove((c_havok_reference_counted *)local_0);
 function_1c3a40(((c_3101c0 *)g_51e9a4)->field_c8);
 if (g_47f06f)
  function_2dadb0(g_51e9a4);
 if (g_47f07d)
 {
  c_interface_278b40 *local_10 = new c_interface_278b40;
  g_51e9a8 = (c_havok_reference_counted *)local_10;
  ((c_3101c0 *)g_51e9a4)->function_30cab0(local_10);
 }
}
