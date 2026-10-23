// @flags /O2 /Gr
/* GAME_STATE_MODULES.CPP: the two passes over the game module table
   (g_46e320, unknown_03d380.cpp) around a load of the game state: entries
   5..9 before the game state is read or decompressed, entries 10..29 after
   it (unknown_067e10.cpp, unknown_123b30.cpp) (lane H) */

#include "unknown_11c920.h"
#include "globals.h"

/* calls the modules that prepare for a load of the game state */
// @retail 0x199520
void game_state_modules_call_before_load(dword flags)
{
	game_module_proc *proc = &g_46e320[5];
	long count;

	for (count = 5; count != 0; count--)
	{
		(*proc)(flags);
		proc++;
	}
}

/* calls the modules that follow up a load of the game state */
// @retail 0x199540
void game_state_modules_call_after_load(dword flags)
{
	game_module_proc *proc = &g_46e320[10];
	long count;

	for (count = 20; count != 0; count--)
	{
		(*proc)(flags);
		proc++;
	}
}
