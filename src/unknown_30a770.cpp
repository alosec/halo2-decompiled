// @flags /O2 /GL-
// Havok (third-party physics library): narrows an interval along a direction
// by a point's projection: lowers the upper end when the point lies inside,
// raises the lower end (by a small margin) when it lies below; 4 when the
// interval is empty.
#include <xmmintrin.h>

class slab_30a770
{
public:
	__m128 direction;
	long field_10;
	__m128 const *origin;

	int function_30a770(__m128 const *point, float *low, float *high) const;
};

// @retail 0x30a770
int slab_30a770::function_30a770(__m128 const *point, float *low, float *high) const
{
	__m128 const d = _mm_mul_ps(direction, _mm_sub_ps(*point, *origin));
	float t;
	_mm_store_ss(&t, _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d)));
	int result = 1;
	if (t > *low)
	{
		if (t < *high)
		{
			*high = t;
		}
	}
	else
	{
		t += 1e-05f;
		if (t > *low)
		{
			*low = t;
		}
		result = 2;
	}
	if (*low >= *high)
	{
		result = 4;
	}
	return result;
}
