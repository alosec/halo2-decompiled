// Source-local native reconstruction of retail function_23ed30. This keeps
// the exact NV2A push-buffer packet and its true reservation/submission
// providers visible for closure scoring without parsing unrelated Xbox D3D
// code from unknown_23d970.cpp. It remains dormant and unlinked.

#include "unknown_11c920.h"
#include "effects.h"

extern byte g_470a38;
extern long g_470a3c[8];

bool function_015d00(long dword_count, dword **push);
extern "C" void __stdcall D3DDevice_EndPush(dword *push);

PRIVATE inline dword native_push_encode(dword method, dword count)
{
    return method | (count << 18);
}

PRIVATE inline dword *native_push_vertex4f(dword *push, long slot,
    real a, real b, real c, real d)
{
    push[0] = native_push_encode(0x1a00 + slot * 16, 4);
    ((real *)push)[1] = a;
    ((real *)push)[2] = b;
    ((real *)push)[3] = c;
    ((real *)push)[4] = d;
    return push + 5;
}

PRIVATE inline dword *native_push_vertex2f(dword *push, long slot,
    real a, real b)
{
    push[0] = native_push_encode(0x1880 + slot * 8, 2);
    ((real *)push)[1] = a;
    ((real *)push)[2] = b;
    return push + 3;
}

PRIVATE inline dword *native_push_vertex_color(dword *push, long slot,
    dword value)
{
    push[0] = native_push_encode(0x1940 + slot * 4, 1);
    push[1] = value;
    return push + 2;
}

// @retail 0x23ed30
void function_23ed30(point3f *position, point3f *direction, real scale,
    real angle, dword color, real blend, real *geometry, real *uv_a,
    real *uv_b)
{
    dword *push;
    if (!g_470a38 || !function_015d00(0x2e, &push))
        return;

    push = native_push_vertex4f(push, g_470a3c[0],
        direction->x, direction->y, direction->z, 1.0f);
    push = native_push_vertex2f(push, g_470a3c[1], blend, 0.0f);
    push = native_push_vertex4f(push, g_470a3c[3],
        position->x, position->y, position->z, 1.0f);
    push = native_push_vertex2f(push, g_470a3c[4], scale, angle);
    push = native_push_vertex_color(push, g_470a3c[7], color);
    push = native_push_vertex4f(push, g_470a3c[2],
        geometry[0], geometry[1], geometry[2], geometry[3]);
    push = native_push_vertex4f(push, g_470a3c[5],
        uv_a[0], uv_a[1], uv_a[2], uv_a[3]);
    push = native_push_vertex4f(push, g_470a3c[6],
        uv_b[0], uv_b[1], uv_b[2], uv_b[3]);

    *push++ = native_push_encode(0x17fc, 1);
    *push++ = 8;
    *push++ = native_push_encode(0x1818, 8) | 0x40000000;
    *push++ = 0;
    *push++ = 0;
    *push++ = 1;
    *push++ = 0;
    *push++ = 0x10001;
    *push++ = 0;
    *push++ = 0x10000;
    *push++ = 0;
    *push++ = native_push_encode(0x17fc, 1);
    *push++ = 0;
    D3DDevice_EndPush(push);
}
