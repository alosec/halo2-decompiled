// @flags /O2 /GL-
// Havok (third-party physics library): the contacts of a shape of several
// spheres with a triangle: the triangle's corners and the spheres' centres
// taken into the world, and each sphere close enough to the triangle passed
// to the collector as a contact. The query is marked in the timing stream.
#include "timer_mark_2ebc80.h"
#include "point_triangle_308d90.h"

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

// count points taken out of a transform's frame, one after another (each as
// 0x2da610 does it, inlined here)
__forceinline void __cdecl transform_points_2f7f10(c_query_transform const *m, __m128 *out, __m128 const *point, int count)
{
	while (--count >= 0)
	{
		__m128 p = *point;
		__m128 z = _mm_add_ps(_mm_mul_ps(m->axes[2], _mm_shuffle_ps(p, p, 0xaa)), m->position);
		__m128 y = _mm_mul_ps(m->axes[1], _mm_shuffle_ps(p, p, 0x55));
		__m128 x = _mm_mul_ps(m->axes[0], _mm_shuffle_ps(p, p, 0x00));
		*out = _mm_add_ps(_mm_add_ps(x, y), z);
		out++;
		point++;
	}
}

// where a body sits: two vectors, then its transform
struct placement_2f7f10
{
	__m128 field_0;
	__m128 field_10;
	c_query_transform transform;
};

// a triangle with a radius
struct triangle_2f7f10
{
	void const *table;
	long field_4;
	long field_8;
	float radius;
	__m128 corners[3];
};

// spheres, each a centre with its radius in the fourth lane
struct spheres_2f7f10
{
	void const *table;
	long field_4;
	long field_8;
	long count;
	__m128 spheres[8];
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2f7f10
{
	void const *shape;
	long key;
	placement_2f7f10 const *placement;
	body_2f7f10 const *parent;
};

struct input_2f7f10
{
	long field_0;
	long field_4;
	float tolerance;
};

// a contact: its point, with the separation in the fourth lane, its normal,
// and the two sides
struct contact_2f7f10
{
	__m128 point;
	__m128 normal;
	void const *a;
	void const *b;
};

// takes the contacts a query finds
class collector_2f7f10
{
public:
	virtual void slot_0() {}
	virtual void add_contact(contact_2f7f10 const *contact) {}
};

// an agent holding the terms that solve for a point's weights in the
// triangle's plane (0x308910)
class class_2f7f10
{
public:
	char unknown0[0xc];
	float terms[4];

	void function_2f7f10(void const *a, void const *b, void const *input, void *collector);
};

// @retail 0x2f7f10
void class_2f7f10::function_2f7f10(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtMultiSphereTriangle");
	body_2f7f10 const *side_a = (body_2f7f10 const *)a;
	body_2f7f10 const *side_b = (body_2f7f10 const *)b;
	spheres_2f7f10 const *spheres = (spheres_2f7f10 const *)side_a->shape;
	triangle_2f7f10 const *triangle = (triangle_2f7f10 const *)side_b->shape;
	__m128 corners[3];
	transform_points_2f7f10(&side_b->placement->transform, corners, triangle->corners, 3);
	__m128 const *sphere_values = spheres->spheres;
	long const sphere_count = spheres->count;
	__m128 centres[8];
	transform_points_2f7f10(&side_a->placement->transform, centres, sphere_values, sphere_count);
	contact_2f7f10 contact;
	contact.a = a;
	contact.b = b;
	if (sphere_count > 0)
	{
		float const *triangle_terms = terms;
		__m128 const *centre = centres;
		__m128 const *sphere = sphere_values;
		for (int count = sphere_count; count > 0; count--)
		{
			float const radii = triangle->radius + sphere->m128_f32[3];
			closest_308d90 closest;
			function_308d90(centre, corners, triangle_terms, &closest);
			if (radii + ((input_2f7f10 const *)input)->tolerance > closest.distance)
			{
				__m128 const single = _mm_set_ss(triangle->radius - closest.distance);
				contact.point = _mm_add_ps(*centre, _mm_mul_ps(_mm_shuffle_ps(single, single, 0), closest.normal));
				contact.point.m128_f32[3] = closest.distance - radii;
				contact.normal = closest.normal;
				((collector_2f7f10 *)collector)->add_contact(&contact);
			}
			centre++;
			sphere++;
		}
	}
	timer_add_2ebc80("Et");
}
