#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Ob1 /Gr

/* The team helpers of the multiplayer globals (g_4e9ae8): the mask of the
   active teams at +0xc and the team table at +0xc18 (a count, then entries of
   16 bytes). Decompiled by lane O because the engine at 0x459d18
   (unknown_2420a0.cpp) calls them with register arguments. Retail never
   inlines them, hence /Ob1. */

struct s_team_entry
{
	long l0;
	short team;
	byte unknown06[0x10 - 0x06];
};

struct s_team_globals
{
	byte unknown00[0xc];
	word field_c_2;
	byte unknown0e[0xc14 - 0xe];
	long engine_index;
	long team_count;
	s_team_entry teams[1];
};

/* a copy of function_15eaf0 (unknown_15b2f0.cpp), which retail inlines here */
static inline bool game_has_teams()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = g_4e6948->flags184.bit0;

	return result;
}

// @retail 0x15e410
s_team_entry *function_15e410(short team)
{
	s_team_globals *g = (s_team_globals *)g_4e9ae8;
	s_team_entry *result = 0;

	if (g_55e4d0[g->engine_index])
	{
		for (long i = 0; i < g->team_count; i++)
		{
			if (g->teams[i].team == team)
			{
				result = &g->teams[i];
				break;
			}
		}
	}

	return result;
}

// @retail 0x161e10
bool function_161e10(long team)
{
	bool result = false;

	if (game_has_teams() && team >= 0 && team < 8)
		result = (((s_team_globals *)g_4e9ae8)->field_c_2 & (1 << team)) != 0;

	return result;
}

// @retail 0x161eb0
long function_161eb0(long team)
{
	long result = NONE;

	for (long i = 0; i < 7; i++)
	{
		team++;
		if (team == 8)
			team = 0;
		if (((s_team_globals *)g_4e9ae8)->field_c_2 & (1 << team))
		{
			result = team;
			break;
		}
	}

	return result;
}
