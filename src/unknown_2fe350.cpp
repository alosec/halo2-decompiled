#include "unknown_11c920.h"

// @flags /O2 /Gr

struct __declspec(align(16)) s_physics_bounds_matrix
{
    float rows[4][4];
};
static_assert(sizeof(s_physics_bounds_matrix) == 0x40, "physics matrix layout");

struct __declspec(align(16)) s_physics_bounds
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_bounds) == 0x20, "physics bounds layout");

union u_physics_bounds_float
{
    float real;
    dword bits;
};

static float physics_absolute(float value)
{
    u_physics_bounds_float converted;
    converted.real = value;
    converted.bits &= 0x7fffffff;
    return converted.real;
}

// @retail 0x2fe350
void __cdecl function_2fe350(const s_physics_bounds_matrix *matrix,
    const float *extent, float extra_radius, s_physics_bounds *result)
{
    for (long component = 0; component < 4; component++)
    {
        float x = physics_absolute(matrix->rows[0][component] * extent[0]);
        float y = physics_absolute(matrix->rows[1][component] * extent[1]);
        float z = physics_absolute(matrix->rows[2][component] * extent[2]);
        float half_extent = (x + y) + (z + extra_radius);
        float center = matrix->rows[3][component];
        result->maximum[component] = center + half_extent;
        result->minimum[component] = center - half_extent;
    }
}

class c_physics_box_shape
{
    byte field_0[0x0c];
    float radius;
    float extent[4];
public:
    void function_2fe420(const s_physics_bounds_matrix *matrix,
        float extra_radius, s_physics_bounds *result);
};
static_assert(sizeof(c_physics_box_shape) == 0x20, "box shape layout");

// @retail 0x2fe420
void c_physics_box_shape::function_2fe420(
    const s_physics_bounds_matrix *matrix,
    float extra_radius, s_physics_bounds *result)
{
    function_2fe350(matrix, extent, extra_radius + radius, result);
}
