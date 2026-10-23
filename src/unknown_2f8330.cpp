// @flags /O2 /GL-
// Havok (third-party physics library): queries of a set of spheres against a
// triangle: the triangle's corners and the spheres' centres taken out of
// their frames, and the pair passed on as soon as one sphere comes within
// the triangle's radius.
#include "pair_agents_2eab70.h"
#include "point_triangle_308d90.h"

void __cdecl function_308910(__m128 const *corners, float *out);

// a triangle with a radius
struct triangle_2f8330
{
	char unknown0[0xc];
	float radius;
	__m128 corners[3];
};

// a set of spheres: each centre, with its radius in the fourth term
struct spheres_2f8330
{
	char unknown0[0xc];
	long count;
	__m128 spheres[8];
};

struct transform_2f8330
{
	__m128 axes[3];
	__m128 position;
};

// where a body sits: two vectors, then its transform
struct placement_2f8330
{
	__m128 field_0;
	__m128 field_10;
	transform_2f8330 transform;
};

// one side of a query: a shape, a key, where it sits, and its parent side
struct body_2f8330
{
	void const *shape;
	long key;
	placement_2f8330 const *placement;
	body_2f8330 const *parent;
};

// told of each pair found touching
class collector_2f8330
{
public:
	virtual void slot_0() {}
	virtual void slot_1(body_2f8330 const *a, body_2f8330 const *b) {}
};

// a point taken out of a frame
inline void transform_2f8330_point(__m128 &out, transform_2f8330 const *frame, __m128 const &p)
{
	out = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(frame->axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(frame->axes[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(frame->axes[2], _mm_shuffle_ps(p, p, 0xaa)), frame->position));
}

// points taken out of a frame
__forceinline void transform_2f8330_points(__m128 *out, transform_2f8330 const *frame, __m128 const *in, int count)
{
	for (int i = count - 1; i >= 0; i--)
	{
		transform_2f8330_point(*out, frame, *in);
		out++;
		in++;
	}
}

// @retail 0x2f8330
void class_2f8330::function_2f8330(void const *a_, void const *b_, void const *input, void *collector)
{
	body_2f8330 const *a = (body_2f8330 const *)a_;
	body_2f8330 const *b = (body_2f8330 const *)b_;
	spheres_2f8330 const *spheres = (spheres_2f8330 const *)a->shape;
	triangle_2f8330 const *triangle = (triangle_2f8330 const *)b->shape;
	__m128 corners[3];
	__m128 *out = corners;
	__m128 const *corner = triangle->corners;
	transform_2f8330 const *frame = &b->placement->transform;
	for (int i = 0; i < 3; i++)
	{
		transform_2f8330_point(out[i], frame, corner[i]);
	}
	__m128 centres[8];
	__m128 const *const in = spheres->spheres;
	int const count = spheres->count;
	transform_2f8330_points(centres, &a->placement->transform, in, count);
	for (int i = 0; i < count; i++)
	{
		float const radii = triangle->radius + in[i].m128_f32[3];
		closest_308d90 closest;
		function_308d90(&centres[i], corners, terms, &closest);
		if (closest.distance < radii)
		{
			((collector_2f8330 *)collector)->slot_1(a, b);
			return;
		}
	}
}

// the same query with the terms for the triangle's plane worked out first
// @retail 0x2f84a0
void __cdecl function_2f84a0(void const *a_, void const *b_, void const *input, void *collector)
{
	body_2f8330 const *a = (body_2f8330 const *)a_;
	body_2f8330 const *b = (body_2f8330 const *)b_;
	spheres_2f8330 const *spheres = (spheres_2f8330 const *)a->shape;
	triangle_2f8330 const *triangle = (triangle_2f8330 const *)b->shape;
	__m128 corners[3];
	__m128 *out = corners;
	__m128 const *corner = triangle->corners;
	transform_2f8330 const *frame = &b->placement->transform;
	for (int i = 0; i < 3; i++)
	{
		transform_2f8330_point(out[i], frame, corner[i]);
	}
	__m128 centres[8];
	__m128 const *const in = spheres->spheres;
	int const count = spheres->count;
	transform_2f8330_points(centres, &a->placement->transform, in, count);
	float terms[4];
	function_308910(triangle->corners, terms);
	for (int i = 0; i < count; i++)
	{
		float const radii = triangle->radius + in[i].m128_f32[3];
		closest_308d90 closest;
		function_308d90(&centres[i], corners, terms, &closest);
		if (closest.distance < radii)
		{
			((collector_2f8330 *)collector)->slot_1(a, b);
			return;
		}
	}
}
