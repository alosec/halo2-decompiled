#ifndef GAME_ENGINE_POINTS_H
#define GAME_ENGINE_POINTS_H

/* GAME_ENGINE_POINTS.H: a player as game_engine_points.cpp reads it; the
   scores themselves are s_game_engine_scores in globals.h */

#include "unknown_11c920.h"
#include <stddef.h>

/* a player (0x21c bytes, the g_4e8c24 data) */
struct s_game_engine_score_player
{
	byte unknown00[0xc0];
	char team_index; /* -1 without a team */
	byte unknownc1[0x21c - 0xc1];
};
typedef char assert_game_engine_score_player_team[(offsetof(s_game_engine_score_player, team_index) == 0xc0) ? 1 : -1];
typedef char assert_game_engine_score_player_size[(sizeof(s_game_engine_score_player) == 0x21c) ? 1 : -1];

#endif
