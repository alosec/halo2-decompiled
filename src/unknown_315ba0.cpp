// @flags /O2 /GL-
// Havok (third-party physics library): a description with a transform, built
// as the identity.
#include "description_312430.h"

// the unit matrix
inline void set_unit_matrix_315ba0(__m128 *rows)
{
	rows[0] = _mm_setzero_ps();
	rows[1] = _mm_setzero_ps();
	rows[2] = _mm_setzero_ps();
	rows[0].m128_f32[0] = 1.0f;
	rows[1].m128_f32[1] = 1.0f;
	rows[2].m128_f32[2] = 1.0f;
}

// the transform that moves nothing
inline void set_unit_transform_315ba0(transform_312630 &transform)
{
	set_unit_matrix_315ba0(transform.rotation.column);
	transform.translation = _mm_setzero_ps();
}

// @retail 0x315ba0
description_315ba0::description_315ba0()
{
	set_unit_transform_315ba0(transform);
}
