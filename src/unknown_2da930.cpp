// @flags /O2 /GL-
// Havok (third-party physics library): multiplies a 3x3 rotation of SSE
// columns by the transpose of another.
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
	void function_2da930(hkRotation const *arg_0, hkRotation const *arg_1);
};

static inline void swap_2da930(float &a, float &b)
{
	float t = a;
	a = b;
	b = t;
}

// @retail 0x2da930
void s_2da8c0::function_2da930(hkRotation const *arg_0, hkRotation const *arg_1)
{
	hkRotation transpose;
	transpose.m_col0.value = arg_1->m_col0.value;
	transpose.m_col1.value = arg_1->m_col1.value;
	transpose.m_col2.value = arg_1->m_col2.value;
	float (*rows)[4] = (float (*)[4])&transpose;
	swap_2da930(rows[1][0], rows[0][1]);
	swap_2da930(rows[2][0], rows[0][2]);
	swap_2da930(rows[2][1], rows[1][2]);
	function_2da8c0(arg_0, &transpose);
}
