#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_two_vertex_matrix
{
    float rows[4][4];
};
static_assert(sizeof(s_physics_two_vertex_matrix) == 0x40,
    "two-vertex matrix layout");

struct s_physics_two_vertex_bounds
{
    float minimum[4];
    float maximum[4];
};
static_assert(sizeof(s_physics_two_vertex_bounds) == 0x20,
    "two-vertex bounds layout");

class c_physics_two_vertex_shape
{
    byte field_4[8];
    float radius;
    float vertices[2][4];
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void function_2de0f0(const s_physics_two_vertex_matrix *matrix,
        float extra_radius, s_physics_two_vertex_bounds *result);
};
static_assert(sizeof(c_physics_two_vertex_shape) == 0x30,
    "two-vertex shape layout");

// @retail 0x2de0f0
void c_physics_two_vertex_shape::function_2de0f0(
    const s_physics_two_vertex_matrix *matrix,
    float extra_radius, s_physics_two_vertex_bounds *result)
{
    float transformed[2][4];
    for (long vertex = 0; vertex < 2; vertex++)
    {
        for (long component = 0; component < 4; component++)
        {
            transformed[vertex][component] =
                matrix->rows[0][component] * vertices[vertex][0] +
                matrix->rows[1][component] * vertices[vertex][1] +
                matrix->rows[2][component] * vertices[vertex][2] +
                matrix->rows[3][component];
        }
    }

    float combined_radius = radius + extra_radius;
    for (long component = 0; component < 4; component++)
    {
        float lower = transformed[0][component] < transformed[1][component]
            ? transformed[0][component] : transformed[1][component];
        float upper = transformed[0][component] > transformed[1][component]
            ? transformed[0][component] : transformed[1][component];
        result->minimum[component] = lower - combined_radius;
        result->maximum[component] = upper + combined_radius;
    }
}
