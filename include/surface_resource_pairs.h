#ifndef SURFACE_RESOURCE_PAIRS_H
#define SURFACE_RESOURCE_PAIRS_H
#include <xtl.h>

struct s_surface_resource_pair
{
    D3DTexture *texture;
    D3DSurface *surface;
};
struct s_surface_resource_pairs
{
    s_surface_resource_pair pairs[2];
};
extern s_surface_resource_pairs g_surface_resource_pairs;
#define g_509374 (g_surface_resource_pairs.pairs[0].texture)
#define g_509378 (g_surface_resource_pairs.pairs[0].surface)
#define g_50937c (g_surface_resource_pairs.pairs[1].texture)
#define g_509380 (g_surface_resource_pairs.pairs[1].surface)
#endif
