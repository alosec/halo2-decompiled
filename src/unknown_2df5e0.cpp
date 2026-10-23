// @flags /O2 /GL-
// Havok (third-party physics library): a library of surfaces queried for the
// keys of the surfaces a sphere, a box or a pair of bounds touches; each query
// is an object built on the stack whose constructor or method does the work, over the
// tree the library keeps at +0x10.
#include <xmmintrin.h>

struct s_surface_key_array
{
	unsigned long *keys;
	long count;
};

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

struct s_query_bounds
{
	__m128 minimum, maximum;
};

class class_3008d0
{
public:
	__m128 storage[2];

	void function_3008d0(void const *tree, const __m128 *sphere, s_surface_key_array *keys);
};

class class_2fff90
{
public:
	__m128 storage[4];

	void function_2fff90(void const *tree, const c_query_transform *matrix, const __m128 *extent, float tolerance, s_surface_key_array *keys);
};

class class_300150
{
public:
	__m128 storage[4];

	void function_300150(void const *tree, const s_query_bounds *bounds, s_surface_key_array *keys);
};

struct c_surface_query_library
{
	char unknown0[0x10];
	void const *tree;

	void query_sphere(const __m128 *sphere, s_surface_key_array *keys);
	void query_box(const c_query_transform *matrix, const __m128 *extent, float tolerance, s_surface_key_array *keys);
	void query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys);
};

// @retail 0x2df5e0
void c_surface_query_library::query_sphere(const __m128 *sphere, s_surface_key_array *keys)
{
	class_3008d0 query;
	query.function_3008d0(tree, sphere, keys);
}

// @retail 0x2df610
void c_surface_query_library::query_box(const c_query_transform *matrix, const __m128 *extent, float tolerance, s_surface_key_array *keys)
{
	class_2fff90 query;
	query.function_2fff90(tree, matrix, extent, tolerance, keys);
}

// @retail 0x2df640
void c_surface_query_library::query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys)
{
	class_300150 query;
	query.function_300150(tree, bounds, keys);
}
