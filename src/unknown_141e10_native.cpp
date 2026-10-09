// Native scalar split of the recovered quaternion-to-matrix conversion at
// retail 0x141e10. The matching Xbox definition remains in
// unknown_141590.cpp, whose translation unit also contains x86-only code.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *quaternion)
{
    quaternionf const q = *quaternion;
    real const norm = q.i * q.i + q.j * q.j + q.k * q.k + q.w * q.w;
    real const scale = norm > 0.0001f ? 2.0f / norm : 0.0f;
    real const xs = q.i * scale;
    real const ys = q.j * scale;
    real const zs = q.k * scale;
    real const wx = q.w * xs;
    real const wy = q.w * ys;
    real const wz = q.w * zs;
    real const xx = q.i * xs;
    real const xy = q.i * ys;
    real const xz = q.i * zs;
    real const yy = q.j * ys;
    real const yz = q.j * zs;
    real const zz = q.k * zs;

    out->forward.i = 1.0f - (yy + zz);
    out->left.i = xy - wz;
    out->up.i = xz + wy;
    out->forward.j = xy + wz;
    out->left.j = 1.0f - (xx + zz);
    out->up.j = yz - wx;
    out->forward.k = xz - wy;
    out->left.k = yz + wx;
    out->up.k = 1.0f - (xx + yy);
    return out;
}
