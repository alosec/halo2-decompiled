#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_forwarded_aabb
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_forwarded_aabb) == 0x20, "forwarded AABB layout");

class c_physics_aabb_target
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void get_aabb(void *transform, void *tolerance, s_physics_forwarded_aabb *result);
};

class c_physics_aabb_forwarder
{
    byte field_0[0x0c];
    c_physics_aabb_target *shape;
public:
    void function_2df5d0(void *transform, void *tolerance, s_physics_forwarded_aabb *result);
};

// @retail 0x2df5d0
void c_physics_aabb_forwarder::function_2df5d0(
    void *transform,
    void *tolerance,
    s_physics_forwarded_aabb *result)
{
    shape->get_aabb(transform, tolerance, result);
}
