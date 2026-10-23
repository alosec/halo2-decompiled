// @flags /O2 /GL-
// Havok (third-party physics library): a rotation's product of itself by the
// transpose of another, then by that other again, through a temporary.
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
	void function_2da9a0(hkRotation const *arg_0);
};

// @retail 0x2da9a0
void s_2da8c0::function_2da9a0(hkRotation const *arg_0)
{
	s_2da8c0 temporary;
	temporary.function_2da930(&field_0, arg_0);
	function_2da8c0(arg_0, &temporary.field_0);
}
