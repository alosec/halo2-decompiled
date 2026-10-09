#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

// Native-port source split of retail 0x1df080. The matching Xbox definition
// remains in unknown_1dee80.cpp, whose unrelated code requires x86 intrinsics.
extern point3f *g_468788;

void function_1df080(point3f const *points, long count, point3f *center, real *radius)
{
    long b = 0;
    long a = 0;
    real maximum = 0.0f;
    if (count > 1)
    {
        for (long i = 0; i < count; ++i)
        {
            for (long j = i + 1; j < count; ++j)
            {
                real x = points[i].x - points[j].x;
                real y = points[i].y - points[j].y;
                real z = points[i].z - points[j].z;
                real distance = x*x + y*y + z*z;
                if (distance > maximum)
                {
                    maximum = distance;
                    b = j;
                    a = i;
                }
            }
        }
        real initial_radius = (real)sqrt(maximum) * 0.5f;
        center->x = (points[b].x + points[a].x) * 0.5f;
        center->y = (points[b].y + points[a].y) * 0.5f;
        center->z = (points[b].z + points[a].z) * 0.5f;
        real larger_radius = 0.0f;
        for (long i = 0; i < count; ++i)
        {
            real x = center->x - points[i].x;
            real y = center->y - points[i].y;
            real z = center->z - points[i].z;
            real distance = (real)sqrt(x*x + y*y + z*z);
            if (distance > initial_radius && distance > larger_radius)
                larger_radius = distance;
        }
        *radius = larger_radius > 0.0f ? larger_radius : initial_radius;
    }
    else
    {
        *radius = 0.0f;
        *center = count == 0 ? *g_468788 : points[0];
    }
}
