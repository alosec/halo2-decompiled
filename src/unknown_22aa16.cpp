// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"

struct s_text_widget;
struct s_text_widget_state;
struct s_widget_transform_block;
struct s_interface_function_context;
struct s_33a0b_default;
extern s_interface_function_context g_5021d0;
extern s_33a0b_default *g_4686d4;
struct s_4e6950;
extern s_4e6950 g_4e6950;
class c_1fa50
{
public:
	void function_1fb10(short_rectangle2d const *, real) const;
};
void function_22a871(s_text_widget const *, s_text_widget_state const *, color4f const *, word *, long *);
void function_13edb0(long, long, long, dword, color4f const *, color4f const *);
bool function_13ee20(word const *, long);
long function_13a690(long);
void function_1be50(void);
void function_1bd50(void *);
void function_1bbf0(long, long, long, long, long, real);
void function_1cf50(void);
color3f *unpack_color3f(dword, color3f *);
void function_1396c7(long, point2f *);
void function_22bcaf(s_widget_transform_block const *, point2f *, point2f *, point2f *);
void function_13e9c0(word const *, short_rectangle2d const *, short_rectangle2d *, short_rectangle2d *, real);
void function_13eb60(color4f const *);
void function_13ed50(color4f const *);

// retail keeps all four arguments on the stack (ret 0x10); without the marker LTCG passes the first in a register
// @retail 0x22aa16 standard
void __stdcall function_22aa16(long arg_0, byte const *arg_1, byte const *arg_2, real const *arg_3)
{
	(void)&arg_0; (void)&arg_1; (void)&arg_2; (void)&arg_3;
	if (*(long const *)(arg_2 + 0x28) && *(long const *)(arg_2 + 0x24) != NONE)
	{
		long string_index = NONE;
		word text[256];

		text[0] = 0;
		function_22a871((s_text_widget const *)arg_2, (s_text_widget_state const *)arg_1,
			(color4f const *)arg_3, text, &string_index);
		function_13edb0(string_index, NONE, 0, 0, g_4686cc, (color4f const *)g_4686d4);
		if (function_13ee20(text, g_4e73a0.font))
		{
			long local_2 = function_13a690(*(word const *)(arg_2 + 0x1c));
			point2f forward;
			point2f up;
			point2f origin;
			short_rectangle2d clip;
			short_rectangle2d text_bounds;
			short_rectangle2d placed_bounds;

			function_1be50();
			function_1bd50(0);
			function_1bd50(&g_5021d0);
			unpack_color3f(*(dword const *)((byte const *)g_510c94 + 0xa4), (color3f *)&g_4e6950);
			function_1bbf0(*(long const *)(arg_2 + 0x24), 0, 1, 0, 0, 100.0f);
			function_1cf50();
			g_4e73a0.unknown5e = 0;
			g_4e73a0.unknown60 = 0;

			forward.x = 1.0f;
			forward.y = 0.0f;
			up.x = 0.0f;
			up.y = 1.0f;
			function_1396c7(*(word const *)(arg_2 + 0x1c), &origin);
			switch (local_2)
			{
			case 0:
				origin.x += *(short const *)(arg_2 + 0x40);
				origin.y += *(short const *)(arg_2 + 0x42);
				break;
			case 1:
				origin.x += *(short const *)(arg_2 + 0x44);
				origin.y += *(short const *)(arg_2 + 0x46);
				break;
			case 2:
				origin.x += *(short const *)(arg_2 + 0x48);
				origin.y += *(short const *)(arg_2 + 0x4a);
				break;
			}
			function_22bcaf((s_widget_transform_block const *)(arg_2 + 0x4c), &origin, &forward, &up);
			origin.x -= g_4b9dd2;
			origin.y -= g_4b9dd0;
			clip.left = 0;
			clip.top = 0;
			clip.right = 1000;
			clip.bottom = 1000;

			function_13e9c0(text, &clip, &text_bounds, &placed_bounds, 1.0f);
			long width = (short)(text_bounds.right - text_bounds.left);
			long height = (short)(text_bounds.bottom - text_bounds.top);
			function_13eb60(g_4686cc);
			function_13ed50((color4f const *)g_4686d4);
			switch (*(word const *)(arg_2 + 0x2c))
			{
			case 0:
				clip.left = (short)origin.x;
				clip.top = (short)origin.y;
				clip.right = (short)(origin.x + width);
				clip.bottom = (short)(origin.y + height);
				break;
			case 1:
				clip.left = (short)(origin.x - (width >> 1));
				clip.top = (short)origin.y;
				clip.right = (short)(origin.x + (width - (width >> 1)));
				clip.bottom = (short)(origin.y + height);
				break;
			case 2:
				clip.left = (short)(origin.x - width);
				clip.top = (short)origin.y;
				clip.right = (short)origin.x;
				clip.bottom = (short)(origin.y + height);
				break;
			}
			((c_1fa50 const *)text)->function_1fb10(&clip, 1.0f);
		}
	}
}
