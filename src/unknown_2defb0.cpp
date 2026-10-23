// @flags /O2 /GL-
// Havok (third-party physics library): a vector holding only the value at
// +0xc, in w.
#include <xmmintrin.h>

class class_2defb0
{
public:
	long field_0;
	long field_4;
	long field_8;
	float field_c;

	__m128 *function_2defb0(__m128 *out) const;
};

// @retail 0x2defb0
__m128 *class_2defb0::function_2defb0(__m128 *out) const
{
	__m128 value;
	value.m128_f32[3] = field_c;
	value.m128_f32[0] = 0.0f;
	value.m128_f32[1] = 0.0f;
	value.m128_f32[2] = 0.0f;
	*out = value;
	return out;
}
