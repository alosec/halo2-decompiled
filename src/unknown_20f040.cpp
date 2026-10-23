// @flags /O2 /Gr
/* UNKNOWN_20F040.CPP: which side of the player's team a team is on */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_20f040.h"

bool function_0bfe60(const dword *flags, long bit);
bool function_15e020(short a, short b);

struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;

struct s_20f040_allegiance_view
{
	byte unknown00[0xa4];
	dword ally_bits[8];
};

__declspec(noinline)
// @retail 0x20f040
long function_20f040(short team)
{
	if (team != 1)
	{
		if (team == NONE)
			return 1;

		long mode = g_4e6948->state;
		bool ally;

		if (mode == 1)
		{
			if (team < 0 || team >= 16)
				return 1;
			ally = function_0bfe60(((s_20f040_allegiance_view *)g_4f55ec)->ally_bits, team * 16 + 1);
		}
		else if (mode == 2)
		{
			ally = !function_15e020(team, 1);
		}
		else
		{
			return 1;
		}

		if (!ally)
			return 1;
	}

	return 0;
}
