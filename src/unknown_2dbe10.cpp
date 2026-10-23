// @flags /O2 /GL-
// Havok (third-party physics library): a shape wrapper's bounding box: the
// wrapped shape's box, swept by its body's velocity over a time step.
#include <xmmintrin.h>

struct aabb_2dbe10
{
	__m128 minimum;
	__m128 maximum;
};

class shape_2dbe10
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void box_around(void *transform, float tolerance, aabb_2dbe10 *out) {}
};

struct world_2dbe10
{
	char pad_0[0x150];
	float time_step;
};

struct motion_2dbe10
{
	char pad_0[0x40];
	__m128 velocity;
};

struct body_2dbe10
{
	long field_0;
	long field_4;
	world_2dbe10 *world;
	char pad_c[0x30];
	motion_2dbe10 *motion;
};

class swept_shape_2dbe10
{
public:
	char pad_0[0x30];
	shape_2dbe10 *shape;
	body_2dbe10 *body;

	void box_around(void *transform, float tolerance, aabb_2dbe10 *out);
};

// @retail 0x2dbe10
void swept_shape_2dbe10::box_around(void *transform, float tolerance, aabb_2dbe10 *out)
{
	shape->box_around(transform, tolerance, out);
	body_2dbe10 *owner = body;
	__m128 sweep;
	if (owner && owner->world)
	{
		__m128 scalar = _mm_set_ss(owner->world->time_step);
		sweep = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), owner->motion->velocity);
	}
	else
	{
		sweep = _mm_setzero_ps();
	}
	__m128 maximum = out->maximum;
	out->maximum = _mm_max_ps(_mm_add_ps(maximum, sweep), maximum);
	__m128 minimum = out->minimum;
	out->minimum = _mm_min_ps(_mm_add_ps(minimum, sweep), minimum);
}
