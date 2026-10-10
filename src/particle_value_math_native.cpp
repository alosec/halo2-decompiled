// Native source split of the compact recovered vector normalization used by
// particle interface-value evaluation. The matching Xbox definition remains
// in unknown_02b400.cpp, whose unrelated tail uses x86 SSE intrinsics.

#include "unknown_11c920.h"
#include "effects.h"
#include <math.h>

// @retail 0x4efb0
vector3f *function_4efb0(vector3f *value)
{
    real const length_squared =
        value->i * value->i + value->j * value->j + value->k * value->k;
    if (length_squared != 0.0f)
    {
        real const inverse = (real)(1.0f / sqrt(length_squared));
        value->i = (real)(value->i * inverse);
        value->j = (real)(value->j * inverse);
        value->k = (real)(value->k * inverse);
    }
    return value;
}
