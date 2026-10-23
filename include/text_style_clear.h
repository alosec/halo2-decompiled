#pragma once
/* Clearing the text style (g_4e73a0, src/unknown_13e8a0.cpp). Retail
   inlines it into 0x15e530 and calls the out-of-line copy (0x13e8a0)
   elsewhere. */

#include "globals.h"
#include <string.h>

void function_13ec70(color4f const *color);

/* clears the text style (font, tab stops, flags, justification) and selects the first global colour */
inline void text_style_clear()
{
	memset(&g_4e73a0, 0, sizeof(g_4e73a0));
	g_4e73a0.font = 0;
	g_4e73a0.tab_stop_count = 0;
	g_4e73a0.flags = 0;
	g_4e73a0.justification = 0;
	g_4e73a0.unknown5e = 0;
	g_4e73a0.unknown60 = 0;
	function_13ec70(g_4686cc);
}
