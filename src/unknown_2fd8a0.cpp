#include "unknown_11c920.h"

#ifndef H2_NATIVE_PORT
#include <xmmintrin.h>
#endif

// @flags /O2 /Gr

struct s_physics_offset_box_matrix
{
    float rows[4][4];
};
static_assert(sizeof(s_physics_offset_box_matrix) == 0x40,
    "offset-box matrix layout");

struct s_physics_offset_box_bounds
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_offset_box_bounds) == 0x20,
    "offset-box bounds layout");

union u_physics_offset_box_float
{
    float real;
    dword bits;
};

static float physics_offset_box_absolute(float value)
{
    u_physics_offset_box_float converted;
    converted.real = value;
    converted.bits &= 0x7fffffff;
    return converted.real;
}

#ifndef H2_NATIVE_PORT
void __cdecl function_1ce210(const __m128 *matrix, const __m128 *extent,
    const __m128 *center, real radius, __m128 *bounds);
#endif

class c_physics_offset_box_shape
{
    byte field_4[8];
    float radius;
    float extent[4];
    float center[4];
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void function_2fd8a0(const s_physics_offset_box_matrix *matrix,
        float extra_radius, s_physics_offset_box_bounds *result);
};
static_assert(sizeof(c_physics_offset_box_shape) == 0x30,
    "offset-box shape layout");

// @retail 0x2fd8a0
void c_physics_offset_box_shape::function_2fd8a0(
    const s_physics_offset_box_matrix *matrix,
    float extra_radius, s_physics_offset_box_bounds *result)
{
    float combined_radius = radius + extra_radius;
#ifdef H2_NATIVE_PORT
    for (long component = 0; component < 4; component++)
    {
        float x = physics_offset_box_absolute(
            matrix->rows[0][component] * extent[0]);
        float y = physics_offset_box_absolute(
            matrix->rows[1][component] * extent[1]);
        float z = physics_offset_box_absolute(
            matrix->rows[2][component] * extent[2]);
        float half_extent = (x + y) + (z + combined_radius);
        float transformed_center =
            matrix->rows[0][component] * center[0] +
            matrix->rows[1][component] * center[1] +
            matrix->rows[2][component] * center[2] +
            matrix->rows[3][component];
        result->maximum[component] = transformed_center + half_extent;
        result->minimum[component] = transformed_center - half_extent;
    }
#else
    function_1ce210((const __m128 *)matrix, (const __m128 *)extent,
        (const __m128 *)center, combined_radius, (__m128 *)result);
#endif
}
