#ifndef POINT_TRIANGLE_308D90_H
#define POINT_TRIANGLE_308D90_H
// Havok (third-party physics library): the closest point of a triangle to a
// point, from the triangle's corners and the terms that solve for a point's
// weights in its plane (0x308910).
#include <xmmintrin.h>

// the direction from the triangle to the point, and the distance between them
struct closest_308d90
{
	__m128 normal;
	float distance;
	char unknown14[0x20 - 0x14];
};

// a status for which part of the triangle is closest
int __cdecl function_308d90(__m128 const *point, __m128 const *corners, float const *terms, closest_308d90 *out);

#endif
