// @flags /O2 /GL-
// Havok (third-party physics library): multiplies two 3x3 rotations of SSE
// columns.
#include <xmmintrin.h>

struct hkVector4
{
	__m128 value;
};

struct hkRotation
{
	hkVector4 m_col0;
	hkVector4 m_col1;
	hkVector4 m_col2;
};

struct s_2da8c0
{
	hkRotation field_0;
	void function_2da8c0(hkRotation const *arg_0, hkRotation const *arg_1);
};

static inline void rotate_2da8c0(hkVector4 &out, const hkVector4 *m, const __m128 &p)
{
	out.value = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(m[0].value, _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(m[1].value, _mm_shuffle_ps(p, p, 0x55))),
		_mm_mul_ps(m[2].value, _mm_shuffle_ps(p, p, 0xaa)));
}

// @retail 0x2da8c0
void s_2da8c0::function_2da8c0(hkRotation const *arg_0, hkRotation const *arg_1)
{
	hkVector4 *out = &field_0.m_col0;
	const hkVector4 *in = &arg_1->m_col0;
	for (int i = 0; i < 3; i++)
	{
		rotate_2da8c0(out[i], &arg_0->m_col0, in[i].value);
	}
}
