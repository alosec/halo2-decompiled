#include "unknown_11c920.h"

// @flags /O2 /Gr

struct __declspec(align(16)) s_physics_affine_matrix
{
    float rows[4][4];
    void function_2da450(const s_physics_affine_matrix *input,
        const s_physics_affine_matrix *transform);
};
static_assert(sizeof(s_physics_affine_matrix) == 0x40, "physics matrix layout");

// @retail 0x2da450
void s_physics_affine_matrix::function_2da450(
    const s_physics_affine_matrix *input,
    const s_physics_affine_matrix *transform)
{
    for (long row = 0; row < 4; row++)
    {
        for (long component = 0; component < 4; component++)
        {
            rows[row][component] =
                input->rows[0][component] * transform->rows[row][0] +
                input->rows[1][component] * transform->rows[row][1] +
                input->rows[2][component] * transform->rows[row][2];
        }
    }
    for (long component = 0; component < 4; component++)
        rows[3][component] += input->rows[3][component];
}

class c_physics_shape_query
{
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void bounds(const s_physics_affine_matrix *matrix, void *input, void *result);
};

class c_physics_transformed_shape
{
    byte field_0[0x0c];
    c_physics_shape_query *child;
    s_physics_affine_matrix transform;
public:
    void function_2de850(const s_physics_affine_matrix *matrix, void *input, void *result);
};

// @retail 0x2de850
void c_physics_transformed_shape::function_2de850(
    const s_physics_affine_matrix *matrix, void *input, void *result)
{
    s_physics_affine_matrix combined;
    combined.function_2da450(matrix, &transform);
    child->bounds(&combined, input, result);
}
