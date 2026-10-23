#ifndef CHILD_SHAPE_2DBFC0_H
#define CHILD_SHAPE_2DBFC0_H
// Havok (third-party physics library): an object around a child object with
// a radius, which takes its radius from the child and keeps a distance and
// its square, and a second distance with the square of how far it falls
// short of the first.
#include "shape_2de750.h"

// the description it is built from
struct info_2dbf10 : shape_info_2de750
{
	float radius;
	class radius_base_2dbfc0 *child;
	float distance;
	float second_distance;
};

class radius_base_2dbfc0 : public shape_2de750
{
public:
	float radius;

	radius_base_2dbfc0(info_2dbf10 const &info) : shape_2de750(info)
	{
		radius = info.radius;
	}
	radius_base_2dbfc0(float radius_)
	{
		radius = radius_;
	}
};

class c_2dbfc0 : public radius_base_2dbfc0
{
public:
	__m128 field_10;
	float distance_squared;
	float distance;
	float second_distance;
	float shortfall_squared;
	radius_base_2dbfc0 *child;
	long field_34;

	// the declaration game code calls (src/unknown_1d7390.cpp)
	c_2dbfc0(long arg_0, void *arg_1, float arg_2);
	c_2dbfc0(info_2dbf10 const &info);
};

#endif
