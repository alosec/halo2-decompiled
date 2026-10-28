#pragma once
#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"

class c_contact_query_allocator
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void *allocate(long size, long kind) = 0;
};
PRIVATE __forceinline void *contact_query_allocate(long size)
{
 void *block = ((c_contact_query_allocator *)g_480118)->allocate(size, 0x2c);
 ((c_havok_reference_counted *)block)->allocation_size = (word)size;
 return block;
}
struct c_contact_query_bounds_info
{
 long filter;
 void *shape;
 long unknown08;
 s_havok_array properties;
 byte unknown18[8];
 hkVector4 lower, upper;
 c_contact_query_bounds_info();
 __forceinline ~c_contact_query_bounds_info()
 {
  if (!(properties.capacity_and_flags & 0x80000000))
   g_480118->allocate((long)properties.data, (properties.capacity_and_flags & 0x7fffffff) * 8, 0x12);
 }
};
struct c_contact_query_transform_info
{
 long filter;
 void *shape;
 long unknown08;
 s_havok_array properties;
 byte unknown18[8];
 hkTransform transform;
 c_contact_query_transform_info();
 __forceinline ~c_contact_query_transform_info()
 {
  if (!(properties.capacity_and_flags & 0x80000000))
   g_480118->allocate((long)properties.data, (properties.capacity_and_flags & 0x7fffffff) * 8, 0x12);
 }
};
struct s_contact_body_view;
class c_contact_query_bounds_volume
{
public:
 c_contact_query_bounds_volume(c_contact_query_bounds_info const *info);
 byte unknown00[0x80];
 s_contact_body_view **bodies;
 long count;
 byte unknown88[8];
 static void *operator new(size_t size) { return contact_query_allocate(size); }
};
class c_contact_query_transform_volume
{
public:
 c_contact_query_transform_volume(c_contact_query_transform_info const *info);
 virtual ~c_contact_query_transform_volume();
 byte unknown04[0xd0 - 4];
 static void *operator new(size_t size) { return contact_query_allocate(size); }
};
class c_contact_callback
{
public:
 __forceinline c_contact_callback() {}
 __forceinline c_contact_callback(bool value) : found(value) {}
 virtual ~c_contact_callback() {}
 virtual void hit(void const *query, s_contact_body_view const *body) = 0;
 bool found;
 byte unknown05[3];
 static void operator delete(void *block)
 {
  g_480118->allocate((long)block, 8, 0x1a);
 }
};
class c_contact_presence : public c_contact_callback
{
public:
 __forceinline c_contact_presence() : c_contact_callback(false) {}
 virtual void hit(void const *query, s_contact_body_view const *body);
 long unknown08;
};
class c_contact_query_dispatch
{
public:
 virtual void slot00() = 0;
 virtual void slot01() = 0;
 virtual void slot02() = 0;
 virtual void slot03() = 0;
 virtual void slot04() = 0;
 virtual void slot05() = 0;
 virtual void slot06() = 0;
 virtual void slot07() = 0;
 virtual void slot08() = 0;
 virtual void slot09() = 0;
 virtual void slot0a() = 0;
 virtual void slot0b() = 0;
 virtual void query(c_contact_callback *callback) = 0;
};
class c_contact_query_world
{
public:
 void *add(void *volume);
 void remove(void *volume);
};
