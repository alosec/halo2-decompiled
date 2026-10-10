// Source-local native reconstruction of render-surface texture routing.
// The Xbox body remains in unknown_01cf50.cpp.

#include "unknown_11c920.h"
#include <xtl.h>

struct IDirect3DBaseTexture8;
struct D3DTexture;

extern IDirect3DBaseTexture8 *g_51f3c8[2][4];
extern D3DResource *g_509374;
extern D3DResource *g_50937c;
extern D3DTexture *g_509354[2];
extern D3DTexture *g_509368;
extern D3DTexture *g_50936c;
extern D3DTexture *g_50938c;
extern __int64 g_485aa0;
extern long g_485af4[4];
extern long g_485b04[4];

void *function_01dcc0(long index);
bool function_01dd60(long index, long *width, long *height);

// @retail 0x14f60
void function_14f60(short stage, short index)
{
    IDirect3DBaseTexture8 *texture = NULL;
    long width = 0;
    long height = 0;
    function_01dd60(index, &width, &height);
    g_485af4[stage] = width;
    g_485b04[stage] = height;

    if (index == 0)
        texture = (IDirect3DBaseTexture8 *)g_509354[(g_485aa0 - 1) % 2];
    else if (index == 4)
        texture = (IDirect3DBaseTexture8 *)g_509354[g_485aa0 % 2];
    else if (index == 16 || index == 29)
        texture = (IDirect3DBaseTexture8 *)g_509374;
    else if (index == 17)
        texture = (IDirect3DBaseTexture8 *)g_50937c;
    else if (index == 3)
        texture = (IDirect3DBaseTexture8 *)g_509368;
    else if (index == 24)
        texture = (IDirect3DBaseTexture8 *)g_50936c;
    else if (index == 1 || index == 18 || index == 19 || index == 13 || index == 20 ||
             index == 5 || index == 6 || index == 7 || index == 8 || index == 9 ||
             index == 10 || index == 11 || index == 12 || index == 15 || index == 21 ||
             index == 22 || index == 25 || index == 26 || index == 27)
        texture = (IDirect3DBaseTexture8 *)function_01dcc0(index);
    else if (index == 23)
        texture = (IDirect3DBaseTexture8 *)g_509354[(g_485aa0 - 1) % 2];
    else if (index >= 33 && index <= 38)
        texture = (IDirect3DBaseTexture8 *)g_50938c;
    else if (index == 14)
        texture = NULL;
    else
        __assume(0);

    g_51f3c8[1][stage] = texture;
}
