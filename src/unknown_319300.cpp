// @flags /O2 /GL-
// Havok (third-party physics library): the product of two rotations held as
// quaternions.
#include <xmmintrin.h>

// the cross product of two vectors' first three terms
inline __m128 cross_319300(__m128 const &a, __m128 const &b)
{
	__m128 b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	__m128 second = _mm_mul_ps(a_zxy, b_yzx);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

// all four terms set to one value
inline __m128 broadcast_319300(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

class quaternion_319300
{
public:
	__m128 vector;

	// this = a * b
	void function_319300(quaternion_319300 const &a, quaternion_319300 const &b);
};

// @retail 0x319300
void quaternion_319300::function_319300(quaternion_319300 const &a, quaternion_319300 const &b)
{
	vector = cross_319300(a.vector, b.vector);
	vector = _mm_add_ps(vector, _mm_mul_ps(broadcast_319300(a.vector.m128_f32[3]), b.vector));
	vector = _mm_add_ps(vector, _mm_mul_ps(broadcast_319300(b.vector.m128_f32[3]), a.vector));
	__m128 products = _mm_mul_ps(a.vector, b.vector);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(products, products, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(products, products, 0x55), products)));
	vector.m128_f32[3] = a.vector.m128_f32[3] * b.vector.m128_f32[3] - dot;
}
