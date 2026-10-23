// @flags /O2 /GL-
// Havok (third-party physics library): a diagonal 3x3 matrix holding the
// value at +0x10.
#include <xmmintrin.h>

inline void set_diagonal_319af0(__m128 *rows, float x, float y, float z)
{
	__m128 zero = _mm_setzero_ps();
	_mm_store_ps((float *)&rows[0], zero);
	_mm_store_ps((float *)&rows[1], zero);
	_mm_store_ps((float *)&rows[2], zero);
	rows[0].m128_f32[0] = x;
	rows[1].m128_f32[1] = y;
	rows[2].m128_f32[2] = z;
}

class class_319af0
{
public:
	char unknown0[0x10];
	float value;

	void get_matrix(__m128 *rows) const;
};

// @retail 0x319af0
void class_319af0::get_matrix(__m128 *rows) const
{
	set_diagonal_319af0(rows, value, value, value);
}
