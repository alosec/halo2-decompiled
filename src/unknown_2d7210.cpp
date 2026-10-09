#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_runtime_base;

#ifndef H2_NATIVE_PORT
class c_physics_registration_manager
{
public:
    long function_2d7120(c_physics_runtime_base *object, long first, long second);
    void function_2d7180(c_physics_runtime_base *object, bool enable);
};

extern c_physics_registration_manager *g_48011c;
#endif

class c_physics_runtime_base
{
    dword vtable;
    word flags;
public:
    long function_2d7210(long first, long second);
    void function_2d7240(bool enable);
};
static_assert(sizeof(c_physics_runtime_base) == 8, "physics runtime base layout");

// @retail 0x2d7210
long c_physics_runtime_base::function_2d7210(long first, long second)
{
#ifdef H2_NATIVE_PORT
    (void)first;
    (void)second;
    /* Native has no Xbox reflection/registration dictionary. */
    return 0;
#else
    if (flags & 0x8000)
        return g_48011c->function_2d7120(this, first, second);
    return 0;
#endif
}

// @retail 0x2d7240
void c_physics_runtime_base::function_2d7240(bool enable)
{
#ifndef H2_NATIVE_PORT
    g_48011c->function_2d7180(this, enable);
#endif
    if (enable)
        flags |= 0x8000;
    else
        flags &= 0x7fff;
}
