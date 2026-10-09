#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_shape_forward_target
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
    virtual void v8(void *input, void *context, void *result);
    virtual void v9(void *input, void *context, void *result);
};

struct s_physics_shape_forward_context
{
    c_physics_shape_forward_target *shape;
    long field_4;
    long field_8;
    void *source;
};
static_assert(sizeof(s_physics_shape_forward_context) == 0x10, "forward context layout");

class c_physics_shape_forwarder
{
    byte field_0[0x10];
    c_physics_shape_forward_target *shape;
public:
    void *function_2fe1a0(void *input, void *context, void *result);
    void function_2fe1c0(void *input, long *context, void *result);
};

// @retail 0x2fe1a0
void *c_physics_shape_forwarder::function_2fe1a0(
    void *input,
    void *context,
    void *result)
{
    shape->v8(input, context, result);
    return input;
}

// @retail 0x2fe1c0
void c_physics_shape_forwarder::function_2fe1c0(
    void *input,
    long *context,
    void *result)
{
    s_physics_shape_forward_context forwarded = {
        shape,
        context[1],
        context[2],
        context,
    };
    shape->v9(input, &forwarded, result);
}
