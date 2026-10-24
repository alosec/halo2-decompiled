#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_26c380.h"

// @flags /O2 /arch:SSE /Gr

extern point2f *g_468778;
real normalize2d(point2f *arg_0);
bool function_1f5110(long arg_0, point2f const *arg_1, real arg_2,
    real arg_3, bool *arg_4, s_path_trace_result *arg_5);

PRIVATE __forceinline real function_1a83e1(point3f const *start, vector3f const *segment, point3f const *point)
{
    vector3f delta;
    real squared_distance;
    real length_squared = segment->k * segment->k + segment->j * segment->j + segment->i * segment->i;
    if (length_squared > 0.0001f)
    {
        delta.i = point->x - start->x;
        delta.j = point->y - start->y;
        delta.k = point->z - start->z;
        real t = (delta.k * segment->k + delta.j * segment->j + delta.i * segment->i) / length_squared;
        if (t < 0.0f)
            t = 0.0f;
        else if (t > 1.0f)
            t = 1.0f;
        t = -t;
        delta.i += segment->i * t;
        delta.j += segment->j * t;
        delta.k += segment->k * t;
        squared_distance = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
    }
    else
    {
        delta.i = start->x - point->x;
        delta.j = start->y - point->y;
        delta.k = start->z - point->z;
        squared_distance = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
    }
    return (real)sqrt(squared_distance);
}

static inline void point_offset_along_vector(point3f const *point, vector3f const *vector, real t, point3f *result)
{
    result->x = vector->i * t + point->x;
    result->y = vector->j * t + point->y;
    result->z = vector->k * t + point->z;
}

PRIVATE __forceinline real function_1a83e2(point2f *arg_0)
{
    real local_0 = (real)sqrt(arg_0->y * arg_0->y + arg_0->x * arg_0->x);
    if (!(fabs(local_0) < 0.0001f))
    {
        real local_1 = 1.0f / local_0;
        arg_0->x = local_1 * arg_0->x;
        arg_0->y = local_1 * arg_0->y;
        return local_0;
    }
    return 0.0f;
}

// @retail 0x1a83e0
bool __stdcall function_1a83e0(long arg_0, short *arg_1, real *arg_2, point2f *arg_3, bool *arg_4)
{
    s_actor_view *local_0 = actor_get(arg_0);
    byte *local_1 = g_4e3b44[ai_object_get(local_0->unknown018)->definition_index & 0xffff].bytes;
    bool local_3 = false;
    bool local_4 = false;
    short local_5 = NONE;
    real local_2 = *(real *)(local_1 + 0x124);
    point2f local_6;
    if (local_2 > 0.0f)
    {
        local_6.x = -local_0->unknown37c.i;
        local_6.y = -local_0->unknown37c.j;
        if (function_1a83e2(&local_6) < 1.0f)
        {
            local_6.x = local_0->unknown370.x - local_0->position.x;
            local_6.y = local_0->unknown370.y - local_0->position.y;
            if (normalize2d(&local_6) == 0.0f)
            {
                local_6 = *(point2f const *)&local_0->unknown290;
                if (normalize2d(&local_6) == 0.0f)
                    local_6 = *g_468778;
            }
        }
        vector3f local_7;
        local_7.i = *(real *)((byte *)local_0 + 0x388) - local_0->unknown370.x;
        local_7.j = *(real *)((byte *)local_0 + 0x38c) - local_0->unknown370.y;
        local_7.k = *(real *)((byte *)local_0 + 0x390) - local_0->unknown370.z;
        point3f local_8;
        point3f local_9;
        vector3f local_10;
        vector3f local_11;
        local_10.i = 0.0f - local_6.y;
        local_10.j = local_6.x;
        local_10.k = 0.0f;
        local_11.i = local_6.y;
        local_11.j = 0.0f - local_6.x;
        local_11.k = 0.0f;
        point_offset_along_vector(&local_0->position, &local_10, local_2, &local_8);
        point_offset_along_vector(&local_0->position, &local_11, local_2, &local_9);
        bool local_12;
        bool local_13;
        s_path_trace_result local_14;
        bool local_15 = function_1f5110(arg_0, (point2f const *)&local_10, local_2, 0.0f, &local_12, &local_14);
        real local_16 = function_1a83e1(&local_0->unknown370, &local_7, &local_8);
        bool local_17 = local_15 && local_16 > local_0->unknown36c;
        bool local_18 = function_1f5110(arg_0, (point2f const *)&local_11, local_2, 0.0f, &local_13, &local_14);
        real local_19 = function_1a83e1(&local_0->unknown370, &local_7, &local_9);
        bool local_20 = local_18 && local_19 > local_0->unknown36c;
        if (local_15)
        {
            if (local_18)
            {
                real local_21 = local_16 - local_19;
                if (local_12 > local_13 || local_17 > local_20 || local_21 > 0.3f)
                {
                    local_5 = 0;
                    local_4 = local_12;
                    local_3 = true;
                }
                else if (local_13 > local_12 || local_20 > local_17 || local_21 < -0.3f)
                {
                    local_5 = 1;
                    local_4 = local_13;
                    local_3 = true;
                }
                else
                {
                    local_5 = 4;
                    local_4 = local_12;
                    local_3 = local_17;
                }
            }
            else
            {
                local_5 = 0;
                local_4 = local_12;
                local_3 = local_17;
            }
        }
        else if (local_18)
        {
            local_5 = 1;
            local_4 = local_13;
            local_3 = local_20;
        }
    }
    *arg_1 = local_5;
    *arg_2 = local_2;
    *arg_4 = local_4;
    *arg_3 = local_6;
    return local_3;
}
