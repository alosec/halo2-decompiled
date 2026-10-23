// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_03bcb0.h"

struct s_bitmap_widget_22ae21
{
	byte unknown00[0x1c];
	word mode;
	byte unknown1e[6];
	long bitmap_tag;
	byte unknown28[4];
	long unknown2c;
	char sequences[3];
};

void function_22ae21(long local_player_index, s_bitmap_widget_22ae21 const *widget,
	long *bitmap_index, long *width, long *height, box2f *bounds);

// @retail 0x22af8d
bool function_22af8d(long index, byte const *unused, s_bitmap_widget_22ae21 const *widget)
{
	// Keep the unused parameter in its retail stack slot.
	byte const *const *unused_reference = &unused;
	(void)*unused_reference;
	bool result = true;
	if (widget->unknown2c != NONE && widget->bitmap_tag != NONE)
	{
		long width;
		long height;
		box2f bounds;
		function_22ae21(index, widget, &index, &width, &height, &bounds);
		if (index != NONE)
		{
			byte *tag = g_4e3b44[widget->bitmap_tag & 0xffff].bytes;
			if (!function_3bcb0((s_bitmap_data *)(*(byte **)(tag + 0x48) + index * 0x74)))
				result = false;
		}
	}
	return result;
}
