#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_projectable_child
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

class c_physics_shape_collection
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual float function_2fbdc0(const float *direction);
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual long first_key();
    virtual long next_key(long key);
    virtual c_physics_projectable_child *child(long key, void *storage);
};

// @retail 0x2fbdc0
float c_physics_shape_collection::function_2fbdc0(const float *direction)
{
    float maximum = -3.402823466e+38F;
    long key = first_key();
    while (key != NONE)
    {
        byte child_storage[0x100];
        c_physics_projectable_child *shape = child(key, child_storage);
        float value = shape->projected_support(direction);
        if (value > maximum)
            maximum = value;
        key = next_key(key);
    }
    return maximum;
}
