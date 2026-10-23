// @flags /O2 /GL-
// Havok (third-party physics library): composes two 3x4 transforms of SSE
// columns.
#include "unknown_182d90.h"

static inline void rotate_2da450(__m128 &out, const __m128 *m, const __m128 &p)
{
	out = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(m[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(m[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_mul_ps(m[2], _mm_shuffle_ps(p, p, 0xaa)));
}

// @retail 0x2da450
void s_extent_transform::compose(s_extent_transform const *a, s_extent_transform const *b)
{
	for (int i = 0; i < 4; i++)
	{
		rotate_2da450(rows[i], a->rows, b->rows[i]);
	}
	rows[3] = _mm_add_ps(rows[3], a->rows[3]);
}
