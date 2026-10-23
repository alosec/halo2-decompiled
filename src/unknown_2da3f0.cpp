// @flags /O2 /GL-
// Havok (third-party physics library): a transform set to the inverse of
// another, and a transform multiplied in place by another.
#include "unknown_1cec30.h"
#include "unknown_182d90.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

class class_2da7a0
{
public:
	float rows[3][4];

	void set_transpose(const class_2da7a0 &other);
};

// @retail 0x2da3f0
void hkTransform::transform_set_to_inverse(hkTransform const &t)
{
	((class_2da7a0 *)this)->set_transpose(*(class_2da7a0 const *)&t);
	__m128 p = _mm_xor_ps(t.m_translation.m_quad, *(const __m128 *)g_4124a0);
	m_translation.m_quad = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(m_rotation.m_col0.m_quad, _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(m_rotation.m_col1.m_quad, _mm_shuffle_ps(p, p, 0x55))),
		_mm_mul_ps(m_rotation.m_col2.m_quad, _mm_shuffle_ps(p, p, 0xaa)));
}

// @retail 0x2da5d0
void hkTransform::transform_compose_in_place(hkTransform const &b)
{
	hkTransform a;
	a.set(*this);
	((s_extent_transform *)this)->compose((s_extent_transform const *)&a, (s_extent_transform const *)&b);
}
