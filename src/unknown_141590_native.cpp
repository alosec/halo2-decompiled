#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <string.h>

#ifdef H2_NATIVE_PORT
// Portable scalar form of the recovered inverse transform at retail 0x141590.
void function_141590(transform4x3f const *in, transform4x3f *out)
{
    if (in->scale == 0.0f)
    {
        memset(out, 0, sizeof(*out));
        return;
    }

    vector3f translated = {
        -in->position.x,
        -in->position.y,
        -in->position.z,
    };
    if (in->scale != 1.0f)
    {
        out->scale = 1.0f / in->scale;
        translated.i *= out->scale;
        translated.j *= out->scale;
        translated.k *= out->scale;
    }
    else
    {
        out->scale = 1.0f;
    }

    out->forward.i = in->forward.i;
    out->left.j = in->left.j;
    out->up.k = in->up.k;
    out->left.i = in->forward.j;
    out->forward.j = in->left.i;
    out->up.i = in->forward.k;
    out->forward.k = in->up.i;
    out->up.j = in->left.k;
    out->left.k = in->up.j;
    out->position.x = out->forward.i * translated.i + out->left.i * translated.j + out->up.i * translated.k;
    out->position.y = out->forward.j * translated.i + out->left.j * translated.j + out->up.j * translated.k;
    out->position.z = out->forward.k * translated.i + out->left.k * translated.j + out->up.k * translated.k;
}
#endif
