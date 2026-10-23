// @flags /O2 /GL-
// The bounds of one side of a pair query in the other side's frame, widened
// by the query's tolerance, and, for a query along a path, stretched by that
// path taken into the same frame.
#include "side_bounds_2e9140.h"

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
	void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

// a direction rotated by a transform's inverse (0x2da730)
struct vector_2da6e0
{
	__m128 value;
	void rotate(const c_query_transform *matrix, const __m128 *direction);
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

// where a body sits: two vectors, then its transform
struct placement_2e9140
{
	__m128 field_0;
	__m128 field_10;
	c_query_transform transform;
};

class shape_2e9140
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	// the shape's bounds under a transform, widened by a tolerance
	virtual void bounds(c_query_transform const *transform, float tolerance, bounds_2e9140 *out) const {}
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2e9140
{
	shape_2e9140 const *shape;
	long key;
	placement_2e9140 const *placement;
	body_2e9140 const *parent;
};

// a query's input: the dispatcher, the filter, the tolerance and, for a
// query along a path, the path
struct input_2e9140
{
	void *dispatcher;
	void *filter;
	float tolerance;
	long field_c;
	__m128 path;
};

// @retail 0x2e9140
void __cdecl function_2e9140(void const *a, void const *b, void const *input, bounds_2e9140 *bounds)
{
	body_2e9140 const *side_a = (body_2e9140 const *)a;
	body_2e9140 const *side_b = (body_2e9140 const *)b;
	input_2e9140 const *query = (input_2e9140 const *)input;
	c_query_transform a_in_b;
	a_in_b.inverse_product(&side_b->placement->transform, &side_a->placement->transform);
	side_a->shape->bounds(&a_in_b, query->tolerance, bounds);
	vector_2da6e0 path;
	path.inverse_rotate(&side_b->placement->transform, &query->path);
	__m128 zero = _mm_setzero_ps();
	_mm_store_ps((float *)&bounds->minimum, _mm_add_ps(bounds->minimum, _mm_min_ps(zero, path.value)));
	bounds->maximum = _mm_add_ps(bounds->maximum, _mm_max_ps(zero, path.value));
}

// @retail 0x2e91c0
void __cdecl function_2e91c0(void const *a, void const *b, void const *input, bounds_2e9140 *bounds)
{
	body_2e9140 const *side_a = (body_2e9140 const *)a;
	body_2e9140 const *side_b = (body_2e9140 const *)b;
	c_query_transform a_in_b;
	a_in_b.inverse_product(&side_b->placement->transform, &side_a->placement->transform);
	side_a->shape->bounds(&a_in_b, ((input_2e9140 const *)input)->tolerance, bounds);
}
