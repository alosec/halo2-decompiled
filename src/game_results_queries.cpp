#include "unknown_11c920.h"
#include "globals.h"
#include "input_record.h"

// @flags /O2 /Gr

/* GAME_RESULTS_QUERIES.CPP: the queries of the game results (the record that the
   "game-results" message carries, unknown_1967d0.cpp) that the postgame
   carnage report reads (unknown_232d43.cpp), and the two calls that store a
   player's and a team's score and standing while the game runs. A query
   answers only once the results are both valid (g_510cb0) and final
   (g_510cb1); a store happens only while they are kept (g_510ca0) and not
   yet final. The players' entries are reached through input_device() */

/* the teams' entries: active, standing (unknown01) and score (value) */
s_input_entry g_511a74[16];

/* a player's statistic of one category (the 45 blocks of 8 counters at
   +0x9c of each player's counters) */
// @retail 0x001969d0
long game_results_get_player_category_statistic(long player, long statistic, long category)
{
	long lookup_hit_value = -1;
	if (g_510cb0 && g_510cb1)
	{
		lookup_hit_value = g_511c90[player * 0x1b5 + statistic + category * 8].value;
	}
	return lookup_hit_value;
}

/* how many of one medal (0..31) a player earned */
// @retail 0x00196a10
long game_results_get_player_medal_count(long player, long medal)
{
	long medal_count = -1;
	if (g_510cb0 && g_510cb1)
	{
		medal_count = g_511c4e[player * 0x1b5 + medal].value;
	}
	return medal_count;
}

/* one of the two statistics a player holds against another player */
// @retail 0x00196a50
long game_results_get_player_versus_statistic(long player, long opponent, long statistic)
{
	long value = -1;
	if (g_510cb0 && g_510cb1)
	{
		value = g_515294[(player * 16 + opponent) * 2 + statistic].value;
	}
	return value;
}

/* the game type, which picks the statistics the report shows */
// @retail 0x00196a90
long game_results_get_game_type()
{
	long game_type = 0;
	if (g_510cb0 && g_510cb1)
	{
		game_type = g_510e2c;
	}
	return game_type;
}

// @retail 0x00196ab0
void game_results_set_player_score(long player, long score, short standing)
{
	if (g_510ca0 && !g_510cb1 && player != NONE && input_device(player)->active)
	{
		input_device(player)->axis = score;
		input_device(player)->button = standing;
	}
}

/* stores a team's score, clamped to a short, and standing */
// @retail 0x00196b00
void game_results_set_team_score(long team, long score, char standing)
{
	if (g_510ca0 && !g_510cb1 && team >= 0 && team < 8 && g_511a74[team].active)
	{
		if (score < -0x8000)
		{
			score = -0x8000;
		}
		else if (score > 0x7fff)
		{
			score = 0x7fff;
		}
		g_511a74[team].value = (word)score;
		g_511a74[team].unknown01 = standing;
	}
}

// @retail 0x00196b70
long game_results_get_player_score(long player)
{
	long score = 0;
	if (g_510cb0 && g_510cb1 && player != NONE && input_device(player)->active)
	{
		score = input_device(player)->axis;
	}
	return score;
}

// @retail 0x00196bb0
long game_results_get_team_score(long team)
{
	long score = 0;
	if (g_510cb0 && g_510cb1 && team >= 0 && team < 8 && g_511a74[team].active)
	{
		score = (short)g_511a74[team].value;
	}
	return score;
}

/* a player's standing; in a team game (g_510e30 bit 0) its team's */
// @retail 0x00196bf0
long game_results_get_player_standing(long player)
{
	long standing = -1;
	if (g_510cb0 && g_510cb1 && player != NONE && input_device(player)->active)
	{
		if (g_510e30 & 1)
		{
			long team = input_device(player)->slot;
			if (team == NONE)
			{
				return NONE;
			}
			if (team >= 0 && team < 16 && g_511a74[team].active)
			{
				standing = (char)g_511a74[team].unknown01;
			}
		}
		else
		{
			standing = input_device(player)->button;
		}
	}
	return standing;
}

// @retail 0x00196c60
long game_results_get_team_standing(long team)
{
	long standing = 0;
	if (g_510cb0 && g_510cb1 && team >= 0 && team < 8 && g_511a74[team].active)
	{
		standing = (char)g_511a74[team].unknown01;
	}
	return standing;
}

/* a player's name, a wide string (empty when there is none) */
// @retail 0x00196ca0
void *game_results_get_player_name(long player)
{
	void *name = (void *)L"";  /* the empty string at 0x450aac */
	if (g_510cb0 && g_510cb1 && player != NONE && input_device(player)->active)
	{
		name = input_device(player)->data;
	}
	return name;
}

/* a player's identifier (the s_id_triplet at +2 of its entry) */
// @retail 0x00196ce0
void *game_results_get_player_identifier(long player)
{
	void *identifier = 0;
	if (g_510cb0 && g_510cb1 && player != NONE && input_device(player)->active)
	{
		identifier = input_device(player)->unknown02;
	}
	return identifier;
}
