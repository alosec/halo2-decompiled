// @flags /O2 /GL-
// Havok (third-party physics library): transforms a point or direction by a
// 3x4 matrix of SSE columns, or by its inverse.
#include <xmmintrin.h>

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
	void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

struct c_query_point
{
	__m128 value;
	void inverse_transform(const c_query_transform *matrix, const __m128 *point);
};

struct vector_2da6e0
{
	__m128 value;
	void rotate(const c_query_transform *matrix, const __m128 *direction);
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

// @retail 0x2da610
void c_transformed_point::transform(const void *matrix, const __m128 *point)
{
	const c_query_transform *m = (const c_query_transform *)matrix;
	__m128 p = *point;
	__m128 z = _mm_add_ps(_mm_mul_ps(m->axes[2], _mm_shuffle_ps(p, p, 0xaa)), m->position);
	__m128 y = _mm_mul_ps(m->axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 x = _mm_mul_ps(m->axes[0], _mm_shuffle_ps(p, p, 0x00));
	value = _mm_add_ps(_mm_add_ps(x, y), z);
}

// @retail 0x2da660
void c_query_point::inverse_transform(const c_query_transform *matrix, const __m128 *point)
{
	__m128 d = _mm_sub_ps(*point, matrix->position);
	__m128 c0 = matrix->axes[0];
	__m128 c1 = matrix->axes[1];
	__m128 c2 = matrix->axes[2];
	__m128 t0 = _mm_shuffle_ps(c0, c1, 0x44);
	__m128 t2 = _mm_shuffle_ps(c0, c1, 0xee);
	__m128 t1 = _mm_shuffle_ps(c2, c2, 0x44);
	__m128 t3 = _mm_shuffle_ps(c2, c2, 0xee);
	__m128 row_0 = _mm_shuffle_ps(t0, t1, 0x88);
	__m128 row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
	__m128 r2 = _mm_shuffle_ps(t2, t3, 0x88);
	value = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row_0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row_1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(r2, _mm_shuffle_ps(d, d, 0xaa)));
}

// @retail 0x2da6e0
void vector_2da6e0::rotate(const c_query_transform *matrix, const __m128 *direction)
{
	__m128 p = *direction;
	value = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(matrix->axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(matrix->axes[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_mul_ps(matrix->axes[2], _mm_shuffle_ps(p, p, 0xaa)));
}

// @retail 0x2da730
void vector_2da6e0::inverse_rotate(const c_query_transform *matrix, const __m128 *direction)
{
	__m128 c0 = matrix->axes[0];
	__m128 c1 = matrix->axes[1];
	__m128 c2 = matrix->axes[2];
	__m128 t0 = _mm_shuffle_ps(c0, c1, 0x44);
	__m128 t2 = _mm_shuffle_ps(c0, c1, 0xee);
	__m128 t1 = _mm_shuffle_ps(c2, c2, 0x44);
	__m128 t3 = _mm_shuffle_ps(c2, c2, 0xee);
	__m128 row_0 = _mm_shuffle_ps(t0, t1, 0x88);
	__m128 row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
	__m128 r2 = _mm_shuffle_ps(t2, t3, 0x88);
	__m128 d = *direction;
	value = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row_0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row_1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(r2, _mm_shuffle_ps(d, d, 0xaa)));
}
