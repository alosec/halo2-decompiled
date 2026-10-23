// @flags /O2 /GL-
// Havok (third-party physics library): a diagonal 3x3 matrix from three
// values.
#include <xmmintrin.h>

class class_31ab30
{
public:
	char unknown0[0xe0];
	float diagonal[3];

	void get_matrix(__m128 *rows) const;
};

// @retail 0x31ab30
void class_31ab30::get_matrix(__m128 *rows) const
{
	int z = *((int *)&diagonal[2]);
	float x = diagonal[0];
	float y = diagonal[1];
	float *first_row = (float *)&rows[0];
	float *second_row = (float *)&rows[1];
	float *r2 = (float *)&rows[2];
	__m128 zero = _mm_setzero_ps();
	_mm_store_ps((float *)&rows[0], zero);
	_mm_store_ps((float *)&rows[1], zero);
	_mm_store_ps((float *)&rows[2], zero);
	first_row[0] = x;
	second_row[1] = y;
	*(float *)&r2[2] = *(float *)&z;
}
