#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_plane_segment
{
    float start[4];
    float end[4];
};
static_assert(sizeof(s_physics_plane_segment) == 0x20,
    "plane segment layout");

struct s_physics_plane_hit
{
    float normal[4];
    dword key;
    float fraction;
};
static_assert(sizeof(s_physics_plane_hit) == 0x18,
    "plane hit layout");

class c_physics_plane_cloud_shape
{
    byte field_4[0xd0];
    float (*planes)[4];
    long plane_count;
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *function_2fd5c0(bool *accepted,
        const s_physics_plane_segment *segment, s_physics_plane_hit *hit);
};
static_assert(sizeof(c_physics_plane_cloud_shape) == 0xdc,
    "plane cloud shape layout");

static float physics_plane_distance(const float *plane, const float *point)
{
    return plane[0] * point[0] + plane[1] * point[1] +
        plane[2] * point[2] + plane[3];
}

// @retail 0x2fd5c0
bool *c_physics_plane_cloud_shape::function_2fd5c0(bool *accepted,
    const s_physics_plane_segment *segment, s_physics_plane_hit *hit)
{
    float minimum_fraction = -1.0f;
    float maximum_fraction = hit->fraction;
    float minimum_plane[4];

    for (long plane_index = plane_count - 1; plane_index >= 0; plane_index--)
    {
        const float *plane = planes[plane_index];
        float start_distance = physics_plane_distance(plane, segment->start);
        float end_distance = physics_plane_distance(plane, segment->end);
        if (start_distance >= 0.0f)
        {
            if (end_distance >= 0.0f)
            {
                *accepted = false;
                return accepted;
            }
            float fraction = start_distance / (start_distance - end_distance);
            if (fraction >= minimum_fraction)
            {
                minimum_fraction = fraction;
                minimum_plane[0] = plane[0];
                minimum_plane[1] = plane[1];
                minimum_plane[2] = plane[2];
                minimum_plane[3] = plane[3];
            }
        }
        else if (end_distance >= 0.0f)
        {
            float fraction = start_distance / (start_distance - end_distance);
            if (fraction <= maximum_fraction)
                maximum_fraction = fraction;
        }

        if (maximum_fraction < minimum_fraction)
        {
            *accepted = false;
            return accepted;
        }
    }

    if (!(minimum_fraction >= 0.0f))
    {
        *accepted = false;
        return accepted;
    }

    hit->normal[0] = minimum_plane[0];
    hit->normal[1] = minimum_plane[1];
    hit->normal[2] = minimum_plane[2];
    hit->normal[3] = minimum_plane[3];
    hit->key = 0xffffffff;
    hit->fraction = minimum_fraction;
    *accepted = true;
    return accepted;
}
