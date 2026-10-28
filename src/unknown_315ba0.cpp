// @flags /O2 /GL-
// Havok (third-party physics library): a description with a transform, built
// as the identity.
#include "description_312430.h"

// the unit matrix
inline void __cdecl set_unit_matrix_315ba0(__m128 &first, __m128 &second, __m128 &third)
{
	first = _mm_setzero_ps();
	second = _mm_setzero_ps();
	third = _mm_setzero_ps();
	first.m128_f32[0] = 1.0f;
	second.m128_f32[1] = 1.0f;
	third.m128_f32[2] = 1.0f;
}

// the transform that moves nothing
inline void __cdecl set_unit_transform_315ba0(transform_312630 &transform)
{
	set_unit_matrix_315ba0(transform.rotation.column[0], transform.rotation.column[1], transform.rotation.column[2]);
	transform.translation = _mm_setzero_ps();
}

// @retail 0x315ba0
description_315ba0::description_315ba0()
{
	set_unit_transform_315ba0(transform);
}
