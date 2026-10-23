#ifndef TEXTURE_STAGE_STORAGE_H
#define TEXTURE_STAGE_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "xtl.h"

/* the texture stages ([0] the textures set on the device, [1] the textures
   wanted) and the render target state after them: one block, since the stage
   arrays are cleared by address (src/unknown_01cf50.cpp) */
struct s_texture_stage_storage
{
	D3DBaseTexture *textures[2][4];
	byte unknown20[8];
	byte unknown28;
	D3DSurface *target;
	D3DSurface *depth;
	byte target_flag;
};

extern s_texture_stage_storage g_texture_stage_storage;
#define g_51f3c8 (g_texture_stage_storage.textures)
#define g_51f3f0 (g_texture_stage_storage.unknown28)
#define g_51f3f4 (g_texture_stage_storage.target)
#define g_51f3f8 (g_texture_stage_storage.depth)
#define g_51f3fc (g_texture_stage_storage.target_flag)
typedef char assert_texture_stage_offset_28[(offsetof(s_texture_stage_storage, unknown28) == 0x28) ? 1 : -1];
typedef char assert_texture_stage_offset_2c[(offsetof(s_texture_stage_storage, target) == 0x2c) ? 1 : -1];
typedef char assert_texture_stage_offset_34[(offsetof(s_texture_stage_storage, target_flag) == 0x34) ? 1 : -1];

#endif
