/* GAME_LOOP.H: the views of shared data that the game's update, its options
   queries, its countdowns and its cluster masks (src/game_loop.cpp) read */

#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "unknown_11c920.h"
#include "object_queries.h"
#include <stddef.h>

/* g_4e6948->state, as these functions test it. The state is the main menu
   when the window manager's main menu code checks it (147f6d) */
enum e_game_loop_state
{
	_game_state_campaign = 1,
	_game_state_multiplayer = 2,
	_game_loop_state_main_menu = 3
};

/* the kinds of thing the game focuses on (g_4e6948->value11fa); the scripts
   set _game_focus_object with an object index (unknown_29f5b0) */
enum e_game_focus
{
	_game_focus_none = 0,
	_game_focus_object,
	_game_focus_cluster
};

/* the countdowns, in seconds (times the ticks per second) */
#define k_game_revert_countdown_seconds 5.0f
#define k_game_finish_countdown_seconds 7.0f
#define k_game_finish_sound_fade_seconds 2.0f

/* the per tick timer at g_510c6c (unknown_1552e0.cpp, 0x78 bytes), as the
   tick reads it */
struct s_game_loop_tick_timer
{
	byte unknown00[4];
	real elapsed_seconds;
	byte unknown08[0x14 - 0x8];
	long ticks_left;
};
typedef char assert_game_loop_tick_timer_elapsed[(offsetof(s_game_loop_tick_timer, elapsed_seconds) == 0x4) ? 1 : -1];
typedef char assert_game_loop_tick_timer_ticks[(offsetof(s_game_loop_tick_timer, ticks_left) == 0x14) ? 1 : -1];
typedef char assert_game_loop_tick_timer_size[(sizeof(s_game_loop_tick_timer) == 0x18) ? 1 : -1];

/* the structure bsp (g_4e0348) as the cluster masks read it: for each of
   its cluster_count clusters, a row of (cluster_count + 31) / 32 dwords
   holding the clusters it sees */
struct s_game_cluster_bsp
{
	byte unknown00[0x58];
	dword *cluster_visibility;
	byte unknown5c[0x9c - 0x5c];
	long cluster_count;
};
typedef char assert_game_cluster_bsp_visibility[(offsetof(s_game_cluster_bsp, cluster_visibility) == 0x58) ? 1 : -1];
typedef char assert_game_cluster_bsp_size[(sizeof(s_game_cluster_bsp) == 0xa0) ? 1 : -1];

/* the dwords of one cluster's row in a cluster mask */
#define GAME_CLUSTER_MASK_DWORDS(cluster_count) (((cluster_count) + 31) >> 5)

/* the players globals (g_4e8c20) as the revert countdown reads them: two
   requests to run it, then the player index of each local player at +0xc */
struct s_game_players_view
{
	byte unknown00[4];
	bool revert_requested;
	bool revert_requested_flag134;
	byte unknown06[0xc - 0x6];
	long local_player_indices[4];
};
typedef char assert_game_players_view_requested[(offsetof(s_game_players_view, revert_requested) == 0x4) ? 1 : -1];
typedef char assert_game_players_view_indices[(offsetof(s_game_players_view, local_player_indices) == 0xc) ? 1 : -1];
typedef char assert_game_players_view_size[(sizeof(s_game_players_view) == 0x1c) ? 1 : -1];

/* a local player's view state (g_4e9bd4[i].state): its position, then where
   it is */
struct s_game_local_player_location
{
	point3f position;
	s_location location;
};
typedef char assert_game_local_player_location_cluster[(offsetof(s_game_local_player_location, location.cluster_index) == 0x10) ? 1 : -1];

/* one of the scenario's points (0x40 bytes) */
struct s_scenario_point
{
	byte unknown00[0x28];
	point3f position;
	byte unknown34[0xc];
};
typedef char assert_scenario_point_size[(sizeof(s_scenario_point) == 0x40) ? 1 : -1];

/* the scenario (g_4e0350) as the focus reads it */
struct s_scenario_points_view
{
	byte unknown00[0x1ec];
	s_scenario_point *points;
};
typedef char assert_scenario_points_view_points[(offsetof(s_scenario_points_view, points) == 0x1ec) ? 1 : -1];

/* an object, as far as its location: the flag at +5 says it has one */
struct s_object_location_view
{
	byte unknown00[4];
	dword : 8;
	dword has_location : 1;
	dword : 23;
	byte unknown08[0x20];
	s_location location;
};
typedef char assert_object_location_view_location[(offsetof(s_object_location_view, location) == 0x28) ? 1 : -1];

/* an object header (g_4e0300's 12 byte elements) */
struct s_object_header_location_view
{
	byte unknown00[8];
	s_object_location_view *object;
};
typedef char assert_object_header_location_view_size[(sizeof(s_object_header_location_view) == 0xc) ? 1 : -1];

/* one tick's input block (s_simulation_block_data) as the random seed stir
   reads it: a mask of the controllers present, then each controller's state
   (0x5c bytes) starting with its stick */
struct s_game_input_view
{
	byte unknown00[8];
	dword controller_mask;
	byte unknown0c[4];
	struct
	{
		real x;
		real y;
		byte unknown08[0x54];
	} controllers[16];
};
typedef char assert_game_input_view_size[(sizeof(s_game_input_view) == 0x10 + 16 * 0x5c) ? 1 : -1];

#endif
