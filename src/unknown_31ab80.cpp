// @flags /O2 /GL-
// Havok (third-party physics library): an object's rotated diagonal matrix,
// R * D * R^T, and the same with the diagonal's reciprocals.
#include <xmmintrin.h>

static inline __m128 splat_31ab80(float value)
{
	__m128 v = _mm_set_ss(value);
	return _mm_shuffle_ps(v, v, 0);
}

// the declarations the called rotation functions use (src/unknown_2da930.cpp)
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

	void function_2da930(hkRotation const *arg_0, hkRotation const *arg_1);
	void function_2da9a0(hkRotation const *arg_0);
};

struct inertia_value_31ab80
{
	__m128 value;
	float &operator[](int i) { return value.m128_f32[i]; }
	float const &operator[](int i) const { return value.m128_f32[i]; }
};
class class_31ab80
{
public:
	char unknown0[0x2c];
	float inverse_mass;
	char unknown30[0x40];
	__m128 balance_point;
	hkRotation rotation;
	char unknownb0[0x30];
	inertia_value_31ab80 diagonal;
	__m128 force;
	__m128 torque;

	void function_31ab80(s_2da8c0 *out) const;
	void function_31ac20(s_2da8c0 *out) const;
	void function_31ac80(float value);
	void function_31aca0(__m128 const &value);
	void function_31ad50(__m128 const &value);
	void function_31adf0(hkRotation const &value);
	void function_31ae40(hkRotation const &value);
};

// @retail 0x31ab80
void class_31ab80::function_31ab80(s_2da8c0 *out) const
{
	hkRotation scaled;
	scaled.m_col0.value = _mm_mul_ps(splat_31ab80(diagonal[0]), rotation.m_col0.value);
	scaled.m_col1.value = _mm_mul_ps(splat_31ab80(diagonal[1]), rotation.m_col1.value);
	scaled.m_col2.value = _mm_mul_ps(splat_31ab80(diagonal[2]), rotation.m_col2.value);
	out->function_2da930(&scaled, &rotation);
}

// @retail 0x31ac20
// the diagonal's reciprocals, written into the three rows of the matrix: the
// elements are written through float pointers, so the compiler keeps retail's
// order of the two stores at the end
void class_31ab80::function_31ac20(s_2da8c0 *out) const
{
	hkRotation *rows = &out->field_0;
	float x = 1.0f / diagonal[0];
	float y = 1.0f / diagonal[1];
	float z = 1.0f / diagonal[2];
	rows->m_col0.value = _mm_setzero_ps();
	rows->m_col1.value = _mm_setzero_ps();
	rows->m_col2.value = _mm_setzero_ps();
	float *first_row = (float *)&rows->m_col0.value;
	float *second_row = (float *)&rows->m_col1.value;
	float *r2 = (float *)&rows->m_col2.value;
	first_row[0] = x;
	second_row[1] = y;
	r2[2] = z;
	out->function_2da9a0(&rotation);
}

// Store inverse mass both in the motion prefix and the diagonal vector.
// @retail 0x31ac80
void class_31ab80::function_31ac80(float value)
{
	inverse_mass = value;
	diagonal[3] = value;
}
// @retail 0x31aca0
void class_31ab80::function_31aca0(__m128 const &value)
{
	force = _mm_add_ps(force, value);
}
// @retail 0x31ad50
void class_31ab80::function_31ad50(__m128 const &value)
{
	torque = _mm_add_ps(torque,value);
	torque.m128_f32[3] = 1.0f;
}
// @retail 0x31adf0
void class_31ab80::function_31adf0(hkRotation const &value)
{
	float x = 1.0f / value.m_col0.value.m128_f32[0];
	float y = 1.0f / value.m_col1.value.m128_f32[1];
	float z = 1.0f / value.m_col2.value.m128_f32[2];
	float mass = diagonal[3];
	diagonal[0]=x;
	diagonal[1]=y;
	diagonal[2]=z;
	diagonal[3]=mass;
}
// @retail 0x31ae40
void class_31ab80::function_31ae40(hkRotation const &value)
{
	float mass=diagonal[3];
	float z=value.m_col2.value.m128_f32[2];
	float y=value.m_col1.value.m128_f32[1];
	float x=value.m_col0.value.m128_f32[0];
	diagonal[0]=x;
	diagonal[1]=y;
	diagonal[2]=z;
	diagonal[3]=mass;
}
