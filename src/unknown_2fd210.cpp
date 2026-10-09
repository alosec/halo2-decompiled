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
    byte field_0[0x0c];
    float vertex_w;
    float bounds[4];
public:
    void function_2fe330(s_physics_shape_marker *marker);
    void function_2fe410(float *result);
    void function_2fe450(const float *direction, float *result);
    void function_2fe490(const word *indices, long count, float *results);
    void function_2fe4f0(float *results);
};
static_assert(sizeof(c_physics_shape_455ef0) == 0x20, "box shape layout");

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

union u_physics_float_bits
{
    float real;
    dword bits;
};

static const float k_physics_box_vertex_signs[8][4] =
{
    {  1.0f,  1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f },
    {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f, 1.0f },
};

// @retail 0x2fe450
void c_physics_shape_455ef0::function_2fe450(const float *direction, float *result)
{
    dword sign_key = 0;
    for (long index = 0; index < 4; index++)
    {
        u_physics_float_bits extent;
        u_physics_float_bits input;
        extent.real = bounds[index];
        input.real = direction[index];
        extent.bits ^= input.bits & 0x80000000;
        result[index] = extent.real;
        if (index < 3)
            sign_key |= (extent.bits >> 31) << index;
    }

    u_physics_float_bits marker;
    marker.bits = 0x3f000000 | sign_key;
    result[3] = marker.real;
}

// @retail 0x2fe490
void c_physics_shape_455ef0::function_2fe490(const word *indices, long count, float *results)
{
    for (long output_index = 0; output_index < count; output_index++)
    {
        word vertex_index = indices[output_index];
        float *result = results + output_index * 4;
        for (long component = 0; component < 4; component++)
            result[component] = bounds[component] * k_physics_box_vertex_signs[vertex_index][component];

        u_physics_float_bits marker;
        marker.bits = 0x3f000000 | vertex_index;
        result[3] = marker.real;
    }
}

// @retail 0x2fe4f0
void c_physics_shape_455ef0::function_2fe4f0(float *results)
{
    for (long vertex_index = 0; vertex_index < 8; vertex_index++)
    {
        float *result = results + vertex_index * 4;
        for (long component = 0; component < 3; component++)
            result[component] = bounds[component] * k_physics_box_vertex_signs[vertex_index][component];
        result[3] = vertex_w;
    }
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
