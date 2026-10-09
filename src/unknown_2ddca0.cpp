#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_shape_marker_455500
{
    long value;
    byte present;
    byte pad[3];
    long size;
};
static_assert(sizeof(s_physics_shape_marker_455500) == 0x0c, "shape marker layout");

class c_physics_shape_455500
{
    byte field_0[0x10];
public:
    void function_2ddca0(s_physics_shape_marker_455500 *marker);
    void *function_2ddcc0(long unused);
};

// @retail 0x2ddca0
void c_physics_shape_455500::function_2ddca0(s_physics_shape_marker_455500 *marker)
{
    marker->value = 2;
    marker->present = false;
    marker->size = 0x10;
}

// @retail 0x2ddcc0
void *c_physics_shape_455500::function_2ddcc0(long)
{
    return field_0 + 0x10;
}
