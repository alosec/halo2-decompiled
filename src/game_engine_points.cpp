// @flags /O2 /Gr
/* GAME_ENGINE_POINTS.CPP: a player's score in the multiplayer globals' score
   table: the team's score when the game is played by team. Moved here from
   unknown_19fc70.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "game_engine_points.h"

/* a player's score, or the player's team's in a team game; 0 without a game
   engine */
// @retail 0x19fc70
long game_engine_get_player_score(dword player_index)
{
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
	s_game_engine_scores *scores = 0;
	if (engine)
	{
		scores = &g_4e9ae8->scores;
	}
	long score = 0;

	if (scores)
	{
		s_game_engine_score_player *player = (s_game_engine_score_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_game_engine_score_player));
		bool uses_teams = false;
		if (engine)
		{
			uses_teams = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
		}
		if (uses_teams)
		{
			char team_index = player->team_index;
			if (team_index != -1)
			{
				score = scores->teams[team_index].score;
			}
		}
		else
		{
			score = scores->players[player_index & 0xffff].score;
		}
	}

	return score;
}
