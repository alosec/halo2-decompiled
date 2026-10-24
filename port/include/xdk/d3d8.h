/*
 * d3d8.h - clean-room Xbox Direct3D 8 compatibility header for the native port.
 *
 * PROVENANCE.  Nothing in this file is copied from a Microsoft Xbox SDK/XDK
 * header, from leaked material, or from GPL code (xemu, Cxbx-Reloaded, ...).
 * Every declaration comes from one of:
 *   - the recovered sources in src/ and include/: call sites give the
 *     prototypes, field accesses and static_asserts give the structure layouts;
 *   - the project's own retail analysis in docs/ (cited per value below);
 *   - the publicly documented PC Direct3D 8 API (names, public structure
 *     layouts and enumeration values);
 *   - the public OpenGL specification's enumerant values, where the verified
 *     members of a group show that the Xbox numbering follows them.
 *
 * STATUS OF THE VALUES
 *   verified (recovered source or docs/ retail analysis):
 *     - D3DRS_* marked "verified" (indices of the game's render-state cache
 *       g_4b81fc[], src/unknown_020560.cpp, and docs/unknown_03a5c0.md,
 *       docs/unknown_052040.md, docs/unknown_0508d0.md)
 *     - D3DTSS_ADDRESSU/ADDRESSV/MAGFILTER/MINFILTER/MIPFILTER (docs/
 *       unknown_052040.md, stores into the texture-stage array); MIPMAPLODBIAS,
 *       MAXMIPLEVEL, MAXANISOTROPY, COLORSIGN, ALPHAKILL (same document, marked
 *       "inferred" there); 32 states per stage (stage stride 0x80)
 *     - D3DCMP_LESSEQUAL/ALWAYS, D3DBLEND_ONE/SRCALPHA/INVSRCALPHA/
 *       CONSTANTCOLOR, D3DBLENDOP_ADD (docs/unknown_03a5c0.md)
 *     - D3DTADDRESS_CLAMP, D3DTEXF_POINT (docs/unknown_03a5c0.md)
 *     - D3DPT_QUADLIST (docs/unknown_052040.md); D3DPT_TRIANGLEFAN and
 *       D3DPT_TRIANGLELIST come from xtl.h and are not redefined here
 *     - D3DPUSH_* encoding (src/particle_sprite_geometry_native.cpp)
 *     - D3DDIRTYFLAG_TEXTURE_STATE_0 (docs/unknown_052040.md: bit 0 = stage 0)
 *     - layouts: D3DResource/D3DPixelContainer/D3DTexture (0x14 bytes),
 *       D3DPalette (0x0c, include/palette_cache_storage.h), D3DSurface (0x18,
 *       g_484f50 is followed by g_484f68), D3DPIXELSHADERDEF (0xf0 bytes,
 *       docs/unknown_03a5c0.md)
 *   PROVISIONAL (see the delimited blocks below):
 *     - D3DRS_* without "verified", all remaining D3DTSS_* (placeholders: the
 *       Xbox numbering is shown to differ from PC, so PC values would collide)
 *     - D3DCMP_*, D3DBLEND_*, D3DCULL_*, D3DSTENCILOP_*, D3DFILL_* not listed
 *       above (OpenGL numbering, which the verified members of these groups use)
 *     - D3DTADDRESS_WRAP/BORDER, D3DTEXF_NONE/LINEAR, D3DPT_POINTLIST/LINELIST/
 *       LINESTRIP/TRIANGLESTRIP, D3DFMT_*, D3DRTYPE_*, D3DCUBEMAP_FACES,
 *       D3DLOCK_*, D3DPRESENT*, D3DSWAPEFFECT_*, D3DBACKBUFFER_TYPE_*,
 *       D3DDEVTYPE_*, D3DCREATE_*, D3DVSDE_POSITION (public PC values)
 *     - D3DPRESENTFLAG_WIDESCREEN/PROGRESSIVE, D3DSDCP_*, D3DCALLBACK_*,
 *       D3DPALETTESIZE (Xbox-only, placeholders)
 *     - the PC-documented structure layouts (D3DPRESENT_PARAMETERS,
 *       D3DSURFACE_DESC, D3DVIEWPORT8, D3DGAMMARAMP, ...) and the placement of
 *       the D3DPIXELSHADERDEF fields that docs/ does not locate
 *
 * Prototypes of the flat Xbox functions are inferred from their call sites.
 * Where a recovered source declares one locally, the declaration here is the
 * same one.  Functions that xtl.h already declares are not repeated.
 */
#ifndef H2_PORT_XDK_D3D8_H
#define H2_PORT_XDK_D3D8_H
#include <xtl.h>

/* Base Win32 names the D3D API uses that xtl.h does not (re)declare.
 * Identical typedef redeclarations are allowed, so these are safe even if
 * xtl.h later adds them with the same types. */
typedef unsigned int UINT;
typedef int INT;
typedef short SHORT;
typedef unsigned long ULONG;
typedef float FLOAT;

typedef DWORD D3DCOLOR;

/* Public PC Direct3D 8 colour macro. */
#define D3DCOLOR_ARGB(a, r, g, b) \
    ((D3DCOLOR)((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)))
#define D3DCOLOR_XRGB(r, g, b) D3DCOLOR_ARGB(0xff, r, g, b)

/* ------------------------------------------------------------------------ */
/* Render states                                                             */
/* ------------------------------------------------------------------------ */

typedef enum _D3DRENDERSTATETYPE
{
    /* verified: g_4b81fc slot (state = (address - 0x4b81fc) / 4) next to a
       named D3DDevice_SetRenderState call in the recovered source, and/or the
       state tables of docs/unknown_03a5c0.md and docs/unknown_052040.md */
    D3DRS_ZFUNC                    = 57,   /* 0x39 */
    D3DRS_ALPHABLENDENABLE         = 59,   /* 0x3b */
    D3DRS_ALPHATESTENABLE          = 60,   /* 0x3c */
    D3DRS_SRCBLEND                 = 62,   /* 0x3e */
    D3DRS_DESTBLEND                = 63,   /* 0x3f */
    D3DRS_ZWRITEENABLE             = 64,   /* 0x40 */
    D3DRS_COLORWRITEENABLE         = 67,   /* 0x43 */
    D3DRS_BLENDOP                  = 74,   /* 0x4a */
    D3DRS_BLENDCOLOR               = 75,   /* 0x4b */
    D3DRS_POLYGONOFFSETZSLOPESCALE = 77,
    D3DRS_POLYGONOFFSETZOFFSET     = 78,
    D3DRS_SOLIDOFFSETENABLE        = 81,
    D3DRS_DEPTHCLIPCONTROL         = 82,   /* 0x52 */
    D3DRS_STIPPLEENABLE            = 83,
    D3DRS_FOGCOLOR                 = 138,
    D3DRS_TWOSIDEDLIGHTING         = 141,
    D3DRS_ZENABLE                  = 143,  /* 0x8f */
    D3DRS_STENCILENABLE            = 144,  /* 0x90 */
    D3DRS_CULLMODE                 = 147,  /* 0x93 */
    D3DRS_ZBIAS                    = 149,  /* 0x95 */
    D3DRS_MULTISAMPLEMODE          = 154,
    D3DRS_SHADOWFUNC               = 156,
    D3DRS_SAMPLEALPHA              = 158,  /* 0x9e, docs/unknown_0508d0.md (inferred there) */
    D3DRS_DXT1NOISEENABLE          = 159,

    /* PROVISIONAL: placeholder */
    /* The Xbox numbering is verified above to differ from the PC one (PC
       ZENABLE is 7, CULLMODE 22, ...), and the public PC values of the states
       below would collide with verified states (PC STENCILREF = 57 = ZFUNC,
       PC STENCILWRITEMASK = 59 = ALPHABLENDENABLE, ...).  These are arbitrary,
       collision-free values below the cache size 0xa6; none is verified.
       Note: game shader data (g_467020, src/unknown_0167a0.cpp) also sets raw
       states 61 (integer) and 157 (float), unnamed in the source. */
    D3DRS_PSFINALCOMBINERCONSTANT0 = 0,
    D3DRS_PSFINALCOMBINERCONSTANT1 = 1,
    D3DRS_ALPHAFUNC                = 2,
    D3DRS_ALPHAREF                 = 3,
    D3DRS_STENCILFAIL              = 4,
    D3DRS_STENCILZFAIL             = 5,
    D3DRS_STENCILPASS              = 6,
    D3DRS_STENCILFUNC              = 7,
    D3DRS_STENCILREF               = 8,
    D3DRS_STENCILMASK              = 9,
    D3DRS_STENCILWRITEMASK         = 10,
    D3DRS_FILLMODE                 = 11,
    D3DRS_FRONTFACE                = 12,
    D3DRS_LOGICOP                  = 13,
    D3DRS_LINEWIDTH                = 14,
    D3DRS_FOGENABLE                = 15,
    D3DRS_LIGHTING                 = 16,
    D3DRS_SPECULARENABLE           = 17,
    D3DRS_TEXTUREFACTOR            = 18,
    D3DRS_POINTSPRITEENABLE        = 19,
    D3DRS_POINTSCALEENABLE         = 20,
    D3DRS_POINTSCALE_A             = 21,
    D3DRS_POINTSCALE_B             = 22,
    D3DRS_POINTSCALE_C             = 23,
    /* end PROVISIONAL */

    D3DRS_MAX                      = 0xa6, /* size of the game's cache g_4b81fc */
    D3DRS_FORCE_DWORD              = 0x7fffffff
} D3DRENDERSTATETYPE;

/* ------------------------------------------------------------------------ */
/* Texture stage states                                                      */
/* ------------------------------------------------------------------------ */

typedef enum _D3DTEXTURESTAGESTATETYPE
{
    /* verified: stores into the texture-stage array (0x404dd0 + stage * 0x80
       + 4 * type), docs/unknown_052040.md */
    D3DTSS_ADDRESSU        = 0,
    D3DTSS_ADDRESSV        = 1,
    D3DTSS_MAGFILTER       = 3,
    D3DTSS_MINFILTER       = 4,
    D3DTSS_MIPFILTER       = 5,
    /* named from the store order in docs/unknown_052040.md ("inferred") */
    D3DTSS_MIPMAPLODBIAS   = 6,
    D3DTSS_MAXMIPLEVEL     = 7,
    D3DTSS_MAXANISOTROPY   = 8,
    D3DTSS_COLORSIGN       = 10,
    D3DTSS_ALPHAKILL       = 11,

    /* PROVISIONAL: placeholder */
    /* The Xbox numbering differs from PC (PC ADDRESSU is 13); arbitrary
       collision-free values below 32 (game shader data, g_46703c in
       src/unknown_0167a0.cpp, uses raw states 6, 22-27, 29 and 30). */
    D3DTSS_ADDRESSW        = 2,
    D3DTSS_COLORKEYOP      = 9,
    D3DTSS_TEXCOORDINDEX   = 12,
    D3DTSS_BUMPENVMAT00    = 22,
    D3DTSS_BUMPENVMAT01    = 23,
    D3DTSS_BUMPENVMAT10    = 24,
    D3DTSS_BUMPENVMAT11    = 25,
    D3DTSS_BORDERCOLOR     = 29,
    D3DTSS_COLORKEYCOLOR   = 30,
    /* end PROVISIONAL */

    D3DTSS_MAX             = 32, /* stage stride 0x80 bytes, docs/unknown_052040.md */
    D3DTSS_FORCE_DWORD     = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

/* ------------------------------------------------------------------------ */
/* State values                                                              */
/* ------------------------------------------------------------------------ */

typedef enum _D3DCMPFUNC
{
    /* verified: docs/unknown_03a5c0.md */
    D3DCMP_LESSEQUAL    = 0x203,
    D3DCMP_ALWAYS       = 0x207,
    /* PROVISIONAL: public OpenGL comparison values, which the verified
       members above follow; Xbox value not yet verified against the retail
       executable */
    D3DCMP_NEVER        = 0x200,
    D3DCMP_LESS         = 0x201,
    D3DCMP_EQUAL        = 0x202,
    D3DCMP_GREATER      = 0x204,
    D3DCMP_NOTEQUAL     = 0x205,
    D3DCMP_GREATEREQUAL = 0x206,
    /* end PROVISIONAL */
    D3DCMP_FORCE_DWORD  = 0x7fffffff
} D3DCMPFUNC;

typedef enum _D3DBLEND
{
    /* verified: docs/unknown_03a5c0.md, docs/unknown_052040.md */
    D3DBLEND_ONE              = 1,
    D3DBLEND_SRCALPHA         = 0x302,
    D3DBLEND_INVSRCALPHA      = 0x303,
    D3DBLEND_CONSTANTCOLOR    = 0x8001,
    /* PROVISIONAL: public OpenGL blend-factor values, which the verified
       members above follow; Xbox value not yet verified against the retail
       executable */
    D3DBLEND_ZERO             = 0,
    D3DBLEND_SRCCOLOR         = 0x300,
    D3DBLEND_INVSRCCOLOR      = 0x301,
    D3DBLEND_DESTALPHA        = 0x304,
    D3DBLEND_INVDESTALPHA     = 0x305,
    D3DBLEND_DESTCOLOR        = 0x306,
    D3DBLEND_INVDESTCOLOR     = 0x307,
    D3DBLEND_SRCALPHASAT      = 0x308,
    D3DBLEND_INVCONSTANTCOLOR = 0x8002,
    D3DBLEND_CONSTANTALPHA    = 0x8003,
    D3DBLEND_INVCONSTANTALPHA = 0x8004,
    /* end PROVISIONAL */
    D3DBLEND_FORCE_DWORD      = 0x7fffffff
} D3DBLEND;

typedef enum _D3DBLENDOP
{
    D3DBLENDOP_ADD         = 0x8006, /* verified: docs/unknown_03a5c0.md */
    D3DBLENDOP_FORCE_DWORD = 0x7fffffff
} D3DBLENDOP;

typedef enum _D3DCULL
{
    /* PROVISIONAL: CULLMODE 0 is the "off" value throughout the source; CW and
       CCW use the public OpenGL front-face values (the source sets CULLMODE to
       raw 0x900/0x901 and FRONTFACE to 0x900); Xbox value not yet verified
       against the retail executable */
    D3DCULL_NONE        = 0,
    D3DCULL_CW          = 0x900,
    D3DCULL_CCW         = 0x901,
    /* end PROVISIONAL */
    D3DCULL_FORCE_DWORD = 0x7fffffff
} D3DCULL;

typedef enum _D3DSTENCILOP
{
    /* PROVISIONAL: public OpenGL stencil-op values (the numbering the verified
       comparison and blend groups follow); Xbox value not yet verified
       against the retail executable */
    D3DSTENCILOP_ZERO        = 0,
    D3DSTENCILOP_KEEP        = 0x1e00,
    D3DSTENCILOP_REPLACE     = 0x1e01,
    D3DSTENCILOP_INCRSAT     = 0x1e02,
    D3DSTENCILOP_DECRSAT     = 0x1e03,
    D3DSTENCILOP_INVERT      = 0x150a,
    /* end PROVISIONAL */
    D3DSTENCILOP_FORCE_DWORD = 0x7fffffff
} D3DSTENCILOP;

typedef enum _D3DFILLMODE
{
    /* PROVISIONAL: public OpenGL polygon-mode values; Xbox value not yet
       verified against the retail executable */
    D3DFILL_POINT       = 0x1b00,
    D3DFILL_WIREFRAME   = 0x1b01,
    D3DFILL_SOLID       = 0x1b02,
    /* end PROVISIONAL */
    D3DFILL_FORCE_DWORD = 0x7fffffff
} D3DFILLMODE;

typedef enum _D3DTEXTUREADDRESS
{
    D3DTADDRESS_CLAMP       = 3, /* verified: docs/unknown_03a5c0.md */
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DTADDRESS_WRAP        = 1,
    D3DTADDRESS_MIRROR      = 2,
    D3DTADDRESS_BORDER      = 4,
    /* end PROVISIONAL */
    D3DTADDRESS_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREADDRESS;

typedef enum _D3DTEXTUREFILTERTYPE
{
    D3DTEXF_POINT       = 1, /* verified: docs/unknown_03a5c0.md */
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DTEXF_NONE        = 0,
    D3DTEXF_LINEAR      = 2,
    D3DTEXF_ANISOTROPIC = 3,
    /* end PROVISIONAL */
    D3DTEXF_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREFILTERTYPE;

/* Primitive types.  xtl.h defines D3DPT_TRIANGLELIST and D3DPT_TRIANGLEFAN
 * (7, retail-verified there) as plain macros, so the type is a DWORD and every
 * member is a macro.  The Xbox values are the NV2A begin/end codes: the two
 * verified ones (FAN 7, QUADLIST 8) are those codes, and the game's own table
 * g_43fdc0 = {1, 2, 5, 8, 6} (src/unknown_01cf50.cpp) turns its primitive
 * kinds (points, lines, triangles, quads, strips) into the same codes for its
 * inline pushes. */
typedef DWORD D3DPRIMITIVETYPE;
#ifndef D3DPT_TRIANGLELIST
#define D3DPT_TRIANGLELIST 5
#endif
#ifndef D3DPT_TRIANGLEFAN
#define D3DPT_TRIANGLEFAN 7
#endif
#define D3DPT_QUADLIST 8 /* verified: docs/unknown_052040.md */
/* PROVISIONAL: the NV2A codes, by the reasoning above; not yet each seen in
   the retail executable */
#define D3DPT_POINTLIST 1
#define D3DPT_LINELIST 2
#define D3DPT_LINELOOP 3
#define D3DPT_LINESTRIP 4
#define D3DPT_TRIANGLESTRIP 6
#define D3DPT_QUADSTRIP 9
#define D3DPT_POLYGON 10
/* end PROVISIONAL */

/* Formats and resource types.  The formats in the game's own tables
 * (g_450768, ...) are raw numbers cast to D3DFORMAT. */
typedef enum _D3DFORMAT
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DFMT_UNKNOWN     = 0,
    D3DFMT_A8R8G8B8    = 21,
    D3DFMT_D24S8       = 75,
    /* end PROVISIONAL */
    D3DFMT_FORCE_DWORD = 0x7fffffff
} D3DFORMAT;

typedef enum _D3DRESOURCETYPE
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DRTYPE_SURFACE       = 1,
    D3DRTYPE_VOLUME        = 2,
    D3DRTYPE_TEXTURE       = 3,
    D3DRTYPE_VOLUMETEXTURE = 4,
    D3DRTYPE_CUBETEXTURE   = 5,
    D3DRTYPE_VERTEXBUFFER  = 6,
    D3DRTYPE_INDEXBUFFER   = 7,
    /* end PROVISIONAL */
    D3DRTYPE_FORCE_DWORD   = 0x7fffffff
} D3DRESOURCETYPE;

typedef enum _D3DCUBEMAP_FACES
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DCUBEMAP_FACE_POSITIVE_X  = 0,
    D3DCUBEMAP_FACE_NEGATIVE_X  = 1,
    D3DCUBEMAP_FACE_POSITIVE_Y  = 2,
    D3DCUBEMAP_FACE_NEGATIVE_Y  = 3,
    D3DCUBEMAP_FACE_POSITIVE_Z  = 4,
    D3DCUBEMAP_FACE_NEGATIVE_Z  = 5,
    /* end PROVISIONAL */
    D3DCUBEMAP_FACE_FORCE_DWORD = 0x7fffffff
} D3DCUBEMAP_FACES;

typedef enum _D3DBACKBUFFER_TYPE
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DBACKBUFFER_TYPE_MONO        = 0,
    /* end PROVISIONAL */
    D3DBACKBUFFER_TYPE_FORCE_DWORD = 0x7fffffff
} D3DBACKBUFFER_TYPE;

typedef enum _D3DDEVTYPE
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DDEVTYPE_HAL         = 1,
    /* end PROVISIONAL */
    D3DDEVTYPE_FORCE_DWORD = 0x7fffffff
} D3DDEVTYPE;

typedef enum _D3DSWAPEFFECT
{
    /* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
    D3DSWAPEFFECT_DISCARD     = 1,
    D3DSWAPEFFECT_FLIP        = 2,
    D3DSWAPEFFECT_COPY        = 3,
    /* end PROVISIONAL */
    D3DSWAPEFFECT_FORCE_DWORD = 0x7fffffff
} D3DSWAPEFFECT;

/* PROVISIONAL: public PC Direct3D 8 value; Xbox value not yet verified against the retail executable */
#define D3DCREATE_HARDWARE_VERTEXPROCESSING 0x00000040L
#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER  0x00000001
#define D3DPRESENT_INTERVAL_IMMEDIATE       0x80000000L
#define D3DLOCK_NOOVERWRITE                 0x00001000L
#define D3DVSDE_POSITION                    0
/* end PROVISIONAL */
#ifndef D3DVSDE_VERTEX
#define D3DVSDE_VERTEX 0
#endif

/* PROVISIONAL: placeholder (Xbox-only names, no public value) */
#define D3DPRESENTFLAG_WIDESCREEN           0x00000010
#define D3DPRESENTFLAG_PROGRESSIVE          0x00000020
#define D3DSDCP_SET_VERTEXPROGRAM_PLANES    1
#define D3DSDCP_SET_FIXEDFUNCTION_PLANES    2
/* end PROVISIONAL */

typedef enum _D3DCALLBACKTYPE
{
    /* PROVISIONAL: placeholder */
    D3DCALLBACK_READ        = 0,
    D3DCALLBACK_WRITE       = 1,
    /* end PROVISIONAL */
    D3DCALLBACK_FORCE_DWORD = 0x7fffffff
} D3DCALLBACKTYPE;

typedef enum _D3DPALETTESIZE
{
    /* PROVISIONAL: placeholder (the source only casts table values to it) */
    D3DPALETTE_256         = 0,
    D3DPALETTE_128         = 1,
    D3DPALETTE_64          = 2,
    D3DPALETTE_32          = 3,
    /* end PROVISIONAL */
    D3DPALETTE_FORCE_DWORD = 0x7fffffff
} D3DPALETTESIZE;

/* verified: bit 0 is stage 0's texture-state dirty bit and the stage loop sets
   bit 'stage' (docs/unknown_052040.md) */
#define D3DDIRTYFLAG_TEXTURE_STATE_0 0x00000001

/* Push-buffer encoding, verified against the native reconstruction in
 * src/particle_sprite_geometry_native.cpp (method | count << 18; 0x17fc is the
 * begin/end method, 0x1818 the inline array, 0x40000000 the no-increment
 * flag). */
#define D3DPUSH_ENCODE(method, count) ((DWORD)(method) | ((DWORD)(count) << 18))
#define D3DPUSH_SET_BEGIN_END    0x000017fc
#define D3DPUSH_INLINE_ARRAY     0x00001818
#define D3DPUSH_NOINCREMENT_FLAG 0x40000000

/* ------------------------------------------------------------------------ */
/* Plain structures                                                          */
/* ------------------------------------------------------------------------ */

/* Public PC Direct3D 8 layouts (PROVISIONAL for the Xbox: only the fields
 * the recovered source uses are known to exist). */
typedef struct _D3DRECT
{
    LONG x1, y1, x2, y2;
} D3DRECT;

typedef struct _D3DBOX
{
    UINT Left, Top, Right, Bottom, Front, Back;
} D3DBOX;

typedef struct _D3DLOCKED_RECT
{
    INT Pitch;
    void *pBits;
} D3DLOCKED_RECT;

typedef struct _D3DLOCKED_BOX
{
    INT RowPitch;
    INT SlicePitch;
    void *pBits;
} D3DLOCKED_BOX;

typedef struct _D3DVIEWPORT8
{
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
} D3DVIEWPORT8;

typedef struct _D3DGAMMARAMP
{
    WORD red[256];
    WORD green[256];
    WORD blue[256];
} D3DGAMMARAMP;

typedef struct _D3DSURFACE_DESC
{
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    UINT Size;
    DWORD MultiSampleType;
    UINT Width;
    UINT Height;
} D3DSURFACE_DESC;

typedef struct _D3DPRESENT_PARAMETERS
{
    UINT BackBufferWidth;
    UINT BackBufferHeight;
    D3DFORMAT BackBufferFormat;
    UINT BackBufferCount;
    DWORD MultiSampleType;
    D3DSWAPEFFECT SwapEffect;
    HANDLE hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT FullScreen_PresentationInterval;
} D3DPRESENT_PARAMETERS;

/* The program definition g_484f68.  Size 0xf0 (the retail code clears 0x3c
 * dwords).  Offsets marked "docs" come from docs/unknown_03a5c0.md; the
 * placement of the others (PROVISIONAL) fills the remaining words. */
typedef struct _D3DPIXELSHADERDEF
{
    DWORD PSAlphaInputs[8];          /* +0x00 docs */
    DWORD PSFinalCombinerInputsABCD; /* +0x20 docs */
    DWORD PSFinalCombinerInputsEFG;  /* +0x24 docs */
    DWORD PSConstant0[8];            /* +0x28 docs */
    DWORD PSConstant1[8];            /* +0x48 docs */
    DWORD PSAlphaOutputs[8];         /* +0x68 docs */
    DWORD PSRGBInputs[8];            /* +0x88 docs */
    DWORD PSCompareMode;             /* +0xa8 PROVISIONAL */
    DWORD PSFinalCombinerConstant0;  /* +0xac docs */
    DWORD PSFinalCombinerConstant1;  /* +0xb0 PROVISIONAL */
    DWORD PSRGBOutputs[8];           /* +0xb4 docs */
    DWORD PSCombinerCount;           /* +0xd4 docs */
    DWORD PSTextureModes;            /* +0xd8 docs */
    DWORD PSDotMapping;              /* +0xdc PROVISIONAL */
    DWORD PSInputTexture;            /* +0xe0 PROVISIONAL */
    DWORD PSC0Mapping;               /* +0xe4 PROVISIONAL */
    DWORD PSC1Mapping;               /* +0xe8 PROVISIONAL */
    DWORD PSFinalCombinerConstants;  /* +0xec PROVISIONAL */
} D3DPIXELSHADERDEF;
#ifdef __cplusplus
static_assert(sizeof(D3DPIXELSHADERDEF) == 0xf0, "D3DPIXELSHADERDEF");
#endif

/* Opaque to the recovered source: only pointers to these are formed. */
struct D3DVBLANKDATA;
struct D3DSTREAM_INPUT;
struct D3DVERTEXATTRIBUTEFORMAT;
#ifndef __cplusplus
typedef struct D3DVBLANKDATA D3DVBLANKDATA;
typedef struct D3DSTREAM_INPUT D3DSTREAM_INPUT;
typedef struct D3DVERTEXATTRIBUTEFORMAT D3DVERTEXATTRIBUTEFORMAT;
#endif

typedef void (__cdecl *D3DVBLANKCALLBACK)(struct D3DVBLANKDATA *data);
typedef void (__cdecl *D3DCALLBACK)(DWORD context);

/* ------------------------------------------------------------------------ */
/* Resources                                                                 */
/* ------------------------------------------------------------------------ */

/* Layouts from the recovered source: a resource header is three dwords
 * (D3DPalette is 0x0c, include/palette_cache_storage.h; the texture-cache
 * entry holds one in 12 bytes, src/unknown_12c0d0.cpp), a pixel container adds
 * Format and Size (D3DTexture is 0x14, the static_asserts of the
 * particle_texture_*_native.cpp files), and D3DSurface is 0x18 (g_484f50 is
 * followed by g_484f68). */
struct D3DResource;
struct D3DPixelContainer;
struct IDirect3DBaseTexture8;
struct D3DTexture;
struct D3DCubeTexture;
struct D3DVolumeTexture;
struct D3DSurface;
struct D3DPalette;
struct D3DDevice;
struct D3DIndexBuffer;
#ifndef __cplusplus
typedef struct D3DResource D3DResource;
typedef struct D3DPixelContainer D3DPixelContainer;
typedef struct D3DTexture D3DTexture;
typedef struct D3DCubeTexture D3DCubeTexture;
typedef struct D3DVolumeTexture D3DVolumeTexture;
typedef struct D3DSurface D3DSurface;
typedef struct D3DPalette D3DPalette;
typedef struct D3DDevice D3DDevice;
typedef struct D3DIndexBuffer D3DIndexBuffer;
#endif
/* The particle natives name the base texture IDirect3DBaseTexture8 (and their
   symbols are mangled with that tag), so it is the struct's tag here. */
typedef struct IDirect3DBaseTexture8 D3DBaseTexture;

#ifdef __cplusplus
extern "C" {
#endif

/* Flat Xbox entry points (prototypes inferred from the call sites).  Those
 * already declared by xtl.h (D3DResource_Release, D3DDevice_Begin, _End,
 * _SetVertexData2f, _SetVertexData4f, _SetVertexDataColor,
 * _SetVertexShaderConstant, _DrawIndexedVertices, _SetTexture) are not
 * repeated. */
BOOL  __stdcall D3DResource_IsBusy(struct D3DResource *resource);
void  __stdcall D3DResource_BlockUntilNotBusy(struct D3DResource *resource);
void  __stdcall D3DResource_Register(struct D3DResource *resource, void *base);

void  __stdcall D3DSurface_GetDesc(struct D3DSurface *surface, D3DSURFACE_DESC *desc);
void  __stdcall D3DSurface_LockRect(struct D3DSurface *surface, D3DLOCKED_RECT *locked,
                                    void const *rect, DWORD flags);
void  __stdcall D3DSurface_UnlockRect(struct D3DSurface *surface);

struct D3DSurface *__stdcall D3DTexture_GetSurfaceLevel2(struct D3DTexture *texture, UINT level);
void  __stdcall D3DTexture_LockRect(struct D3DTexture *texture, UINT level, D3DLOCKED_RECT *locked,
                                    void const *rect, DWORD flags);
void  __stdcall D3DTexture_UnlockRect(struct D3DTexture *texture, UINT level);
void  __stdcall D3DCubeTexture_LockRect(struct D3DCubeTexture *texture, D3DCUBEMAP_FACES face,
                                        UINT level, D3DLOCKED_RECT *locked, void const *rect,
                                        DWORD flags);
void  __stdcall D3DCubeTexture_UnlockRect(struct D3DCubeTexture *texture, D3DCUBEMAP_FACES face,
                                          UINT level);
void  __stdcall D3DVolumeTexture_LockBox(struct D3DVolumeTexture *texture, UINT level,
                                         D3DLOCKED_BOX *locked, D3DBOX const *box, DWORD flags);
void  __stdcall D3DVolumeTexture_UnlockBox(struct D3DVolumeTexture *texture, UINT level);

D3DCOLOR *__stdcall D3DPalette_Lock2(struct D3DPalette *palette, DWORD flags);
void  __stdcall D3DPalette_Unlock(struct D3DPalette *palette);

HRESULT __stdcall Direct3D_CreateDevice(UINT adapter, D3DDEVTYPE type, void *focus_window,
                                        DWORD behavior, D3DPRESENT_PARAMETERS *parameters,
                                        struct D3DDevice **device);
void  __stdcall Direct3D_SetPushBufferSize(DWORD push_buffer_size, DWORD kickoff_size);

#ifndef H2_PORT_SHIMS
/* xtl.h declares these only for the native port; same prototypes otherwise. */
typedef struct D3DResource D3DResource;
DWORD __stdcall D3DResource_Release(D3DResource *resource);
void __stdcall D3DDevice_Begin(DWORD primitive_type);
void __stdcall D3DDevice_End(void);
void __stdcall D3DDevice_SetVertexData2f(INT vertex_register, float x, float y);
void __stdcall D3DDevice_SetVertexShaderConstant(int register_index, void const *constant_data,
                                                 DWORD constant_count);
void __stdcall D3DDevice_SetVertexData4f(DWORD vertex_register, float x, float y, float z, float w);
void __stdcall D3DDevice_SetVertexDataColor(DWORD vertex_register, DWORD color);
void __stdcall D3DDevice_DrawIndexedVertices(DWORD primitive_type, DWORD count,
                                             WORD const *indices);
void __stdcall D3DDevice_SetTexture(DWORD stage, void *texture);
#endif

/* Declared the same way by src/particle_*_native.cpp (keep identical). */
void  __stdcall D3DDevice_SetRenderState(DWORD state, DWORD value);
void  __stdcall D3DDevice_SetRenderStateNotInline(long state, DWORD value);
DWORD *__stdcall D3DDevice_BeginPush(DWORD dword_count);
void  __stdcall D3DDevice_EndPush(DWORD *push);
void  __stdcall D3DDevice_SetPixelShaderProgram(void const *program);
void  __stdcall D3DDevice_SetVertexShaderConstantFast(int register_index, void const *constant_data,
                                                      DWORD constant_count);
void  __stdcall D3DDevice_LoadVertexShaderProgram(DWORD const *program, DWORD address);
void  __stdcall D3DDevice_SetVertexShaderInputDirect(void const *formats, long stream_count,
                                                     void const *streams);
void  __stdcall D3DDevice_SelectVertexShaderDirect(void const *formats, long shader);

void  __stdcall D3DDevice_GetRenderState(DWORD state, DWORD *value);
void  __stdcall D3DDevice_SetRenderState_Simple(DWORD method, DWORD value);
void  __stdcall D3DDevice_SetRenderState_ZEnable(DWORD value);
void  __stdcall D3DDevice_SetRenderState_ZBias(DWORD value);
void  __stdcall D3DDevice_SetTextureStageState(DWORD stage, DWORD type, DWORD value);
void  __stdcall D3DDevice_GetTextureStageState(DWORD stage, DWORD type, DWORD *value);
void  __stdcall D3DDevice_SetVertexData2s(DWORD vertex_register, SHORT a, SHORT b);
void  __stdcall D3DDevice_SetVertexData4ub(DWORD vertex_register, BYTE a, BYTE b, BYTE c, BYTE d);
void  __stdcall D3DDevice_SetVertexShaderConstantNotInline(int register_index,
                                                           void const *constant_data,
                                                           DWORD constant_count);
void  __stdcall D3DDevice_SetVertexShader(DWORD handle);
void  __stdcall D3DDevice_SetPixelShader(DWORD handle);
void  __stdcall D3DDevice_DrawVertices(D3DPRIMITIVETYPE primitive_type, UINT start, UINT count);
void  __stdcall D3DDevice_SetIndices(struct D3DIndexBuffer *indices, UINT base_vertex);
void  __stdcall D3DDevice_Clear(DWORD count, D3DRECT const *rects, DWORD flags, D3DCOLOR color,
                                float z, DWORD stencil);
void  __stdcall D3DDevice_SetViewport(D3DVIEWPORT8 const *viewport);
void  __stdcall D3DDevice_GetViewport(D3DVIEWPORT8 *viewport);
void  __stdcall D3DDevice_SetScissors(DWORD count, BOOL exclusive, D3DRECT const *rects);
void  __stdcall D3DDevice_SetStipple(DWORD const *pattern);
void  __stdcall D3DDevice_SetDepthClipPlanes(float near_plane, float far_plane, DWORD flags);
void  __stdcall D3DDevice_SetPalette(DWORD stage, struct D3DPalette *palette);
struct D3DPalette *__stdcall D3DDevice_CreatePalette2(D3DPALETTESIZE size);
struct D3DTexture *__stdcall D3DDevice_CreateTexture2(UINT width, UINT height, UINT depth,
                                                      UINT levels, DWORD usage, D3DFORMAT format,
                                                      D3DRESOURCETYPE type);
void  __stdcall D3DDevice_SetRenderTarget(struct D3DSurface *target, struct D3DSurface *depth);
void  __stdcall D3DDevice_SetRenderTargetFast(struct D3DSurface *target, struct D3DSurface *depth,
                                              DWORD flags);
void  __stdcall D3DDevice_GetRenderTarget(struct D3DSurface **target);
void  __stdcall D3DDevice_GetBackBuffer(INT index, D3DBACKBUFFER_TYPE type,
                                        struct D3DSurface **surface);
struct D3DSurface *__stdcall D3DDevice_GetBackBuffer2(INT index);
void  __stdcall D3DDevice_GetDepthStencilSurface(struct D3DSurface **surface);
void  __stdcall D3DDevice_SetBackBufferScale(float x, float y);
void  __stdcall D3DDevice_SetFlickerFilter(DWORD filter);
void  __stdcall D3DDevice_SetSoftDisplayFilter(BOOL enable);
void  __stdcall D3DDevice_SetGammaRamp(DWORD flags, D3DGAMMARAMP const *ramp);
void  __stdcall D3DDevice_SetVerticalBlankCallback(D3DVBLANKCALLBACK callback);
void  __stdcall D3DDevice_InsertCallback(D3DCALLBACKTYPE type, D3DCALLBACK callback, DWORD context);
void  __stdcall D3DDevice_Swap(DWORD flags);
void  __stdcall D3DDevice_PersistDisplay(void);
void  __stdcall D3DDevice_Reset(D3DPRESENT_PARAMETERS *parameters);
ULONG __stdcall D3DDevice_Release(void);
void  __stdcall D3DDevice_KickPushBuffer(void);
BOOL  __stdcall D3DDevice_IsBusy(void);
void  __stdcall D3DDevice_FlushVertexCache(void);
DWORD *__stdcall D3DDevice_MakeSpace(void);
HRESULT __stdcall D3DDevice_GetVisibilityTestResult(DWORD index, UINT *result,
                                                    ULONGLONG *timestamp);
HRESULT __stdcall D3DDevice_EndVisibilityTest(DWORD index);

/* Library state the recovered source touches directly. */
extern struct D3DDevice *D3D__pDevice;
extern DWORD D3D__DirtyFlags;
extern DWORD D3D__RenderState[D3DRS_MAX];
extern DWORD D3D__TextureState[4][D3DTSS_MAX];
extern DWORD D3D__CommandBufferSize;
extern DWORD D3D__SegmentSize;
extern DWORD const D3DSIMPLERENDERSTATEENCODE[];

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct D3DResource
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;

    BOOL IsBusy() { return D3DResource_IsBusy(this); }
    void BlockUntilNotBusy() { D3DResource_BlockUntilNotBusy(this); }
};

struct D3DPixelContainer : D3DResource
{
    DWORD Format;
    DWORD Size;
};

struct IDirect3DBaseTexture8 : D3DPixelContainer
{
};

/* NOTE: src/particle_texture_cache_lifecycle_native.cpp and
   src/particle_texture_cache_request_native.cpp define their own identical
   struct D3DTexture (same five dwords, 0x14 bytes) after including <xtl.h>;
   that local copy has to go for those two units to compile with this one. */
struct D3DTexture : IDirect3DBaseTexture8
{
    void LockRect(UINT level, D3DLOCKED_RECT *locked, void const *rect, DWORD flags)
    {
        D3DTexture_LockRect(this, level, locked, rect, flags);
    }
    void UnlockRect(UINT level) { D3DTexture_UnlockRect(this, level); }
};

struct D3DCubeTexture : IDirect3DBaseTexture8
{
};

struct D3DVolumeTexture : IDirect3DBaseTexture8
{
};

struct D3DSurface : D3DPixelContainer
{
    D3DBaseTexture *Parent; /* PROVISIONAL name; the size is from g_484f50 */
};

struct D3DPalette : D3DResource
{
};

static_assert(sizeof(D3DResource) == 0x0c, "D3DResource");
static_assert(sizeof(D3DTexture) == 0x14, "D3DTexture");
static_assert(sizeof(D3DPalette) == 0x0c, "D3DPalette");
static_assert(sizeof(D3DSurface) == 0x18, "D3DSurface");

/* The device: the recovered source calls both the flat functions and the
 * D3DDevice:: static forms, and calls Release() on the device object. */
struct D3DDevice
{
    static void Begin(D3DPRIMITIVETYPE primitive_type) { D3DDevice_Begin(primitive_type); }
    static void End() { D3DDevice_End(); }
    static void Clear(DWORD count, D3DRECT const *rects, DWORD flags, D3DCOLOR color, float z,
                      DWORD stencil)
    {
        D3DDevice_Clear(count, rects, flags, color, z, stencil);
    }
    static void GetBackBuffer(INT index, D3DBACKBUFFER_TYPE type, D3DSurface **surface)
    {
        D3DDevice_GetBackBuffer(index, type, surface);
    }
    static void GetRenderTarget(D3DSurface **target) { D3DDevice_GetRenderTarget(target); }
    static void SetRenderState(DWORD state, DWORD value) { D3DDevice_SetRenderState(state, value); }
    static void SetTextureStageState(DWORD stage, DWORD type, DWORD value)
    {
        D3DDevice_SetTextureStageState(stage, type, value);
    }
    static void SetVertexData2f(INT vertex_register, float a, float b)
    {
        D3DDevice_SetVertexData2f(vertex_register, a, b);
    }
    static void SetVertexData2s(DWORD vertex_register, SHORT a, SHORT b)
    {
        D3DDevice_SetVertexData2s(vertex_register, a, b);
    }
    static void SetVertexData4f(DWORD vertex_register, float a, float b, float c, float d)
    {
        D3DDevice_SetVertexData4f(vertex_register, a, b, c, d);
    }
    static void SetVertexDataColor(DWORD vertex_register, D3DCOLOR color)
    {
        D3DDevice_SetVertexDataColor(vertex_register, color);
    }
    ULONG Release() { return D3DDevice_Release(); }
};

/* D3D::CDevice, D3D::MakeRequestedSpace, D3D::SetFence and D3D::PixelJar are
 * declared by the recovered sources that use them (src/unknown_0158f0.cpp,
 * unknown_015d00.cpp, unknown_020560.cpp, unknown_023540.cpp), not here. */

typedef D3DResource IDirect3DResource8;
typedef D3DTexture IDirect3DTexture8;
typedef D3DCubeTexture IDirect3DCubeTexture8;
typedef D3DVolumeTexture IDirect3DVolumeTexture8;
typedef D3DSurface IDirect3DSurface8;
typedef D3DPalette IDirect3DPalette8;
typedef D3DDevice IDirect3DDevice8;

static __forceinline void IDirect3DDevice8_Clear(D3DDevice *device, DWORD count,
                                                 D3DRECT const *rects, DWORD flags,
                                                 D3DCOLOR color, float z, DWORD stencil)
{
    (void)device;
    D3DDevice_Clear(count, rects, flags, color, z, stencil);
}

#else /* C */

struct D3DResource { DWORD Common, Data, Lock; };
struct D3DPixelContainer { DWORD Common, Data, Lock, Format, Size; };
struct IDirect3DBaseTexture8 { DWORD Common, Data, Lock, Format, Size; };
struct D3DTexture { DWORD Common, Data, Lock, Format, Size; };
struct D3DCubeTexture { DWORD Common, Data, Lock, Format, Size; };
struct D3DVolumeTexture { DWORD Common, Data, Lock, Format, Size; };
struct D3DSurface { DWORD Common, Data, Lock, Format, Size; D3DBaseTexture *Parent; };
struct D3DPalette { DWORD Common, Data, Lock; };

#endif /* __cplusplus */

typedef D3DBaseTexture *LPDIRECT3DBASETEXTURE8;
typedef struct D3DTexture *LPDIRECT3DTEXTURE8;
typedef struct D3DSurface *LPDIRECT3DSURFACE8;
typedef struct D3DDevice *LPDIRECT3DDEVICE8;

#endif
