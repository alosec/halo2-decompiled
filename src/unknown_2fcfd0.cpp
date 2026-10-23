// @flags /O2 /GL-
// Havok (third-party physics library): the corner of a triangle furthest
// along a direction.
#include <xmmintrin.h>

class triangle_2fcfd0
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	__m128 vertices[3];

	void function_2fcfd0(__m128 const *direction, __m128 *out) const;
};

// out: the corner with the largest dot product with the direction, its
// fourth word the corner's offset (0x00, 0x10 or 0x20) tagged with
// 0x3f000000
// @retail 0x2fcfd0
void triangle_2fcfd0::function_2fcfd0(__m128 const *direction, __m128 *out) const
{
	__m128 row0 = vertices[0];
	__m128 row1 = vertices[1];
	__m128 row2 = vertices[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 const d = *direction;
	__m128 dots = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
	long offset;
	float best;
	if (dots.m128_f32[2] > dots.m128_f32[1])
	{
		best = dots.m128_f32[2];
		offset = 2 * sizeof(__m128);
	}
	else
	{
		best = dots.m128_f32[1];
		offset = sizeof(__m128);
	}
	if (dots.m128_f32[0] > best)
	{
		offset = 0;
	}
	*out = *(__m128 const *)((char const *)vertices + offset);
	((long *)out)[3] = offset | 0x3f000000;
}

struct description_2fd8d0;

class blocked_convex_2fd8d0
{
public:
	void function_2fd8d0(description_2fd8d0 *out) const;
};

// the method above called as a free function
// @retail 0x2fd090
void __cdecl function_2fd090(blocked_convex_2fd8d0 const *object, description_2fd8d0 *out)
{
	object->function_2fd8d0(out);
}
