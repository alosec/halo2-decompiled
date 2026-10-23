// @flags /O2 /Ob1 /Gr
/* MATCHMAKING_HOPPER.CPP: the checks of a loaded matchmaking hopper, one per
   hopper type (kept apart with /Ob1 because retail calls them out of line;
   the hopper's check that picks one is in unknown_1932c0.cpp), and the
   tables of start delays by player count that the built-in hoppers
   (unknown_1936e0.cpp) are filled with (lane H) */

#include "unknown_11c920.h"
#include "matchmaking_hopper.h"

/* checks what every hopper has: a known type, a map status, an identifier
   and a rating table in which each rating is matched at least with itself */
// @retail 0x193560
bool matchmaking_hopper_verify_common(s_matchmaking_hopper *hopper)
{
	bool valid;
	dword rating;

	valid = (hopper->type == _matchmaking_hopper_type_players_alone ||
		hopper->type == _matchmaking_hopper_type_players ||
		hopper->type == _matchmaking_hopper_type_teams_party_limits ||
		hopper->type == _matchmaking_hopper_type_teams ||
		hopper->type == _matchmaking_hopper_type_teams_party_of_team) &&
		hopper->map_status >= 0 && hopper->map_status < 4 && hopper->identifier > 0 && hopper->identifier <= 100;
	for (rating = 0; rating < k_matchmaking_hopper_rating_count; rating++)
	{
		valid = valid && hopper->highest_matched_rating[rating] >= (long)rating && hopper->highest_matched_rating[rating] <= 127;
	}
	return valid;
}

/* checks the player limits of a hopper of single players (types 1 and 2) */
// @retail 0x193610
byte matchmaking_hopper_verify_players(s_matchmaking_hopper *hopper)
{
	long minimum_count = hopper->players.minimum_count;
	if (minimum_count >= 1)
	{
		long maximum_count = hopper->players.maximum_count;
		if (maximum_count <= k_matchmaking_hopper_maximum_players && minimum_count <= maximum_count)
		{
			return true;
		}
	}
	return false;
}

/* checks the team, party and imbalance limits of a hopper of type 4 */
// @retail 0x193630
byte matchmaking_hopper_verify_teams_party_limits(s_matchmaking_hopper *hopper)
{
	long team_count = hopper->teams.count;
	if (team_count > 1 && team_count <= k_matchmaking_hopper_maximum_players)
	{
		long minimum_size = hopper->teams.minimum_size;
		if (minimum_size > 1)
		{
			long maximum_size = hopper->teams.maximum_size;
			if (maximum_size <= k_matchmaking_hopper_maximum_players / team_count && minimum_size <= maximum_size)
			{
				long minimum_party_size = hopper->teams.minimum_party_size;
				if (minimum_party_size > 0)
				{
					long maximum_party_size = hopper->teams.maximum_party_size;
					if (maximum_party_size <= maximum_size && minimum_party_size <= maximum_party_size)
					{
						long maximum_imbalance = hopper->teams.maximum_imbalance;
						if (maximum_imbalance >= 0 && maximum_imbalance <= maximum_size - minimum_size)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/* checks the team limits of a hopper of type 3 */
// @retail 0x1936a0
byte matchmaking_hopper_verify_teams(s_matchmaking_hopper *hopper)
{
	long team_count = hopper->teams.count;
	if (team_count > 1 && team_count <= k_matchmaking_hopper_maximum_players)
	{
		long minimum_size = hopper->teams.minimum_size;
		if (minimum_size >= 1)
		{
			long maximum_size = hopper->teams.maximum_size;
			if (maximum_size <= k_matchmaking_hopper_maximum_players / team_count && minimum_size <= maximum_size)
			{
				return true;
			}
		}
	}
	return false;
}


/* the check of type 5: the same code as type 3's, which the linker folded
   into it */
byte matchmaking_hopper_verify_teams_party_of_team(s_matchmaking_hopper *hopper)
{
	long team_count = hopper->teams.count;
	if (team_count > 1 && team_count <= k_matchmaking_hopper_maximum_players)
	{
		long minimum_size = hopper->teams.minimum_size;
		if (minimum_size >= 1)
		{
			long maximum_size = hopper->teams.maximum_size;
			if (maximum_size <= k_matchmaking_hopper_maximum_players / team_count && minimum_size <= maximum_size)
			{
				return true;
			}
		}
	}
	return false;
}


/* the start delay for each player count from 1 to 16 of a hopper of single
   players: none allowed outside [minimum, maximum], none to wait when full,
   and a share of the longest delay falling to zero at the maximum */
// @retail 0x1939f0
void matchmaking_hopper_build_player_start_delays(long longest_delay, long minimum_count, long maximum_count, long *start_delays)
{
	for (long player_count = 1; player_count <= k_matchmaking_hopper_maximum_players; player_count++)
	{
		long start_delay;

		if (player_count < minimum_count || player_count > maximum_count)
			start_delay = 0x7fffffff;
		else if (player_count == maximum_count)
			start_delay = 0;
		else
			start_delay = longest_delay * (maximum_count - player_count) / (maximum_count - minimum_count);
		start_delays[player_count - 1] = start_delay;
	}
}

/* the same for a hopper of teams, between the team count times the smallest
   and the largest team, falling with the players per team */
// @retail 0x193a50
void matchmaking_hopper_build_team_start_delays(long *start_delays, long team_count, long maximum_size, long minimum_size, long longest_delay)
{
	long maximum_count = team_count * maximum_size;
	long minimum_count = team_count * minimum_size;

	for (long player_count = 1; player_count <= k_matchmaking_hopper_maximum_players; player_count++)
	{
		long start_delay;

		if (player_count < minimum_count || player_count > maximum_count)
			start_delay = 0x7fffffff;
		else if (player_count == maximum_count)
			start_delay = 0;
		else
			start_delay = (maximum_size - player_count / team_count) * longest_delay / (maximum_size - minimum_size);
		start_delays[player_count - 1] = start_delay;
	}
}
