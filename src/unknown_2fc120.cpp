#include "unknown_11c920.h"

// @flags /O2 /Gr

struct __declspec(align(16)) s_physics_aabb
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_aabb) == 0x20, "physics AABB layout");

class c_physics_shape_query
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void get_aabb(void *transform, void *tolerance, s_physics_aabb *result);
};

struct s_physics_shape_collection_entry
{
    c_physics_shape_query *shape;
    long key;
};
static_assert(sizeof(s_physics_shape_collection_entry) == 8, "shape collection entry layout");

class c_physics_shape_collection
{
    byte field_0[0x0c];
    s_physics_shape_collection_entry *entries;
    long count;
public:
    void function_2fc120(void *transform, void *tolerance, s_physics_aabb *result);
};

// @retail 0x2fc120
void c_physics_shape_collection::function_2fc120(
    void *transform,
    void *tolerance,
    s_physics_aabb *result)
{
    entries[0].shape->get_aabb(transform, tolerance, result);
    for (long index = 1; index < count; index++)
    {
        s_physics_aabb child;
        entries[index].shape->get_aabb(transform, tolerance, &child);
        for (long component = 0; component < 4; component++)
        {
            if (child.minimum[component] < result->minimum[component])
                result->minimum[component] = child.minimum[component];
            if (child.maximum[component] > result->maximum[component])
                result->maximum[component] = child.maximum[component];
        }
    }
}
