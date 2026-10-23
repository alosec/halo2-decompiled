// @flags /O1 /arch:SSE /Gr
/* WINDOW_MANAGER_GAME_VARIANT.CPP: the game variant the window manager
   remembers at +0x1224: function_148cfc (unknown_147f6d.cpp) sets it to the
   index of the variant block a hosted game is started with
   (game_variant_block_read) */

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "unknown_234c64.h"

/* the chosen game variant's index, or NONE before one is chosen */
// @retail 0x148d30
long window_manager_get_game_variant_index()
{
	long result = NONE;

	if (g_54d598.m1220)
	{
		result = g_54d598.m1224;
	}
	return result;
}
