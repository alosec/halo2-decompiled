// @flags /O1 /Gr
/* UNKNOWN_24C819.CPP: a caller optimized for size (/O1), which calls
function_123d40 rather than inlining it. */

#include "unknown_11c920.h"
#include "unknown_123b30.h"

/* the first-person HUD's state (unknown_24c7c1.cpp) */
struct s_hud_state;
extern s_hud_state *g_5023f4;

// @retail 0x24c819
void game_state_initialize_24c819(void)
{
	g_5023f4 = (s_hud_state *)function_123d40("unknown", "unknown", 0x13a8);
}
