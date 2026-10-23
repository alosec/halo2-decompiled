// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1388E0.CPP: finishing the game. game_finish belongs with the rest
   of the game code in game_loop.cpp, but it has to link after
   network_session_client.cpp (sources link in name order): in game_loop.cpp, 15
   network session functions (0x59360 to 0x942d0) take other register
   conventions under LTCG and stop matching. So it keeps this file name. */

#include "unknown_11c920.h"
#include "globals.h"
#include "game_loop.h"

/* game_loop.cpp */
void game_finish_countdown_start();

/* 0x19043f (unknown_190001.cpp): raises the profile's key of the kind to
   the value */
void function_19043f(long kind, long difficulty, long position);
void function_12b980(void);
void function_18e960(void);

static __forceinline bool game_loop_state_is(long state)
{
	return g_4e6948->state == state;
}

/* the profile key kind after this one, or NONE after the last (2) */
static __forceinline long game_finish_progress_kind_next(long kind)
{
	long next = NONE;
	if (kind >= 0 && kind < 3)
		next = kind + 1;
	return next;
}

/* finishes the game: a campaign raises the profile's progress keys to the
   difficulty and position reached; a multiplayer game starts the finish
   countdown */
// @retail 0x1388e0
void game_finish(void)
{
	s_game_options_view *options = g_4e6948;
	if (options && options->flag1120)
	{
		if (options->state == _game_state_campaign)
		{
			if (game_loop_state_is(_game_state_campaign) && options->position_b != NONE)
			{
				long kind = 0;
				do
				{
					function_19043f(kind, options->difficulty, options->position_b);
					kind = game_finish_progress_kind_next(kind);
				}
				while (kind != NONE);
				function_12b980();
			}
			function_18e960();
		}
		else if (options->state == _game_state_multiplayer)
		{
			game_finish_countdown_start();
		}
	}
}

