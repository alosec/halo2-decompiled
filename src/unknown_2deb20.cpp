#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_affine_matrix
{
    float rows[4][4];
    void function_2da450(const s_physics_affine_matrix *input,
        const s_physics_affine_matrix *transform);
};
static_assert(sizeof(s_physics_affine_matrix) == 0x40,
    "physics affine matrix layout");

struct s_physics_forward_segment
{
    float start[4];
    float end[4];
    dword field_20;
    dword field_24;
};
static_assert(sizeof(s_physics_forward_segment) == 0x28,
    "forward segment layout");

struct s_physics_query_matrix
{
    float prefix[2][4];
    s_physics_affine_matrix transform;
};
static_assert(sizeof(s_physics_query_matrix) == 0x60,
    "physics query matrix layout");

struct s_physics_query_context
{
    void *shape;
    dword key;
    s_physics_query_matrix *matrix;
    s_physics_query_context *parent;
};
static_assert(sizeof(s_physics_query_context) == 0x10,
    "physics query context layout");

class c_physics_forward_shape
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
    virtual void v8();
    virtual void forward(const s_physics_forward_segment *segment,
        s_physics_query_context *context, void *collector);
};

class c_physics_transformed_forward_shape
{
    byte field_4[8];
    c_physics_forward_shape *child;
    s_physics_affine_matrix transform;
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void function_2deb20(const s_physics_forward_segment *segment,
        s_physics_query_context *context, void *collector);
};
static_assert(sizeof(c_physics_transformed_forward_shape) == 0x50,
    "transformed forward shape layout");

// @retail 0x2deb20
void c_physics_transformed_forward_shape::function_2deb20(
    const s_physics_forward_segment *segment,
    s_physics_query_context *context, void *collector)
{
    s_physics_forward_segment local;
    const float *world_points[2] = { segment->start, segment->end };
    float *local_points[2] = { local.start, local.end };
    for (long point = 0; point < 2; point++)
    {
        float delta[4];
        for (long component = 0; component < 4; component++)
            delta[component] = world_points[point][component] - transform.rows[3][component];
        for (long component = 0; component < 4; component++)
        {
            local_points[point][component] =
                transform.rows[0][component] * delta[0] +
                transform.rows[1][component] * delta[1] +
                transform.rows[2][component] * delta[2];
        }
    }
    local.field_20 = segment->field_20;
    local.field_24 = segment->field_24;

    s_physics_query_matrix local_matrix;
    for (long row = 0; row < 2; row++)
        for (long component = 0; component < 4; component++)
            local_matrix.prefix[row][component] = context->matrix->prefix[row][component];
    local_matrix.transform.function_2da450(&context->matrix->transform, &transform);

    s_physics_query_context local_context = {
        child, context->key, &local_matrix, context
    };
    child->forward(&local, &local_context, collector);
}
