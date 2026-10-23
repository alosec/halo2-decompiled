// @flags /O2 /GL-
// Havok (third-party physics library): how far an object reaches along a
// direction, from its own bounding box.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long sign_mask_2df220[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a vector of four copies of a float
inline __m128 splat_2df220(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a 3x4 transform of SSE columns
struct transform_2df220
{
	__m128 columns[4];

	void load_unit_columns()
	{
		__m128 zero = _mm_setzero_ps();
		columns[0] = zero;
		columns[1] = zero;
		columns[2] = zero;
		float *elements = (float *)columns;
		elements[0] = 1.0f;
		elements[5] = 1.0f;
		elements[10] = 1.0f;
		columns[3] = zero;
	}
};

struct bounds_2df220
{
	__m128 lower;
	__m128 upper;
};

class shape_2df220
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	// the bounding box under a transform, grown by a tolerance
	virtual void slot_6(transform_2df220 const &transform, float tolerance, bounds_2df220 &out) const {}

	float function_2df220(__m128 const &direction) const;
};

// the corner of the untransformed bounding box furthest along the direction,
// measured along it
// @retail 0x2df220
float shape_2df220::function_2df220(__m128 const &direction) const
{
	transform_2df220 identity;
	identity.load_unit_columns();
	bounds_2df220 bounds;
	slot_6(identity, 0.0f, bounds);
	__m128 const d = direction;
	__m128 const lower = bounds.lower;
	__m128 const upper = bounds.upper;
	__m128 const center = _mm_mul_ps(splat_2df220(0.5f), _mm_add_ps(upper, lower));
	__m128 const extent = _mm_mul_ps(splat_2df220(0.5f), _mm_sub_ps(upper, lower));
	__m128 const corner = _mm_add_ps(_mm_xor_ps(extent, _mm_and_ps(d, *(__m128 const *)sign_mask_2df220)), center);
	__m128 const product = _mm_mul_ps(corner, d);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}
