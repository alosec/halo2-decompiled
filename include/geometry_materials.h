/* GEOMETRY_MATERIALS.H: a geometry material and the part of its shader tag
   that function_13da30 (0x13da30) follows to the material's bitmap */
#ifndef GEOMETRY_MATERIALS_H
#define GEOMETRY_MATERIALS_H

#include <stddef.h>
#include "unknown_11c920.h"

/* an element of the shader's bitmap block (12 bytes): a bitmap tag */
struct s_material_bitmap
{
	long bitmap_tag_index;
	byte unknown04[8];
};
typedef char assert_material_bitmap_size[(sizeof(s_material_bitmap) == 0xc) ? 1 : -1];

/* what the shader tag points to at +0x24: its bitmaps, and the indices into
   them (the first index picks the material's bitmap; NONE for none) */
struct s_material_shader_data
{
	byte unknown00[8];
	s_material_bitmap *bitmaps;
	byte unknown0c[0x60 - 0xc];
	short *bitmap_index_table;
};
typedef char assert_material_shader_data_bitmaps[(offsetof(s_material_shader_data, bitmaps) == 0x8) ? 1 : -1];
typedef char assert_material_shader_data_indices[(offsetof(s_material_shader_data, bitmap_index_table) == 0x60) ? 1 : -1];

/* the part of a shader tag read here */
struct s_material_shader
{
	byte unknown00[0x24];
	s_material_shader_data *data;
};
typedef char assert_material_shader_data[(offsetof(s_material_shader, data) == 0x24) ? 1 : -1];

/* a geometry material (0x20 bytes in the arrays its callers index): its
   shader tag. The name stays a placeholder: callers declare
   function_13da30 with it. */
struct s_13da30_owner
{
	byte unknown00[0xc];
	long shader_tag_index;
};
typedef char assert_13da30_owner_shader[(offsetof(s_13da30_owner, shader_tag_index) == 0xc) ? 1 : -1];

#endif
