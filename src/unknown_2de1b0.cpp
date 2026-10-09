#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_physics_capsule_segment
{
    float start[4];
    float end[4];
};
static_assert(sizeof(s_physics_capsule_segment) == 0x20,
    "capsule segment layout");

struct s_physics_capsule_hit
{
    float normal[4];
    dword key;
    float fraction;
};
static_assert(sizeof(s_physics_capsule_hit) == 0x18,
    "capsule hit layout");

class c_physics_capsule_shape
{
    byte field_4[8];
    float radius;
    float endpoints[2][4];
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual bool *function_2de1b0(bool *accepted,
        const s_physics_capsule_segment *segment, s_physics_capsule_hit *hit);
};
static_assert(sizeof(c_physics_capsule_shape) == 0x30,
    "capsule shape layout");

static float physics_capsule_dot(const float *a, const float *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static float physics_capsule_sqrt(float value)
{
    if (!(value > 0.0f))
        return 0.0f;
    union
    {
        float real;
        dword bits;
    } estimate;
    estimate.real = value;
    estimate.bits = (estimate.bits >> 1) + 0x1fc00000;
    for (long iteration = 0; iteration < 6; iteration++)
        estimate.real = 0.5f * (estimate.real + value / estimate.real);
    return estimate.real;
}

static void physics_capsule_closest(const float *point, const float *a,
    const float *axis, float axis_length_squared, float *closest)
{
    float relative[3] = {
        point[0] - a[0], point[1] - a[1], point[2] - a[2]
    };
    float scale = axis_length_squared > 0.0f
        ? physics_capsule_dot(relative, axis) / axis_length_squared : 0.0f;
    if (scale < 0.0f)
        scale = 0.0f;
    if (scale > 1.0f)
        scale = 1.0f;
    closest[0] = a[0] + axis[0] * scale;
    closest[1] = a[1] + axis[1] * scale;
    closest[2] = a[2] + axis[2] * scale;
}

// @retail 0x2de1b0
bool *c_physics_capsule_shape::function_2de1b0(bool *accepted,
    const s_physics_capsule_segment *segment, s_physics_capsule_hit *hit)
{
    float axis[3] = {
        endpoints[1][0] - endpoints[0][0],
        endpoints[1][1] - endpoints[0][1],
        endpoints[1][2] - endpoints[0][2]
    };
    float direction[3] = {
        segment->end[0] - segment->start[0],
        segment->end[1] - segment->start[1],
        segment->end[2] - segment->start[2]
    };
    float start_relative[3] = {
        segment->start[0] - endpoints[0][0],
        segment->start[1] - endpoints[0][1],
        segment->start[2] - endpoints[0][2]
    };
    float axis_length_squared = physics_capsule_dot(axis, axis);
    float direction_length_squared = physics_capsule_dot(direction, direction);

    float start_closest[3];
    physics_capsule_closest(segment->start, endpoints[0], axis,
        axis_length_squared, start_closest);
    float start_offset[3] = {
        segment->start[0] - start_closest[0],
        segment->start[1] - start_closest[1],
        segment->start[2] - start_closest[2]
    };
    if (physics_capsule_dot(start_offset, start_offset) <= radius * radius ||
        !(direction_length_squared > 0.0f))
    {
        *accepted = false;
        return accepted;
    }

    float best_fraction = hit->fraction;
    bool found = false;
    float best_closest[3] = {};

    if (axis_length_squared > 0.0f)
    {
        float axis_direction = physics_capsule_dot(axis, direction);
        float axis_start = physics_capsule_dot(axis, start_relative);
        float direction_start = physics_capsule_dot(direction, start_relative);
        float start_squared = physics_capsule_dot(start_relative, start_relative);
        float quadratic_a = axis_length_squared * direction_length_squared -
            axis_direction * axis_direction;
        float quadratic_b = axis_length_squared * direction_start -
            axis_start * axis_direction;
        float quadratic_c = axis_length_squared * start_squared -
            axis_start * axis_start - radius * radius * axis_length_squared;
        float discriminant = quadratic_b * quadratic_b -
            quadratic_a * quadratic_c;
        if (quadratic_a > 0.0f && discriminant >= 0.0f)
        {
            float fraction = (-quadratic_b - physics_capsule_sqrt(discriminant)) /
                quadratic_a;
            float axis_position = axis_start + fraction * axis_direction;
            if (fraction >= 0.0f && fraction <= best_fraction &&
                axis_position >= 0.0f && axis_position <= axis_length_squared)
            {
                best_fraction = fraction;
                found = true;
                float scale = axis_position / axis_length_squared;
                best_closest[0] = endpoints[0][0] + axis[0] * scale;
                best_closest[1] = endpoints[0][1] + axis[1] * scale;
                best_closest[2] = endpoints[0][2] + axis[2] * scale;
            }
        }
    }

    for (long endpoint = 0; endpoint < 2; endpoint++)
    {
        float offset[3] = {
            segment->start[0] - endpoints[endpoint][0],
            segment->start[1] - endpoints[endpoint][1],
            segment->start[2] - endpoints[endpoint][2]
        };
        float linear = physics_capsule_dot(offset, direction);
        float constant = physics_capsule_dot(offset, offset) - radius * radius;
        float discriminant = linear * linear - direction_length_squared * constant;
        if (discriminant < 0.0f)
            continue;
        float fraction = (-linear - physics_capsule_sqrt(discriminant)) /
            direction_length_squared;
        if (fraction < 0.0f || fraction > best_fraction)
            continue;
        float point[3] = {
            segment->start[0] + direction[0] * fraction,
            segment->start[1] + direction[1] * fraction,
            segment->start[2] + direction[2] * fraction
        };
        float from_a[3] = {
            point[0] - endpoints[0][0],
            point[1] - endpoints[0][1],
            point[2] - endpoints[0][2]
        };
        float axis_position = physics_capsule_dot(from_a, axis);
        if ((endpoint == 0 && axis_position > 0.0f) ||
            (endpoint == 1 && axis_position < axis_length_squared))
            continue;
        best_fraction = fraction;
        found = true;
        best_closest[0] = endpoints[endpoint][0];
        best_closest[1] = endpoints[endpoint][1];
        best_closest[2] = endpoints[endpoint][2];
    }

    if (!found)
    {
        *accepted = false;
        return accepted;
    }

    float point[3] = {
        segment->start[0] + direction[0] * best_fraction,
        segment->start[1] + direction[1] * best_fraction,
        segment->start[2] + direction[2] * best_fraction
    };
    float normal[3] = {
        point[0] - best_closest[0],
        point[1] - best_closest[1],
        point[2] - best_closest[2]
    };
    float normal_length = physics_capsule_sqrt(physics_capsule_dot(normal, normal));
    if (!(normal_length > 0.0f))
    {
        *accepted = false;
        return accepted;
    }
    hit->normal[0] = normal[0] / normal_length;
    hit->normal[1] = normal[1] / normal_length;
    hit->normal[2] = normal[2] / normal_length;
    hit->normal[3] = 0.0f;
    hit->key = 0xffffffff;
    hit->fraction = best_fraction;
    *accepted = true;
    return accepted;
}
