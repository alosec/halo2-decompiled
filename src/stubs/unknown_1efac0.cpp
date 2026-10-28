// stub for the c_a virtual in the library range
#include "unknown_1efac0.h"
#include "unknown_1eb350.h"
#include <xmmintrin.h>

// Name virtuals are now implemented in unknown_2d7120.cpp.


struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};


struct s_surface_key_array
{
 dword *keys;
 long count;
};

struct c_query_transform
{
 __m128 axes[3];
 __m128 position;
 void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

struct c_query_point
{
 __m128 value;
 void inverse_transform(const c_query_transform *matrix, const __m128 *point);
};

struct s_query_bounds
{
 __m128 minimum, maximum;
};

struct c_surface_query_library
{
 void query_sphere(const __m128 *sphere, s_surface_key_array *keys);
 void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys);
 void query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys);
};


class c_havok_reference_counted;
struct c_child_transform
{
 c_child_transform(c_havok_reference_counted *child);
};

