// @flags /O2 /GL-
// Havok (third-party physics library): a query of a sphere against a
// triangle: the sphere's centre taken into the triangle's frame, and the pair
// passed on when the closest point of the triangle lies within their radii.
#include "pair_agents_2eab70.h"
#include "point_triangle_308d90.h"
#include "timer_mark_2ebc80.h"

void __cdecl function_308910(__m128 const *corners, float *out);

// a shape with a radius: a sphere, or a triangle whose corners follow
struct rounded_2f1560
{
	char unknown0[0xc];
	float radius;
	__m128 corners[3];
};

struct transform_2f1560
{
	__m128 axes[3];
	__m128 position;
};

// where a body sits: two vectors, then its transform
struct placement_2f1560
{
	__m128 field_0;
	__m128 field_10;
	transform_2f1560 transform;
};

// one side of a query: a shape, a key, where it sits, and its parent side
struct body_2f1560
{
	rounded_2f1560 const *shape;
	long key;
	placement_2f1560 const *placement;
	body_2f1560 const *parent;
};

// told of each pair found touching
class collector_2f1560
{
public:
	virtual void slot_0() {}
	virtual void slot_1(body_2f1560 const *a, body_2f1560 const *b) {}
};

// @retail 0x2f1560
void class_2f1560::function_2f1560(void const *a_, void const *b_, void const *input, void *collector)
{
	body_2f1560 const *a = (body_2f1560 const *)a_;
	body_2f1560 const *b = (body_2f1560 const *)b_;
	timer_add_2ebc80("TtSphereTriangle");
	rounded_2f1560 const *sphere = a->shape;
	rounded_2f1560 const *triangle = b->shape;
	transform_2f1560 const *frame = &b->placement->transform;
	__m128 const d = _mm_sub_ps(a->placement->transform.position, frame->position);
	__m128 row0 = frame->axes[0];
	__m128 row1 = frame->axes[1];
	__m128 row2 = frame->axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 centre = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
	closest_308d90 closest;
	function_308d90(&centre, triangle->corners, terms, &closest);
	if (triangle->radius + sphere->radius > closest.distance)
	{
		((collector_2f1560 *)collector)->slot_1(a, b);
	}
	timer_add_2ebc80("Et");
}

// the same query with the terms for the triangle's plane worked out first
// @retail 0x2f1690
void __cdecl function_2f1690(void const *a_, void const *b_, void const *input, void *collector)
{
	float terms[4];
	body_2f1560 const *a = (body_2f1560 const *)a_;
	body_2f1560 const *b = (body_2f1560 const *)b_;
	timer_add_2ebc80("TtSphereTriangle");
	rounded_2f1560 const *sphere = a->shape;
	rounded_2f1560 const *triangle = b->shape;
	function_308910(triangle->corners, terms);
	transform_2f1560 const *frame = &b->placement->transform;
	__m128 const d = _mm_sub_ps(a->placement->transform.position, frame->position);
	__m128 row0 = frame->axes[0];
	__m128 row1 = frame->axes[1];
	__m128 row2 = frame->axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 centre_in_frame = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
	closest_308d90 closest;
	function_308d90(&centre_in_frame, triangle->corners, terms, &closest);
	if (triangle->radius + sphere->radius > closest.distance)
	{
		((collector_2f1560 *)collector)->slot_1(a, b);
	}
	timer_add_2ebc80("Et");
}
