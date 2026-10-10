// Native source split of recovered 3x3 matrix composition used by particle
// frame placement. The matching Xbox definition remains in unknown_1428b0.cpp,
// whose unrelated functions contain x86 SSE assembly.

#include "unknown_11c920.h"
#include "effects.h"

// @retail 0x142eb0
matrix3x3 *function_142eb0(matrix3x3 const *a, matrix3x3 const *b, matrix3x3 *out)
{
    matrix3x3 temp;
    if (b == out)
    {
        temp = *b;
        b = &temp;
    }
    if (a == out)
    {
        temp = *a;
        a = &temp;
    }
    out->forward.i = b->forward.i * a->forward.i + b->left.i * a->forward.j + b->up.i * a->forward.k;
    out->forward.j = b->forward.j * a->forward.i + b->left.j * a->forward.j + b->up.j * a->forward.k;
    out->forward.k = b->forward.k * a->forward.i + b->left.k * a->forward.j + b->up.k * a->forward.k;
    out->left.i = b->forward.i * a->left.i + b->left.i * a->left.j + b->up.i * a->left.k;
    out->left.j = b->forward.j * a->left.i + b->left.j * a->left.j + b->up.j * a->left.k;
    out->left.k = b->forward.k * a->left.i + b->left.k * a->left.j + b->up.k * a->left.k;
    out->up.i = b->forward.i * a->up.i + b->left.i * a->up.j + b->up.i * a->up.k;
    out->up.j = b->forward.j * a->up.i + b->left.j * a->up.j + b->up.j * a->up.k;
    out->up.k = b->forward.k * a->up.i + b->left.k * a->up.j + b->up.k * a->up.k;
    return out;
}
