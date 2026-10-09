#include "unknown_11c920.h"

// @flags /O2 /Gr

class c_physics_model_leaf
{
public:
    void function_2dae30(void *, void *, void *);
    void function_2fdfb0(void *, void *, float *bounds);
};

// @retail 0x2dae30
void c_physics_model_leaf::function_2dae30(void *, void *, void *)
{
}

// @retail 0x2fdfb0
void c_physics_model_leaf::function_2fdfb0(void *, void *, float *bounds)
{
    for (long index = 0; index < 8; index++)
        bounds[index] = 0.0f;
}
