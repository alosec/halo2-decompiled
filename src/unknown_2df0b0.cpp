#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_convex_shape
{
    byte field_4[8];
    float radius;
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual float function_2df0b0(const float *direction);
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void support_vertex(const float *direction, float *result);
};
static_assert(sizeof(c_physics_convex_shape) == 0x10, "convex shape base layout");

// @retail 0x2df0b0
float c_physics_convex_shape::function_2df0b0(const float *direction)
{
    float vertex[4];
    support_vertex(direction, vertex);
    float x = vertex[0] * direction[0];
    float y = vertex[1] * direction[1];
    float z = vertex[2] * direction[2];
    return x + y + z + radius;
}
