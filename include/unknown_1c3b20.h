#ifndef UNKNOWN_1C3B20_H
#define UNKNOWN_1C3B20_H
#include "unknown_1eb350.h"
#include "havok_reference.h"
#include <xmmintrin.h>
struct s_surface_key_array;
struct c_query_transform;
struct s_query_bounds;
struct c_shape_owner : c_shape_library_base_a
{
	c_havok_reference_counted *object;
	c_shape_owner(c_havok_reference_counted *arg_0, long arg_1);
	virtual ~c_shape_owner();
 virtual void v1() {}
 virtual void v2() {}
 virtual void v3() {}
 virtual void v4() {}
 virtual void v5() {}
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void query_sphere(const __m128 *sphere, s_surface_key_array *keys);
 virtual void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys);
 virtual void query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys);
};
#endif
