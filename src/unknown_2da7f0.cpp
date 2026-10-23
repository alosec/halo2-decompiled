// @flags /O2 /GL-
// Havok (third-party physics library): compares two 3x4 matrices of SSE
// columns within a tolerance.
#include "matrix_compare_2da7f0.h"

// every bit but the sign bit, in four lanes
__declspec(align(16)) static const unsigned long absolute_mask_455940[4] = {
    0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
};

// @retail 0x2da7f0
bool_result_2da7f0 matrix_2da7f0::equal_within(matrix_2da7f0 const &other, float tolerance) const
{
    __m128 const mask = *(__m128 const *)absolute_mask_455940;
    __m128 const limit = _mm_set1_ps(tolerance);
    return bool_result_2da7f0(
        (_mm_movemask_ps(_mm_cmplt_ps(limit, _mm_and_ps(_mm_sub_ps(column[0], other.column[0]), mask))) & 7) == 0 &&
        (_mm_movemask_ps(_mm_cmplt_ps(limit, _mm_and_ps(_mm_sub_ps(column[1], other.column[1]), mask))) & 7) == 0 &&
        (_mm_movemask_ps(_mm_cmplt_ps(limit, _mm_and_ps(_mm_sub_ps(column[2], other.column[2]), mask))) & 7) == 0);
}
