// @flags /O2 /arch:SSE /Gr
/* INTERFACE_RECTANGLE.CPP: filling a screen rectangle with one colour, as a
   quad drawn through the screen-space quad drawer (function_52040) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "camera_storage.h"
#include "globals.h"
#include "interface_rectangle.h"
#include <string.h>

void __stdcall function_52040(void const *request, void const *vertices);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
/* fills bounds with color (a packed ARGB colour) using the second bitmap of
   the bitmap tag the global tag data names; nothing is drawn without a
   scenario */
// @retail 0x13c050
void interface_rectangle_fill(short_rectangle2d const *bounds, dword color)
{
	if (g_4e0350)
	{
		s_interface_rectangle_globals *globals = (s_interface_rectangle_globals *)g_4e034c;
		s_interface_rectangle_globals_entry *entry = globals->entry_count ? globals->entries : NULL;
		s_interface_bitmap_tag *bitmap_tag = (s_interface_bitmap_tag *)g_4e3b44[entry->bitmap_tag_index & 0xffff].bytes;
		real left = (real)bounds->left;
		long top = bounds->top;
		_ReadWriteBarrier();
		s_interface_bitmap *bitmaps = bitmap_tag->bitmaps;
		point2f corners[4];
		corners[0].x = left;
		corners[0].y = top;
		corners[1].x = bounds->right;
		corners[1].y = top;
		corners[2].x = bounds->right;
		corners[2].y = bounds->bottom;
		corners[3].x = left;
		corners[3].y = bounds->bottom;
		s_interface_quad_vertex vertices[4];
		vertices[0].position = corners[0];
		vertices[1].position = corners[1];
		vertices[0].texture.x = 0.0f;
		vertices[0].texture.y = 0.0f;
		vertices[1].texture.x = 0.0f;
		vertices[1].texture.y = 0.0f;
		vertices[2].texture.x = 0.0f;
		vertices[2].texture.y = 0.0f;
		vertices[3].texture.x = 0.0f;
		vertices[3].texture.y = 0.0f;
		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[2].position = corners[2];
		vertices[3].color = color;
		vertices[3].position = corners[3];
		s_interface_quad_request request;
		memset(&request, 0, sizeof(request));
		request.bitmaps[0] = &bitmaps[1];
		request.blend_mode = 0;
		request.flags = 0;
		request.point_sampled = false;
		request.transform_scale_y = 1.0f;
		request.transform_scale_x = 1.0f;
		request.shader_scale_y = 1.0f;
		request.shader_scale_x = 1.0f;
		function_52040(&request, vertices);
	}
}
#pragma function(_ReadWriteBarrier)
