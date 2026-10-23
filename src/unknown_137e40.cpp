// @flags /O2 /Gr
/* UNKNOWN_137E40.CPP: setting up the players from the game options: the
   machines, the local machine, each active player's creation record, then
   the simulation watcher */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_067e10.h"

struct s_player_creation_record;

/* a player's creation record as this file sees it (0xe4 bytes) */
struct s_player_record_137e40
{
	bool active;
	byte unknown01[0xe4 - 1];
};

/* the game options (at +8 of the game globals) as this file sees them */
struct s_game_options_137e40
{
	byte unknown000[0x268];
	dword machine_valid_mask;
	s_machine_address machines[16];
	bool has_local_machine;
	s_machine_address local_machine;
	byte unknown2d3;
	s_player_record_137e40 players[16];
};

struct s_game_globals_137e40
{
	byte unknown00[8];
	s_game_options_137e40 options;
};

void function_14c630(dword valid_mask, s_machine_address const *addresses);
void players_set_local_machine(s_machine_address const *machine_address);
long function_14bc00(long player_index, s_player_creation_record const *record);
void simulation_watcher_rebuild_players(s_simulation_world_owner *watcher);
void function_1e73b0();

/* unknown_14b560.cpp */
extern bool g_4ed39c;
/* unknown_03d380.cpp */
extern byte g_4cf77b;

#define GAME_GLOBALS ((s_game_globals_137e40 *)g_4e6948)

// @retail 0x137e40
void function_137e40(void)
{
	if (g_4ed39c)
		return;
	s_game_options_137e40 *options = &GAME_GLOBALS->options;
	function_14c630(options->machine_valid_mask, options->machines);
	if (options->has_local_machine)
		players_set_local_machine(&options->local_machine);
	else
		players_set_local_machine(0);
	for (long i = 0; i < 16; i++)
	{
		if (options->players[i].active)
			function_14bc00(i, (s_player_creation_record const *)&options->players[i]);
	}
	if (g_4cf770 && !g_4cf77b)
		simulation_watcher_rebuild_players((s_simulation_world_owner *)g_4cf780);
	function_1e73b0();
}
