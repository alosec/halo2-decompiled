// @flags /O2 /GL-
// Havok (third-party physics library): a transform (three axes and a
// position) set to the inverse of one transform times another.
#include <xmmintrin.h>

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;

	void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

// the rows of a transform's axes: its transpose
inline void transpose_2da4d0(const c_query_transform *t, __m128 &row0, __m128 &row1, __m128 &row2)
{
	row0 = t->axes[0];
	row1 = t->axes[1];
	row2 = t->axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
}

// a vector times the matrix whose rows are given
inline __m128 rotate_2da4d0(__m128 const &row0, __m128 const &row1, __m128 const &row2, __m128 const &v)
{
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(v, v, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(v, v, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(v, v, 0xaa)));
}

// this = a's inverse times b: b's axes and position rotated by a's transposed
// axes, then a's position, rotated the same way, taken off
// @retail 0x2da4d0
void c_query_transform::inverse_product(const c_query_transform *a, const c_query_transform *b)
{
	__m128 row0;
	__m128 row1;
	__m128 row2;
	transpose_2da4d0(a, row0, row1, row2);
	for (long i = 0; i < 4; i++)
	{
		(&axes[0])[i] = rotate_2da4d0(row0, row1, row2, (&b->axes[0])[i]);
	}
	transpose_2da4d0(a, row0, row1, row2);
	position = _mm_sub_ps(position, rotate_2da4d0(row0, row1, row2, a->position));
}
