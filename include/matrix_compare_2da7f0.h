#ifndef MATRIX_COMPARE_2DA7F0_H
#define MATRIX_COMPARE_2DA7F0_H
// Havok (third-party physics library): a yes-or-no result returned through a
// hidden pointer, and the comparison of two 3x3 matrices of SSE columns
// within a tolerance (0x2da7f0).
#include <xmmintrin.h>

struct bool_result_2da7f0
{
    bool value;

    bool_result_2da7f0(bool value_)
    {
        value = value_;
    }
};

struct matrix_2da7f0
{
    __m128 column[3];

    bool_result_2da7f0 equal_within(matrix_2da7f0 const &other, float tolerance) const;
};

#endif
