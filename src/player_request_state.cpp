// @flags /O2 /Gr
/* PLAYER_REQUEST_STATE.CPP: the lifecycle callbacks (entry 21 of g_440dd8)
   of g_51e9c0, the state the unit request code (unknown_1e77c0.cpp,
   unknown_1e7ab0.cpp) keeps for each of the four local players */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include <string.h>

/* a local player's state (0x1b0 bytes); unknown_1e77c0.cpp views it whole */
struct s_player_request_state
{
	byte unknown000[0x80];
	long unknown080;
	byte unknown084[0x190 - 0x84];
	long profile_index;              /* the profile its request flags are saved to */
	byte unknown194[0x1b0 - 0x194];
};
typedef char assert_player_request_state_size[(sizeof(s_player_request_state) == 0x1b0) ? 1 : -1];

/* the name other files declare g_51e9c0's type by */
struct s_unknown_1e6a40
{
	s_player_request_state signed_in_players[4];
	long unknown6c0;
	byte unknown6c4;
	byte unknown6c5;
	byte unknown6c6[2];
};
typedef char assert_unknown_1e6a40_size[(sizeof(s_unknown_1e6a40) == 0x6c8) ? 1 : -1];

s_unknown_1e6a40 *g_51e9c0;

/* allocates the state */
// @retail 0x1e6a40
void player_request_state_initialize(void)
{
	g_51e9c0 = (s_unknown_1e6a40 *)function_123d40("unknown", "unknown", sizeof(s_unknown_1e6a40));
}

/* clears the state for a new map: no player has a profile yet */
// @retail 0x1e6a80
void player_request_state_initialize_for_new_map(void)
{
	s_unknown_1e6a40 *state = g_51e9c0;

	memset(state, 0, sizeof(*state));
	state->unknown6c5 = 1;
	for (long i = 0; i < 4; i++)
	{
		state->signed_in_players[i].unknown080 = NONE;
		state->signed_in_players[i].profile_index = NONE;
	}
	state->unknown6c0 = 0;
	state->unknown6c4 = 0;
}

void __stdcall function_1e75d0(dword flush);

/* saves the players' changed request flags to their profiles */
// @retail 0x1e6ae0
void player_request_state_dispose_from_old_map(void)
{
	function_1e75d0(1);
}
