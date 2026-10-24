#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_transform_matrix
{
    float rows[4][4];
};
static_assert(sizeof(s_physics_transform_matrix) == 0x40,
    "physics transform matrix layout");

struct s_physics_direction
{
    float values[4];
    void function_2da6e0(const s_physics_transform_matrix *matrix,
        const float *source);
};
static_assert(sizeof(s_physics_direction) == 0x10,
    "physics direction layout");

// @retail 0x2da6e0
void s_physics_direction::function_2da6e0(
    const s_physics_transform_matrix *matrix, const float *source)
{
    for (long component = 0; component < 4; component++)
    {
        values[component] =
            matrix->rows[0][component] * source[0] +
            matrix->rows[1][component] * source[1] +
            matrix->rows[2][component] * source[2];
    }
}

struct s_physics_transformed_segment
{
    float start[4];
    float end[4];
    dword field_20;
    dword field_24;
};
static_assert(sizeof(s_physics_transformed_segment) == 0x28,
    "transformed segment layout");

struct s_physics_transformed_hit
{
    s_physics_direction normal;
    dword key;
    float fraction;
};
static_assert(sizeof(s_physics_transformed_hit) == 0x18,
    "transformed hit layout");

class c_physics_intersection_shape
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
#if H2_WHOLE_IMAGE
    /* an hkBool returned by value: through the hidden result pointer (x8 on
       the port's ARM guest), as retail's callers pass it */
    struct answer { char value; answer() {} };
    virtual answer intersect(const s_physics_transformed_segment *segment,
        s_physics_transformed_hit *hit);
#else
    virtual bool *intersect(bool *accepted,
        const s_physics_transformed_segment *segment,
        s_physics_transformed_hit *hit);
#endif
};

class c_physics_transformed_intersection_shape
{
    byte field_4[8];
    c_physics_intersection_shape *child;
    s_physics_transform_matrix transform;
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
#if H2_WHOLE_IMAGE
    virtual c_physics_intersection_shape::answer function_2de9e0(
        const s_physics_transformed_segment *segment,
        s_physics_transformed_hit *hit);
    bool *function_2de9e0_into(bool *accepted,
        const s_physics_transformed_segment *segment,
        s_physics_transformed_hit *hit);
#else
    virtual bool *function_2de9e0(bool *accepted,
        const s_physics_transformed_segment *segment,
        s_physics_transformed_hit *hit);
#endif
};
static_assert(sizeof(c_physics_transformed_intersection_shape) == 0x50,
    "transformed intersection shape layout");

#if H2_WHOLE_IMAGE
// @retail 0x2de9e0
c_physics_intersection_shape::answer c_physics_transformed_intersection_shape::function_2de9e0(
    const s_physics_transformed_segment *segment, s_physics_transformed_hit *hit)
{
    c_physics_intersection_shape::answer result;
    bool accepted;
    function_2de9e0_into(&accepted, segment, hit);
    result.value = accepted;
    return result;
}

bool *c_physics_transformed_intersection_shape::function_2de9e0_into(
    bool *accepted, const s_physics_transformed_segment *segment,
    s_physics_transformed_hit *hit)
#else
// @retail 0x2de9e0
bool *c_physics_transformed_intersection_shape::function_2de9e0(
    bool *accepted, const s_physics_transformed_segment *segment,
    s_physics_transformed_hit *hit)
#endif
{
    s_physics_transformed_segment local;
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

#if H2_WHOLE_IMAGE
    *accepted = child->intersect(&local, hit).value != 0;
#else
    child->intersect(accepted, &local, hit);
#endif
    if (*accepted)
    {
        float local_normal[4] = {
            hit->normal.values[0], hit->normal.values[1],
            hit->normal.values[2], hit->normal.values[3]
        };
        hit->normal.function_2da6e0(&transform, local_normal);
    }
    return accepted;
}
