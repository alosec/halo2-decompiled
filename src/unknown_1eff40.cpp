// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1EFF40.CPP: casts a ray against each shape of a lookup (built with
   SSE, apart from its neighbours in unknown_1efac0.cpp) */

#include <string.h>
#include <float.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"
#include "unknown_1c3b70.h"

struct s_table;
struct s_iterator
{
	s_table *table;
	signed char bytes[16];
	union
	{
		dword position;
		struct
		{
			byte first;
			byte item;
			byte sub;
			byte group;
		};
	};
	void *current;

	bool advance();
};

struct s_mix_output;
struct s_mix_source;
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices);

struct s_1de6d2;
struct s_1de6d3
{
 real field_0;
 plane3f const *field_4;
 long field_8;
 long field_c;
 long field_10;
 byte field_14;
 byte field_15;
 short field_16;
 long field_18;
 long field_1c[256];
};

struct s_1de6d0
{
 dword field_0;
 s_1de6d2 const *field_4;
 short field_8;
 dword const *field_c;
 point3f const *field_10;
 vector3f const *field_14;
 s_1de6d3 *field_18;
 long field_1c;
 byte field_20;
 long field_24;
};

struct s_lookup_ray_result
{
 dword position;
 short node;
 short item;
 s_1de6d3 hit;
};

void function_141590(const transform4x3f *matrix, transform4x3f *inverse);
bool __stdcall function_1de6d0(s_1de6d0 *query, long root, real start, real end);

// @retail 0x1eff40
bool function_1eff40(s_lookup *lookup, long flags, const point3f *point,
 const vector3f *direction, s_lookup_ray_result *result)
{
 (void)&lookup; (void)&flags; (void)&point; (void)&direction; (void)&result;
 s_lookup_ray_result *const *result_reference = &result;
 s_1de6d3 *hit = &(*result_reference)->hit;
 bool found = false;
 s_iterator iterator;
 hit->field_0 = FLT_MAX;
 function_1efbd0((dword)lookup->tag_b, (s_mix_output *)&iterator,
  (s_mix_source *)lookup->tag_a, (signed char *)lookup->pointer_a);
 while (iterator.advance())
 {
  dword position = iterator.position;
  long node = iterator.position & 0xff;
  transform4x3f inverse;
  function_141590((const transform4x3f *)lookup->pointer_b + node, &inverse);
  point3f local_point;
  real x = point->x, y = point->y, z = point->z;
  if (inverse.scale != 1.0f)
  {
   x = inverse.scale * x;
   y = inverse.scale * y;
   z = inverse.scale * z;
  }
  local_point.x = inverse.up.i * z + inverse.left.i * y + inverse.forward.i * x + inverse.position.x;
  local_point.y = inverse.up.j * z + inverse.left.j * y + inverse.forward.j * x + inverse.position.y;
  local_point.z = inverse.up.k * z + inverse.left.k * y + inverse.forward.k * x + inverse.position.z;
  vector3f local_direction;
  x = direction->i; y = direction->j; z = direction->k;
  if (inverse.scale != 1.0f)
  {
   x *= inverse.scale;
   y *= inverse.scale;
   z *= inverse.scale;
  }
  local_direction.i = inverse.up.i * z + inverse.left.i * y + inverse.forward.i * x;
  local_direction.j = inverse.up.j * z + inverse.left.j * y + inverse.forward.j * x;
  local_direction.k = inverse.up.k * z + inverse.left.k * y + inverse.forward.k * x;
  s_1de6d0 query;
  query.field_0 = flags;
  query.field_4 = (s_1de6d2 const *)iterator.current;
  query.field_8 = 0;
  query.field_c = NULL;
  query.field_10 = &local_point;
  query.field_14 = &local_direction;
  query.field_18 = hit;
  query.field_1c = NONE;
  query.field_20 = 0;
  query.field_24 = NONE;
  real fraction = hit->field_0;
  real bounded = fraction < 0.0f ? 0.0f : fraction;
  hit->field_0 = bounded;
  hit->field_18 = 0;
  bounded = bounded > 1.0f ? 1.0f : bounded;
  if (function_1de6d0(&query, 0, 0.0f, bounded))
  {
   (*result_reference)->position = position;
   (*result_reference)->node = (short)node;
   (*result_reference)->item = iterator.group;
   found = true;
  }
 }
 return found;
}
