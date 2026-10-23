// @flags /O2 /GL-
// Havok (third-party physics library): a diagonal 3x3 matrix holding the
// reciprocal of the value at +0x10.
#include <xmmintrin.h>

class class_319a20
{
public:
	char unknown0[0x10];
	float mass;

	void get_inverse(__m128 *rows) const;
};

// @retail 0x319a20
void class_319a20::get_inverse(__m128 *rows) const
{
	float inverse = 1.0f / mass;
	rows[0] = _mm_setzero_ps();
	rows[1] = _mm_setzero_ps();
	rows[2] = _mm_setzero_ps();
	rows[0].m128_f32[0] = inverse;
	rows[1].m128_f32[1] = inverse;
	rows[2].m128_f32[2] = inverse;
}
