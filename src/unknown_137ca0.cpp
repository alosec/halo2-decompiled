// @flags /O2 /Gr
/* UNKNOWN_137CA0.CPP: entering a game: the floating-point control word,
   the game options, then the subsystems' game-initialize callbacks */

#include "unknown_440dd8.h"
#include "globals.h"
#include <float.h>

struct s_game_options;

void function_137dd0(s_game_options const *options);

struct s_440de0
{
	void (*field_0)(void);
	dword field_4[8];
};

#define g_440de0 (*(s_440de0 const (*)[68])((byte const *)g_440dd8 + 8))

// @retail 0x137ca0
void __stdcall function_137ca0(s_game_options const *options)
{
	_control87(0x9001f, 0xfffff);
	g_4e6948->unknown00 = 1;
	function_137dd0(options);
	for (dword i = 0; i < 68; i++)
	{
		if (g_440de0[i].field_0)
			g_440de0[i].field_0();
	}
	g_4e6948->unknown00 = 0;
	g_4e6948->flag = 1;
}
