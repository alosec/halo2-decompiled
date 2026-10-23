// @flags /O2 /GL-
// Havok (third-party physics library): setting a vector together with its
// length, and a word.
#include <xmmintrin.h>

class class_30b710
{
public:
	char unknown0[8];
	long field_8;
	char unknownc[4];
	__m128 vector;
	char unknown20[4];
	float length;

	void set(__m128 const *vector_, long value);
};

// @retail 0x30b710
void class_30b710::set(__m128 const *vector_, long value)
{
	vector = *vector_;
	__m128 d = _mm_mul_ps(*vector_, *vector_);
	__m128 sum = _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d));
	float result;
	_mm_store_ss(&result, _mm_sqrt_ss(sum));
	length = result;
	field_8 = value;
}
