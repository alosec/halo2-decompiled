// @flags /O2 /GL-
// Havok (third-party physics library): creation functions for small objects
// holding a value (their fourth argument) and a 16-bit index that starts at
// -1.
#include "reference_counted_2d88a0.h"
#include "timer_mark_2ebc80.h"
#include <math.h>
#include <xmmintrin.h>

class valued_2ef970
{
public:
	valued_2ef970(long value_) : value(value_) {}
	virtual ~valued_2ef970() {}
	unsigned short memory_size;
	unsigned short reference_count;
	long value;
};

class indexed_2ef970 : public valued_2ef970
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	short index;

	indexed_2ef970(long value_) : valued_2ef970(value_), index(-1) {}
};

// @retail 0x2ef970
indexed_2ef970 *__cdecl function_2ef970(long arg_0, long arg_1, long arg_2, long value)
{
	return new indexed_2ef970(value);
}

// two spheres closer than the sum of their radii and a tolerance, reported
// with a contact point: on the second sphere's surface along the normal from
// it to the first, with the separation in the point's fourth term

// the identity transform's first axis
static __declspec(align(16)) const float g_unit_x_461ea0[4] = {1.0f, 0.0f, 0.0f, 0.0f};

// a vector of four copies of a float
inline __m128 splat_2ef9a0(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

struct sphere_2ef9a0
{
	char unknown0[0xc];
	float radius;
};

struct motion_2ef9a0
{
	char unknown0[0x50];
	__m128 position;
};

struct body_2ef9a0
{
	sphere_2ef9a0 const *shape;
	long key;
	motion_2ef9a0 const *motion;
};

struct input_2ef9a0
{
	long field_0;
	long field_4;
	float tolerance;
};

// where two bodies touch: the point (its fourth term the separation), the
// normal and the two bodies
struct contact_2ef9a0
{
	__m128 point;
	__m128 normal;
	body_2ef9a0 const *a;
	body_2ef9a0 const *b;

	contact_2ef9a0(body_2ef9a0 const *a_, body_2ef9a0 const *b_) : a(a_), b(b_) {}
};

class collector_2ef9a0
{
public:
	virtual void slot_0() {}
	virtual void add_contact(contact_2ef9a0 const *contact) {}
};

class sphere_pair_2ef9a0
{
public:
	void function_2ef9a0(body_2ef9a0 const *a, body_2ef9a0 const *b, input_2ef9a0 const *input, collector_2ef9a0 *collector);
};

static __forceinline bool __cdecl report_contact_2ef9a0(body_2ef9a0 const *a, body_2ef9a0 const *b, input_2ef9a0 const *input, contact_2ef9a0 &contact)
{
	sphere_2ef9a0 const *sphere_a = a->shape;
	sphere_2ef9a0 const *sphere_b = b->shape;
	__m128 const &position_a = a->motion->position;
	__m128 const &position_b = b->motion->position;
	__m128 const offset = _mm_sub_ps(position_a, position_b);
	__m128 const squares = _mm_mul_ps(offset, offset);
	float distance_squared;
	_mm_store_ss(&distance_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	float const reach = input->tolerance + sphere_b->radius + sphere_a->radius;
	if (distance_squared < reach * reach)
	{
		float distance;
		if (distance_squared > 0.0f)
		{
			float inverse = 1.0f / (float)sqrt(distance_squared);
			contact.normal = _mm_mul_ps(splat_2ef9a0(inverse), offset);
			distance = distance_squared * inverse;
		}
		else
		{
			distance = 0.0f;
			contact.normal = *(__m128 const *)g_unit_x_461ea0;
		}
		float const radius_a = sphere_a->radius;
		float const radius_b = sphere_b->radius;
		distance -= radius_a + radius_b;
		contact.point = _mm_add_ps(position_b, _mm_mul_ps(splat_2ef9a0(sphere_b->radius), contact.normal));
		contact.point.m128_f32[3] = distance;
		return true;
	}
	return false;
}

// @retail 0x2ef9a0
void sphere_pair_2ef9a0::function_2ef9a0(body_2ef9a0 const *a, body_2ef9a0 const *b, input_2ef9a0 const *input, collector_2ef9a0 *collector)
{
	timer_add_2ebc80("TtSphereSphere");
	contact_2ef9a0 contact(a, b);
	if (report_contact_2ef9a0(a, b, input, contact))
	{
		collector->add_contact(&contact);
	}
	timer_add_2ebc80("Et");
}

// @retail 0x2efb10
void __cdecl function_2efb10(body_2ef9a0 const *a, body_2ef9a0 const *b, input_2ef9a0 const *input, collector_2ef9a0 *collector)
{
	timer_add_2ebc80("TtSphereSphere");
	contact_2ef9a0 contact(a, b);
	if (report_contact_2ef9a0(a, b, input, contact))
	{
		collector->add_contact(&contact);
	}
	timer_add_2ebc80("Et");
}
