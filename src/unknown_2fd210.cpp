#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_shape_marker
{
    long value;
    byte present;
    byte pad[3];
    long size;
};
static_assert(sizeof(s_physics_shape_marker) == 0x0c, "shape marker layout");

class c_physics_shape_455ef0
{
    byte field_0[0x10];
    float bounds[4];
public:
    void function_2fe330(s_physics_shape_marker *marker);
    void function_2fe410(float *result);
};

// @retail 0x2fe330
void c_physics_shape_455ef0::function_2fe330(s_physics_shape_marker *marker)
{
    marker->value = 8;
    marker->present = true;
    marker->size = 0x10;
}

// @retail 0x2fe410
void c_physics_shape_455ef0::function_2fe410(float *result)
{
    for (long index = 0; index < 4; index++)
        result[index] = bounds[index];
}

class c_physics_shape_456008
{
    byte field_0[0x30];
    byte *values;
    byte field_34[0xd0 - 0x34];
    long marker_value;
public:
    void function_2fd210(s_physics_shape_marker *marker);
    void function_2fd230(long *result);
};

// @retail 0x2fd210
void c_physics_shape_456008::function_2fd210(s_physics_shape_marker *marker)
{
    marker->value = marker_value;
    marker->present = true;
    marker->size = 0x10;
}

// @retail 0x2fd230
void c_physics_shape_456008::function_2fd230(long *result)
{
    result[0] = *(long *)(values + 0x00);
    result[1] = *(long *)(values + 0x10);
    result[2] = *(long *)(values + 0x20);
    result[3] = 0;
}
