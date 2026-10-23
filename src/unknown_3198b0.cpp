// @flags /O2 /GL-
// Havok (third-party physics library): a body's velocity at a point: the
// linear velocity plus the angular velocity crossed with the point's offset
// from the centre of mass.
#include <xmmintrin.h>

class motion_3198b0
{
public:
	char unknown0[0x40];
	__m128 linear_velocity;
	__m128 angular_velocity;
	char unknown60[0x10];
	__m128 mass_centre;

	void velocity_at(__m128 const *point, __m128 *out) const;
};

// @retail 0x3198b0
void motion_3198b0::velocity_at(__m128 const *point, __m128 *out) const
{
	__m128 const offset = _mm_sub_ps(*point, mass_centre);
	__m128 const w = angular_velocity;
	*out = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(w, w, 0xc9), _mm_shuffle_ps(offset, offset, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(w, w, 0xd2), _mm_shuffle_ps(offset, offset, 0xc9)));
	*out = _mm_add_ps(*out, linear_velocity);
}
