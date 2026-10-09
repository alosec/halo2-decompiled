#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_shape_hit
{
    float direction[4];
    dword key;
    dword collector_value;
};
static_assert(sizeof(s_physics_shape_hit) == 0x18, "shape hit layout");

struct s_physics_shape_query_transform
{
    dword field_0;
    dword key;
    const float (*matrix)[4];
};
static_assert(sizeof(s_physics_shape_query_transform) == 0x0c,
    "shape query transform layout");

class c_physics_shape_hit_collector
{
public:
    dword value;
    virtual void submit(s_physics_shape_query_transform *transform,
        s_physics_shape_hit *hit);
};
static_assert(sizeof(c_physics_shape_hit_collector) == 8,
    "shape hit collector layout");

class c_physics_convex_shape
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *intersect(bool *accepted, const void *query,
        s_physics_shape_hit *hit);
    virtual void function_2df110(const void *query,
        s_physics_shape_query_transform *transform,
        c_physics_shape_hit_collector *collector);
};

// @retail 0x2df110
void c_physics_convex_shape::function_2df110(const void *query,
    s_physics_shape_query_transform *transform,
    c_physics_shape_hit_collector *collector)
{
    bool accepted;
    s_physics_shape_hit hit;
    hit.collector_value = collector->value;
    intersect(&accepted, query, &hit);
    if (!accepted)
        return;

    hit.key = transform->key;
    float original[4] = {
        hit.direction[0], hit.direction[1],
        hit.direction[2], hit.direction[3]
    };
    for (long component = 0; component < 4; component++)
    {
        hit.direction[component] =
            transform->matrix[2][component] * original[0] +
            transform->matrix[3][component] * original[1] +
            transform->matrix[4][component] * original[2];
    }
    collector->submit(transform, &hit);
}
