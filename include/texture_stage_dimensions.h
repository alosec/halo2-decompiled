#ifndef TEXTURE_STAGE_DIMENSIONS_H
#define TEXTURE_STAGE_DIMENSIONS_H

/* the width and height of the bitmap bound to each texture stage: one block,
   prefetched by address (src/unknown_01cf50.cpp) */
struct s_texture_stage_dimensions
{
	long width[4];
	long height[4];
};

extern s_texture_stage_dimensions g_texture_stage_dimensions;
#define g_485af4 (g_texture_stage_dimensions.width)
#define g_485b04 (g_texture_stage_dimensions.height)

#endif
