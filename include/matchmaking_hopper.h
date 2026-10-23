/* MATCHMAKING_HOPPER.H: a matchmaking hopper, one of the 16 loaded at
   0x551ae8 from "multiplayer_hopper.dat" (the retail strings beside it name
   "multiplayer\matchmaking_hopper_descriptions"); the session's value at
   +0x49c8 is the index of its hopper, and the user interface's
   hopper_name and hopper_description tokens read its names and descriptions */

#ifndef MATCHMAKING_HOPPER_H
#define MATCHMAKING_HOPPER_H

#include "unknown_11c920.h"
#include <stddef.h>

enum
{
	k_matchmaking_hopper_count = 16,
	k_matchmaking_hopper_language_count = 9,
	k_matchmaking_hopper_rating_count = 128,
	k_matchmaking_hopper_maximum_players = 16,
};

/* what a hopper matches players into. The first two match single players,
   the other three teams; they differ in how big a party may join: */
enum e_matchmaking_hopper_type
{
	_matchmaking_hopper_type_players = 1,              // any party, up to the whole game
	_matchmaking_hopper_type_players_alone = 2,        // parties of one
	_matchmaking_hopper_type_teams = 3,                // any party, up to the whole game
	_matchmaking_hopper_type_teams_party_limits = 4,   // parties between the hopper's limits
	_matchmaking_hopper_type_teams_party_of_team = 5,  // parties up to a team
};

/* the limits of a hopper of single players (types 1 and 2) */
struct s_matchmaking_hopper_players
{
	long minimum_count;            // 0x5ec
	long maximum_count;            // 0x5f0
};

/* the limits of a hopper of teams (types 3 to 5) */
struct s_matchmaking_hopper_teams
{
	long count;                    // 0x5ec
	long minimum_size;             // 0x5f0
	long maximum_size;             // 0x5f4
	long maximum_imbalance;        // 0x5f8: of type 3 only its low byte, a flag
	long minimum_party_size;       // 0x5fc: type 4
	long maximum_party_size;       // 0x600: type 4
	bool party_team_limits;        // 0x604: type 4 has the next three
	byte unknown605[3];
	long party_team_minimum;       // 0x608
	long party_team_maximum;       // 0x60c
	long party_team_spread;        // 0x610
};

/* the tag keeps the name the other sources declare these functions with
   (unknown_1932c0.cpp and its callers); s_matchmaking_hopper is its name here */
struct s_surface_description
{
	long type;                     // e_matchmaking_hopper_type
	long map_status;               // 0..3 (game_variant_get_map_status)
	dword identifier;              // 1..100
	char names[k_matchmaking_hopper_language_count][0x10];
	char descriptions[k_matchmaking_hopper_language_count][0x80];
	/* for each rating, the highest rating it is matched with */
	char highest_matched_rating[k_matchmaking_hopper_rating_count];   // 0x51c
	long field_59c;
	long field_5a0;
	/* for each player count from 1, the milliseconds to wait before starting
	   with that many: 0x7fffffff outside the hopper's limits, 0 when full */
	long start_delays[k_matchmaking_hopper_maximum_players];          // 0x5a4
	long field_5e4;
	bool flag_5e8;
	byte unknown5e9[3];
	union
	{
		s_matchmaking_hopper_players players;
		s_matchmaking_hopper_teams teams;
	};                             // 0x5ec
};
typedef s_surface_description s_matchmaking_hopper;

typedef char assert_matchmaking_hopper_players_size[(sizeof(s_matchmaking_hopper_players) == 0x8) ? 1 : -1];
typedef char assert_matchmaking_hopper_teams_size[(sizeof(s_matchmaking_hopper_teams) == 0x28) ? 1 : -1];
typedef char assert_matchmaking_hopper_size[(sizeof(s_matchmaking_hopper) == 0x614) ? 1 : -1];
typedef char assert_matchmaking_hopper_ratings[(offsetof(s_matchmaking_hopper, highest_matched_rating) == 0x51c) ? 1 : -1];
typedef char assert_matchmaking_hopper_start_delays[(offsetof(s_matchmaking_hopper, start_delays) == 0x5a4) ? 1 : -1];
typedef char assert_matchmaking_hopper_limits[(offsetof(s_matchmaking_hopper, field_5e4) == 0x5e4) ? 1 : -1];

#endif
