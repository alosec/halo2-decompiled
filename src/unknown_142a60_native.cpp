// Native scalar split of the recovered transform composition at retail
// 0x142a60. The matching Xbox definition remains in unknown_1428b0.cpp as
// its original SSE assembly. Snapshot both inputs to preserve in-place use.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <stdint.h>

static vector3f compose_basis(matrix3x3 const *basis, vector3f const *column)
{
    vector3f result;
    result.i = basis->forward.i * column->i + basis->left.i * column->j + basis->up.i * column->k;
    result.j = basis->forward.j * column->i + basis->left.j * column->j + basis->up.j * column->k;
    result.k = basis->forward.k * column->i + basis->left.k * column->j + basis->up.k * column->k;
    return result;
}

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *out)
{
    transform4x3f const left = *a;
    transform4x3f const right = *b;

    out->scale = left.scale * right.scale;
    out->forward = compose_basis(&left.rotation, &right.forward);
    out->left = compose_basis(&left.rotation, &right.left);
    out->up = compose_basis(&left.rotation, &right.up);

    vector3f position;
    position.i = right.position.x;
    position.j = right.position.y;
    position.k = right.position.z;
    vector3f const translated = compose_basis(&left.rotation, &position);
    out->position.x = left.position.x + left.scale * translated.i;
    out->position.y = left.position.y + left.scale * translated.j;
    out->position.z = left.position.z + left.scale * translated.k;
    return (int)(uintptr_t)out;
}
