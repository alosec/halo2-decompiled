// @flags /O2 /GL-
// Havok (third-party physics library): returns a constant; builds a diagonal
// matrix of reciprocals.
#include <xmmintrin.h>

struct class_31aac0
{
    long function_31aac0();
};

// @retail 0x31aac0
long class_31aac0::function_31aac0()
{
    return 0x80;
}

// a diagonal 3x3 matrix of the reciprocals of three values
class class_31aad0
{
public:
    char unknown0[0xe0];
    float diagonal[3];

    void get_inverse_matrix(__m128 *rows) const;
};

// @retail 0x31aad0
void class_31aad0::get_inverse_matrix(__m128 *rows) const
{
    float x = 1.0f / diagonal[0];
    float y = 1.0f / diagonal[1];
    float z = 1.0f / diagonal[2];
    int z_bits = *(int *)&z;
    float *first_row = (float *)&rows[0];
    float *second_row = (float *)&rows[1];
    float *third_row = (float *)&rows[2];
    __m128 zero = _mm_setzero_ps();
    _mm_store_ps((float *)&rows[0], zero);
    _mm_store_ps((float *)&rows[1], zero);
    _mm_store_ps((float *)&rows[2], zero);
    first_row[0] = x;
    second_row[1] = y;
    *(float *)&third_row[2] = *(float *)&z_bits;
}
