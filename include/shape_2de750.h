#ifndef SHAPE_2DE750_H
#define SHAPE_2DE750_H
// Havok (third-party physics library): reference-counted objects with a user
// value at +8: a radius-carrying kind (vtable 0x455388) and two of its
// derived kinds, and a kind that places a child object by a transform.
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>

// the start of every kind's description
struct shape_info_2de750
{
	long user_value;
};

class shape_2de750 : public referenced_2d88a0
{
public:
	long user_value;

	shape_2de750()
	{
		user_value = 0;
	}
	shape_2de750(shape_info_2de750 const &info)
	{
		user_value = info.user_value;
	}
};

struct radius_info_2de750 : shape_info_2de750
{
	float radius;
	long unknown8;
	long unknownc;
	__m128 first;
	__m128 second;
};

class radius_shape_2de750 : public shape_2de750
{
public:
	float radius;

	radius_shape_2de750(radius_info_2de750 const &info) : shape_2de750(info)
	{
		radius = info.radius;
	}
	radius_shape_2de750(float radius_)
	{
		radius = radius_;
	}
};

#endif
