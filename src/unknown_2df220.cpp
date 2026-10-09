#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_projection_matrix
{
    float rows[4][4];
};
static_assert(sizeof(s_physics_projection_matrix) == 0x40,
    "projection matrix layout");

struct s_physics_projection_bounds
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_projection_bounds) == 0x20,
    "projection bounds layout");

union u_physics_projection_float
{
    float real;
    dword bits;
};

class c_physics_projected_shape
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void bounds(const s_physics_projection_matrix *matrix,
        float extra_radius, s_physics_projection_bounds *result);
    virtual float function_2df220(const float *direction);
};

// @retail 0x2df220
float c_physics_projected_shape::function_2df220(const float *direction)
{
    s_physics_projection_matrix identity;
    identity.rows[0][0] = 1.0f;
    identity.rows[0][1] = 0.0f;
    identity.rows[0][2] = 0.0f;
    identity.rows[0][3] = 0.0f;
    identity.rows[1][0] = 0.0f;
    identity.rows[1][1] = 1.0f;
    identity.rows[1][2] = 0.0f;
    identity.rows[1][3] = 0.0f;
    identity.rows[2][0] = 0.0f;
    identity.rows[2][1] = 0.0f;
    identity.rows[2][2] = 1.0f;
    identity.rows[2][3] = 0.0f;
    identity.rows[3][0] = 0.0f;
    identity.rows[3][1] = 0.0f;
    identity.rows[3][2] = 0.0f;
    identity.rows[3][3] = 0.0f;

    s_physics_projection_bounds shape_bounds;
    bounds(&identity, 0.0f, &shape_bounds);

    float projected[4];
    for (long component = 0; component < 4; component++)
    {
        float center = 0.5f *
            (shape_bounds.maximum[component] + shape_bounds.minimum[component]);
        float half_extent = 0.5f *
            (shape_bounds.maximum[component] - shape_bounds.minimum[component]);
        u_physics_projection_float signed_extent;
        signed_extent.real = half_extent;
        u_physics_projection_float sign;
        sign.real = direction[component];
        signed_extent.bits ^= sign.bits & 0x80000000;
        projected[component] = center + signed_extent.real;
    }

    return projected[0] * direction[0] +
        projected[1] * direction[1] +
        projected[2] * direction[2];
}
