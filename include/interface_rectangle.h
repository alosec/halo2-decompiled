/* INTERFACE_RECTANGLE.H: what interface_rectangle_fill (0x13c050) hands to the
   screen-space quad drawer (function_52040), and the tag data it reaches for
   the bitmap it fills with */
#ifndef INTERFACE_RECTANGLE_H
#define INTERFACE_RECTANGLE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* one corner of a quad: screen position, texture coordinates, packed colour */
struct s_interface_quad_vertex
{
	point2f position;
	point2f texture;
	dword color;
};
typedef char assert_interface_quad_vertex_size[(sizeof(s_interface_quad_vertex) == 0x14) ? 1 : -1];

/* one element of a bitmap tag's bitmap block (0x74 bytes, as the other
   readers of that block step through it) */
struct s_interface_bitmap
{
	byte unknown00[0x74];
};
typedef char assert_interface_bitmap_size[(sizeof(s_interface_bitmap) == 0x74) ? 1 : -1];

/* the part of a bitmap tag read here: its bitmap block's elements */
struct s_interface_bitmap_tag
{
	byte unknown00[0x48];
	s_interface_bitmap *bitmaps;
};
typedef char assert_interface_bitmap_tag_bitmaps[(offsetof(s_interface_bitmap_tag, bitmaps) == 0x48) ? 1 : -1];

/* the first element of the block of the global tag data (g_4e034c) at +0x108:
   it references the bitmap tag the rectangles are filled with */
struct s_interface_rectangle_globals_entry
{
	byte unknown00[0x60];
	dword bitmap_group_tag;
	long bitmap_tag_index;
};
typedef char assert_interface_rectangle_globals_entry_tag[(offsetof(s_interface_rectangle_globals_entry, bitmap_tag_index) == 0x64) ? 1 : -1];

/* the part of the global tag data (g_4e034c) read here: that block (an empty
   block counts as none) */
struct s_interface_rectangle_globals
{
	byte unknown000[0x108];
	long entry_count;
	s_interface_rectangle_globals_entry *entries;
};
typedef char assert_interface_rectangle_globals_count[(offsetof(s_interface_rectangle_globals, entry_count) == 0x108) ? 1 : -1];
typedef char assert_interface_rectangle_globals_entries[(offsetof(s_interface_rectangle_globals, entries) == 0x10c) ? 1 : -1];

/* a quad drawing request (0x98 bytes), as function_52040 reads it: up to
   three bitmaps (one per texture stage), scales and shader constants, the
   blend mode and whether the bitmaps are point sampled rather than
   filtered. The members this code does not set stay as unknownNN. */
struct s_interface_quad_request
{
	dword flags;
	byte unknown04[8];
	s_interface_bitmap *bitmaps[3];
	byte unknown18[0x10];
	real shader_scale_x;
	real shader_scale_y;
	byte unknown30[0x10];
	real transform_scale_x;
	real transform_scale_y;
	byte unknown48[0x4c];
	short blend_mode;
	bool point_sampled;
	byte unknown97;
};
typedef char assert_interface_quad_request_bitmaps[(offsetof(s_interface_quad_request, bitmaps) == 0xc) ? 1 : -1];
typedef char assert_interface_quad_request_shader_scale[(offsetof(s_interface_quad_request, shader_scale_x) == 0x28) ? 1 : -1];
typedef char assert_interface_quad_request_transform_scale[(offsetof(s_interface_quad_request, transform_scale_x) == 0x40) ? 1 : -1];
typedef char assert_interface_quad_request_blend_mode[(offsetof(s_interface_quad_request, blend_mode) == 0x94) ? 1 : -1];
typedef char assert_interface_quad_request_point_sampled[(offsetof(s_interface_quad_request, point_sampled) == 0x96) ? 1 : -1];
typedef char assert_interface_quad_request_size[(sizeof(s_interface_quad_request) == 0x98) ? 1 : -1];

#endif
