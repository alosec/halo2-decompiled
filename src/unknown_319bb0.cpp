// @flags /O2 /GL-
// Havok (third-party physics library): accumulating vectors into a body's
// sums.
#include <xmmintrin.h>

class class_319bb0
{
public:
	char unknown0[0x10];
	float scale;
	char unknown14[0x3c];
	__m128 scaled_sum;
	char unknown60[0x60];
	__m128 sum;

	void add_scaled(const __m128 &vector);
	void add(const __m128 &vector);
};

// @retail 0x319bb0
void class_319bb0::add_scaled(const __m128 &vector)
{
	__m128 factor = _mm_set_ss(scale);
	factor = _mm_shuffle_ps(factor, factor, 0);
	scaled_sum = _mm_add_ps(scaled_sum, _mm_mul_ps(factor, vector));
}

// @retail 0x319bf0
void class_319bb0::add(const __m128 &vector)
{
	sum = _mm_add_ps(sum, vector);
}
