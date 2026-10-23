// @flags /O2 /GL-
// Havok (third-party physics library): the point of a segment closest to a
// given point.
#include <xmmintrin.h>

class segment_2ddfc0
{
public:
	void function_2ddfc0(__m128 const &point, __m128 const &start, __m128 const &end, __m128 &out);
};

// three-term dot product of two vectors
inline void dot3_2ddfc0(float *result, __m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	_mm_store_ss(result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
}

// out: start when the point projects before it, end when it projects past
// it, else the projection itself
// @retail 0x2ddfc0
void segment_2ddfc0::function_2ddfc0(__m128 const &point, __m128 const &start, __m128 const &end, __m128 &out)
{
	__m128 const b = start;
	__m128 const c = end;
	__m128 const direction = _mm_sub_ps(c, b);
	__m128 const product = _mm_mul_ps(direction, _mm_sub_ps(b, point));
	float along;
	_mm_store_ss(&along, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	float length_squared;
	dot3_2ddfc0(&length_squared, direction, direction);
	if ((along = -along) <= 0.0f)
	{
		out = b;
	}
	else if (along >= length_squared)
	{
		out = c;
	}
	else
	{
		__m128 const single = _mm_set_ss(along / length_squared);
		out = _mm_add_ps(b, _mm_mul_ps(_mm_shuffle_ps(single, single, 0), direction));
	}
}
