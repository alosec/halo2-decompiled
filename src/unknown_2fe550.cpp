#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_line_segment
{
    float start[4];
    float end[4];
};
static_assert(sizeof(s_physics_line_segment) == 0x20,
    "physics line-segment layout");

struct s_physics_line_hit
{
    float normal[4];
    dword key;
    float fraction;
};
static_assert(sizeof(s_physics_line_hit) == 0x18,
    "physics line-hit layout");

class c_physics_box_shape
{
    byte field_4[8];
    float radius;
    float extent[4];
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *function_2fe550(bool *accepted,
        const s_physics_line_segment *segment, s_physics_line_hit *hit);
};
static_assert(sizeof(c_physics_box_shape) == 0x20, "box shape layout");

// @retail 0x2fe550
bool *c_physics_box_shape::function_2fe550(bool *accepted,
    const s_physics_line_segment *segment, s_physics_line_hit *hit)
{
    dword start_positive = 0;
    dword start_negative = 0;
    dword end_positive = 0;
    dword end_negative = 0;
    for (long axis = 0; axis < 3; axis++)
    {
        dword bit = 1u << axis;
        if (extent[axis] <= segment->start[axis])
            start_positive |= bit;
        if (segment->start[axis] <= -extent[axis])
            start_negative |= bit;
        if (extent[axis] <= segment->end[axis])
            end_positive |= bit;
        if (segment->end[axis] <= -extent[axis])
            end_negative |= bit;
    }

    if ((start_positive & end_positive) != 0 ||
        (start_negative & end_negative) != 0 ||
        (start_positive | start_negative) == 0)
    {
        *accepted = false;
        return accepted;
    }

    float minimum_fraction = 0.0f;
    float maximum_fraction = hit->fraction;
    float normal[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    for (long side_index = 0; side_index < 2; side_index++)
    {
        float side = side_index == 0 ? 1.0f : -1.0f;
        dword start_mask = side_index == 0 ? start_positive : start_negative;
        dword active_mask = start_mask |
            (side_index == 0 ? end_positive : end_negative);

        for (long axis = 2; axis >= 0; axis--)
        {
            dword bit = 1u << axis;
            if ((active_mask & bit) == 0)
                continue;

            float start_distance = side * segment->start[axis] - extent[axis];
            float end_distance = side * segment->end[axis] - extent[axis];
            float fraction = start_distance / (start_distance - end_distance);
            if (!(fraction == fraction))
            {
                *accepted = false;
                return accepted;
            }
            if ((start_mask & bit) != 0)
            {
                if (!(fraction >= minimum_fraction))
                {
                    *accepted = false;
                    return accepted;
                }
                minimum_fraction = fraction;
                normal[0] = normal[1] = normal[2] = normal[3] = 0.0f;
                normal[axis] = side;
            }
            else if (maximum_fraction >= fraction)
            {
                maximum_fraction = fraction;
            }

            if (!(maximum_fraction >= minimum_fraction))
            {
                *accepted = false;
                return accepted;
            }
        }
    }

    hit->normal[0] = normal[0];
    hit->normal[1] = normal[1];
    hit->normal[2] = normal[2];
    hit->normal[3] = normal[3];
    hit->key = 0xffffffff;
    hit->fraction = minimum_fraction;
    *accepted = true;
    return accepted;
}
