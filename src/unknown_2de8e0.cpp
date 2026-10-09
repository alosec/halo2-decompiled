#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_projection_shape
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual float projected_support(const float *direction);
};

class c_physics_transformed_projection_shape
{
    byte field_4[8];
    c_physics_projection_shape *child;
    float transform[4][4];
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual float function_2de8e0(const float *direction);
};
static_assert(sizeof(c_physics_transformed_projection_shape) == 0x50,
    "transformed projection shape layout");

// @retail 0x2de8e0
float c_physics_transformed_projection_shape::function_2de8e0(
    const float *direction)
{
    float local_direction[4];
    for (long component = 0; component < 4; component++)
    {
        local_direction[component] =
            transform[0][component] * direction[0] +
            transform[1][component] * direction[1] +
            transform[2][component] * direction[2];
    }

    float distance = child->projected_support(local_direction);
    float local_point[4];
    for (long component = 0; component < 4; component++)
        local_point[component] = local_direction[component] * distance;

    float world_point[4];
    for (long component = 0; component < 4; component++)
    {
        world_point[component] =
            transform[0][component] * local_point[0] +
            transform[1][component] * local_point[1] +
            transform[2][component] * local_point[2] +
            transform[3][component];
    }

    return world_point[0] * direction[0] +
        world_point[1] * direction[1] +
        world_point[2] * direction[2];
}
