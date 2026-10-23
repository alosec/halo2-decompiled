// @flags /O2 /GL-
// Havok (third-party physics library): adding a vector scaled by the value
// at +0x2c.
#include <xmmintrin.h>

class class_319910
{
public:
	char unknown0[0x2c];
	float scale;
	char unknown30[0x10];
	__m128 sum;

	void add_scaled(const __m128 &vector);
};

// @retail 0x319910
void class_319910::add_scaled(const __m128 &vector)
{
	__m128 factor = _mm_set_ss(scale);
	factor = _mm_shuffle_ps(factor, factor, 0);
	sum = _mm_add_ps(sum, _mm_mul_ps(factor, vector));
}
