// Native scalar split of the recovered bounded ray/sphere test at retail
// 0x11e5e0. The matching Xbox definition remains in unknown_11cb00.cpp,
// whose unrelated language helpers contain x86 inline assembly.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

bool function_11e5e0(point3f const *origin, point3f const *center,
    vector3f const *direction, real radius)
{
    vector3f delta;
    delta.i = origin->x - center->x;
    delta.j = origin->y - center->y;
    delta.k = origin->z - center->z;
    real const c = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i - radius * radius;
    if (c < 0.0f)
        return true;

    vector3f const d = *direction;
    real const b = d.k * delta.k + d.j * delta.j + d.i * delta.i;
    if (b >= 0.0f)
        return false;

    real const a = d.k * d.k + d.j * d.j + d.i * d.i;
    real const discriminant = b * b - a * c;
    if (discriminant <= 0.0f)
        return false;

    real const end = 0.0f - a - b;
    return end < 0.0f || discriminant > end * end;
}
