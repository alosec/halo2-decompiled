#include "unknown_11c920.h"
#include <xmmintrin.h>
#include <math.h>
#include "globals.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

/* a datum of the linked list array g_4f55d4: the next link at +8 and the
   value returned at +4 */
struct s_linked_datum
{
	byte unknown00[4];
	long value;
	long next;
};

s_record_pool *g_4f55d4;

PRIVATE __forceinline long object_list_next_inlined(long *index)
{
	long result;
	if (*index != NONE)
	{
		long size = g_4f55d4->size;
		byte *data = g_4f55d4->data;
		s_linked_datum *datum = (s_linked_datum *)(data + (*index & 0xffff) * size);
		long next = datum->next;

		if (next != NONE)
			_mm_prefetch((const char *)(data + (next & 0xffff) * size), _MM_HINT_T0);

		*index = next;
		result = datum->value;
	}
	else
		result = NONE;
	return result;
}

// @retail 0x1dee80
long function_1dee80(long *index)
{
	if (*index != NONE)
	{
		long size = g_4f55d4->size;
		byte *data = g_4f55d4->data;
		s_linked_datum *datum = (s_linked_datum *)(data + (*index & 0xffff) * size);
		long next = datum->next;
		if (next != NONE)
			_mm_prefetch((const char *)(data + (next & 0xffff) * size), _MM_HINT_T0);
		*index = next;
		return datum->value;
	}
	return NONE;
}

struct s_counted_object_list
{
	byte unknown00[8];
	long first;
};

struct s_counted_object
{
	byte unknown000[0x10a];
	word unknown10a_0 : 2;
	word excluded : 1;
	word unknown10a_3 : 13;
};

struct s_counted_object_header
{
	byte unknown00[8];
	s_counted_object *object;
};

extern s_record_pool *g_4f55d8;

// @retail 0x1defb0
short function_1defb0(long list_index)
{
	struct { long reference; } state;
	long count = 0;
	if (list_index != NONE)
	{
		state.reference = ((s_counted_object_list *)g_4f55d8->data)[list_index & 0xffff].first;
		long object_index = function_1dee80(&state.reference);
		while (object_index != NONE)
		{
			s_counted_object *object = ((s_counted_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			if (!TEST_FIELD_BIT(object->excluded))
				count++;
			object_index = object_list_next_inlined(&state.reference);
		}
	}
	return (short)count;
}


struct s_mutable_object_list
{
 word salt;
 short field_02;
 short users;
 short count;
 long first;
};

s_record_pool *function_296270(char const *name, long size, long count);
void function_296330(s_record_pool *pool, long *head, long key);
bool function_296370(long *head, s_record_pool *pool, long key);
void function_2963b0(s_record_pool *pool, long index);

// @retail 0x1ded00
void function_1ded00(void)
{
 g_4f55d8 = data_new_inlined("object list header", 0x30, 12, 0, g_510c2c);
 g_4f55d4 = function_296270("list object", 0, 0x80);
}

// @retail 0x1dee00
bool function_1dee00(long list_index, long object_index)
{
 s_mutable_object_list *list = &((s_mutable_object_list *)g_4f55d8->data)[list_index & 0xffff];
 bool result = false;
 if (function_296370(&list->first, g_4f55d4, object_index))
 {
  function_296330(g_4f55d4, &list->first, object_index);
  --list->count;
  result = true;
 }
 return result;
}

// @retail 0x1deed0
void object_lists_garbage_collect(void)
{
 s_record_pool *lists = g_4f55d8;
 long index = data_datum_index(lists, function_16bc00(lists, 0));
 while (index != NONE)
 {
  s_mutable_object_list *list = &((s_mutable_object_list *)lists->data)[index & 0xffff];
  if (list->users == 0 && index != NONE)
  {
   #pragma inline_depth(0)
   function_2963b0(g_4f55d4, list->first);
   #pragma inline_depth()
   record_pool_release(lists, index);
  }
  index = data_datum_index(lists, data_next_absolute_index_inlined(lists, index == NONE ? 0 : (index & 0xffff) + 1));
 }
}


// @retail 0x1df080
void function_1df080(point3f const *points, long count, point3f *center, real *radius)
{
 (void)&count;
 (void)&center;
 (void)&radius;
 long b = 0;
 long a = 0;
 real maximum = 0.0f;
 if (count > 1)
 {
  for (long i = 0; i < count; ++i)
  {
   for (long j = i + 1; j < count; ++j)
   {
    vector3f delta;
    vector3d_from_points3d(&points[j], &points[i], &delta);
    real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
    if (distance > maximum)
    {
     maximum = distance;
     b = j;
     a = i;
    }
   }
  }
  real initial_radius = (real)sqrt(maximum) * 0.5f;
  point3f midpoint;
  midpoint.x = (points[b].x + points[a].x) * 0.5f;
  midpoint.y = (points[b].y + points[a].y) * 0.5f;
  midpoint.z = (points[b].z + points[a].z) * 0.5f;
  *center = midpoint;
  real larger_radius = 0.0f;
  for (long i = 0; i < count; ++i)
  {
   real x = center->x - points[i].x;
   real y = center->y - points[i].y;
   real z = center->z - points[i].z;
   real distance = (real)sqrt(x*x + y*y + z*z);
   if (distance > initial_radius && distance > larger_radius)
    larger_radius = distance;
  }
  if (larger_radius > 0.0f)
   *radius = larger_radius;
  else
   *radius = initial_radius;
 }
 else
 {
  *radius = 0.0f;
  if (count == 0)
   *center = *g_468788;
  else
   *center = points[0];
 }
}
