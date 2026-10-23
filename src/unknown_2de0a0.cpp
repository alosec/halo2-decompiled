// @flags /O2 /GL-
// Havok (third-party physics library): a capsule built from its two points and
// its radius, with one reference.
#include "unknown_0a9f20.h"

// @retail 0x2de0a0
c_z_capsule_temp::c_z_capsule_temp(void const *first, void const *second, real radius)
{
	this->radius = radius;
	first_point = *(__m128 const *)first;
	second_point = *(__m128 const *)second;
	first_point.m128_f32[3] = radius;
	second_point.m128_f32[3] = radius;
}