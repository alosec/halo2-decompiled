#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// Native-port source split of retail 0x142570. The matching Xbox definition
// remains in unknown_141590.cpp, whose unrelated code contains x86 assembly.
point3f *transform4x3f_apply_point(
    transform4x3f const *matrix,
    point3f const *point,
    point3f *out)
{
    transform4x3f const *const *matrix_reference = &matrix;
    real x = point->x;
    real y = point->y;
    real z = point->z;
    if ((*matrix_reference)->scale != 1.0f)
    {
        x = matrix->scale * x;
        y = matrix->scale * y;
        z = matrix->scale * z;
    }
    out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
    out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
    out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
    return out;
}
