// @flags /O2 /GL-
// Havok (third-party physics library): a body's description as it is built:
// at the origin, not turning or moving, a mass of one with the unit matrix
// and no offset, and its default values.
#include "motion_description_3115a0.h"

// the rotation that turns nothing
inline void set_unit_rotation_311340(__m128 &rotation)
{
	rotation = _mm_setzero_ps();
	rotation.m128_f32[3] = 1.0f;
}

// the unit matrix
inline void set_unit_matrix_311340(__m128 *rows)
{
	rows[0] = _mm_setzero_ps();
	rows[1] = _mm_setzero_ps();
	rows[2] = _mm_setzero_ps();
	rows[0].m128_f32[0] = 1.0f;
	rows[1].m128_f32[1] = 1.0f;
	rows[2].m128_f32[2] = 1.0f;
}

struct s_311340 : description_3115a0
{
	s_311340();
};

// @retail 0x311340
s_311340::s_311340()
{
	field_20 = _mm_setzero_ps();
	set_unit_rotation_311340(field_30);
	field_40 = _mm_setzero_ps();
	field_50 = _mm_setzero_ps();
	set_unit_matrix_311340(field_60);
	field_a0 = 1.0f;
	field_90 = _mm_setzero_ps();
	field_a4 = 0;
	// floats, though the description copies them as words
	*(float *)&field_a8 = 0.05f;
	*(float *)&field_ac = 0.5f;
	*(float *)&field_b0 = 0.4f;
	field_b4 = 1;
	field_b5 = 2;
}
