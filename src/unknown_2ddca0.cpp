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
    float vertices[2][4];
public:
    void function_2ddca0(s_physics_shape_marker_455500 *marker);
    void *function_2ddcc0(long unused);
    void function_2ddd30(const float *direction, float *result);
    void function_2dddb0(const word *offsets, long count, float *results);
};
static_assert(sizeof(c_physics_shape_455500) == 0x30, "two-vertex shape layout");

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

union u_physics_float_bits_455500
{
    float real;
    dword bits;
};

// @retail 0x2ddd30
void c_physics_shape_455500::function_2ddd30(const float *direction, float *result)
{
    float x = (vertices[1][0] - vertices[0][0]) * direction[0];
    float y = (vertices[1][1] - vertices[0][1]) * direction[1];
    float z = (vertices[1][2] - vertices[0][2]) * direction[2];
    bool use_first = x + y + z < 0.0f;
    const float *source = vertices[use_first ? 0 : 1];
    for (long component = 0; component < 4; component++)
        result[component] = source[component];

    u_physics_float_bits_455500 marker;
    marker.bits = 0x3f000000 | (use_first ? 0 : 0x10);
    result[3] = marker.real;
}

// @retail 0x2dddb0
void c_physics_shape_455500::function_2dddb0(
    const word *offsets, long count, float *results)
{
    for (long output_index = 0; output_index < count; output_index++)
    {
        word offset = offsets[output_index];
        const float *source = (const float *)((const byte *)vertices + offset);
        float *result = results + output_index * 4;
        for (long component = 0; component < 4; component++)
            result[component] = source[component];

        u_physics_float_bits_455500 marker;
        marker.bits = 0x3f000000 | offset;
        result[3] = marker.real;
    }
}
