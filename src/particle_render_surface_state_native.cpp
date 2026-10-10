// Source-local native reconstruction of compact render-surface queries.
// The Xbox bodies remain in unknown_01dcc0.cpp.

#include "unknown_11c920.h"

struct s_unknown_01dcc0
{
    byte unknown00[4];
    long sub_header[5];
    long elements[4][6];
    long element_count;
    byte unknown7c[4];
    long width;
    long height;
    void *data;
    byte unknown8c[8];
    bool flag94;
    byte flag95;
    byte unknown96[2];
};
static_assert(sizeof(s_unknown_01dcc0) == 0x98,
    "s_unknown_01dcc0");

extern s_unknown_01dcc0 g_4b4b58[39];

// @retail 0x25960
long function_25960(void)
{
    long result = NONE;
    if (g_4b4b58[20].data && !g_4b4b58[20].flag95)
        return 20;
    if (g_4b4b58[18].data && !g_4b4b58[18].flag95)
        result = 18;
    return result;
}

// @retail 0x1dcc0
void *function_01dcc0(long index)
{
    s_unknown_01dcc0 *surface = &g_4b4b58[index];
    if (surface->data && !surface->flag95)
        return &surface->sub_header;
    return NULL;
}

// @retail 0x1dd60
bool function_01dd60(long index, long *width, long *height)
{
    s_unknown_01dcc0 *surface = &g_4b4b58[index];
    if (surface->data && !surface->flag95)
    {
        *width = surface->width;
        *height = surface->height;
        return true;
    }
    if (index == 0 || index == 3 || index == 0x18)
    {
        *width = 0x280;
        *height = 0x1e0;
        return true;
    }
    *width = 0;
    *height = 0;
    return false;
}
