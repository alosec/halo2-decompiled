// @flags /O2 /GL-
// Havok (third-party physics library): rotations held as four floats (an
// axis part and a scalar part): a product of two, a normalized copy with a
// non-negative scalar part, and a vector rotated by one.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long sign_mask_2db630[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a vector of four copies of a float
inline __m128 splat_2db630(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

struct rotation_2db630
{
	__m128 v;

	void function_2db630(rotation_2db630 const &a, rotation_2db630 const &b);
	void function_2db6f0(rotation_2db630 &out) const;
};

struct vector_2db7a0
{
	__m128 v;

	void function_2db7a0(rotation_2db630 const &rotation, vector_2db7a0 const &vector);
};

// the axis part: b's cross a, less a's scalar times b, plus b's scalar times
// a; the scalar part: the four-term dot product of a and b
// @retail 0x2db630
void rotation_2db630::function_2db630(rotation_2db630 const &a, rotation_2db630 const &b)
{
	v = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(b.v, b.v, 0xc9), _mm_shuffle_ps(a.v, a.v, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(b.v, b.v, 0xd2), _mm_shuffle_ps(a.v, a.v, 0xc9)));
	v = _mm_add_ps(v, _mm_mul_ps(splat_2db630(-a.v.m128_f32[3]), b.v));
	v = _mm_add_ps(v, _mm_mul_ps(splat_2db630(b.v.m128_f32[3]), a.v));
	__m128 const product = _mm_mul_ps(a.v, b.v);
	__m128 sum = _mm_add_ps(_mm_shuffle_ps(product, product, 0x4e), product);
	sum = _mm_add_ps(sum, _mm_shuffle_ps(sum, sum, 0xb1));
	float scalar;
	_mm_store_ss(&scalar, sum);
	v.m128_f32[3] = scalar;
}

// out: this rotation scaled by the inverse length of its axis part, negated
// when the scalar part is negative
// @retail 0x2db6f0
void rotation_2db630::function_2db6f0(rotation_2db630 &out) const
{
	out.v = v;
	__m128 const value = out.v;
	__m128 const squares = _mm_mul_ps(value, value);
	__m128 const sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	out.v = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), out.v);
	if (v.m128_f32[3] < 0.0f)
	{
		out.v = _mm_xor_ps(out.v, *(__m128 const *)sign_mask_2db630);
	}
}

// the vector rotated: (2w^2 - 1) v + 2 (q . v) q + 2 w (q x v), for the axis
// part q and scalar part w
// @retail 0x2db7a0
void vector_2db7a0::function_2db7a0(rotation_2db630 const &rotation, vector_2db7a0 const &vector)
{
	__m128 const b = vector.v;
	float const w = rotation.v.m128_f32[3];
	v = _mm_mul_ps(splat_2db630(w * w * 2.0f - 1.0f), b);
	__m128 axis = rotation.v;
	axis.m128_f32[3] = 0.0f;
	__m128 const product = _mm_mul_ps(axis, b);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	__m128 const cross = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(axis, axis, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(axis, axis, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
	v = _mm_add_ps(v, _mm_mul_ps(splat_2db630(dot * 2.0f), axis));
	v = _mm_add_ps(v, _mm_mul_ps(splat_2db630(w * 2.0f), cross));
}
