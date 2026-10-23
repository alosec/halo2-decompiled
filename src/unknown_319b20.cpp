// @flags /O2 /GL-
// Havok (third-party physics library): an impulse applied to a body at a
// point: its velocity changed by the impulse over its mass, and its turning
// velocity by the turn the impulse gives about its position.
#include <xmmintrin.h>

// a vector of four copies of a float
inline __m128 splat_319b20(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the cross product of two vectors
inline __m128 cross_319b20(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// a vector updated in place
struct vector_319b20
{
	__m128 value;

	void add_product(__m128 const &a, __m128 const &b) { value = _mm_add_ps(value, _mm_mul_ps(a, b)); }
};

class body_319b20
{
public:
	char unknown0[0x10];
	float inverse_inertia;
	char unknown14[0x2c - 0x14];
	float inverse_mass;
	char unknown30[0x40 - 0x30];
	vector_319b20 velocity;
	vector_319b20 turning_velocity;
	char unknown60[0x70 - 0x60];
	__m128 position;

	void apply_impulse_at(const __m128 &impulse, const __m128 &point);
};

// @retail 0x319b20
void body_319b20::apply_impulse_at(const __m128 &impulse, const __m128 &point)
{
	velocity.add_product(splat_319b20(inverse_mass), impulse);
	__m128 const arm = _mm_sub_ps(point, position);
	turning_velocity.add_product(splat_319b20(inverse_inertia), cross_319b20(arm, impulse));
}
