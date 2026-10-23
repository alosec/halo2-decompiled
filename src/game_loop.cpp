// @flags /O2 /arch:SSE /Gr
/* GAME_LOOP.CPP: the game's update (one frame, the ticks it runs, and one tick
   that updates every subsystem in turn), queries on the game options
   (g_4e6948), the revert and finish countdowns, the cluster masks the game
   keeps for the subsystems, and the d:\launch.txt script launch.
   game_finish (0x1388e0) is in unknown_1388e0.cpp: see there why */

#include "unknown_11c920.h"
#include "device_setup_storage.h"
#include "unknown_0259d0.h"
#include "main_globals.h"
#include "globals.h"
#include "unknown_067e10.h"
#include "unknown_440dd8.h"
#include "object_queries.h"
#include "game_loop.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

/* 0x138800 (game_loop_status.cpp): whether a game is in progress */
bool game_loop_running();

/* ---- the update ---- */

/* the seconds of the last frame the game updated */
real g_game_loop_frame_seconds;
extern double g_4ba040;
void function_17bc40(real seconds);
void function_17d690(void);
void __stdcall function_d4db0(long seconds_bits);
void __stdcall function_18c060(real seconds);
void function_2118c0(void);
void scenario_ramps_update(real seconds);
void function_14980(void);
bool function_128c0(s_128c0_settings const *settings);
bool function_14600(void);
void __stdcall function_39e50(real seconds);
void function_176f60(real seconds);

/* updates the game for a frame of the given seconds, and resets the device
   when its configuration changed */
// @retail 0x137ed0
void game_loop_frame_update(real seconds)
{
	if (seconds > 0.0f)
	{
		function_17bc40(seconds);
		function_17d690();
		function_d4db0(*(long *)&seconds);
		function_18c060(seconds);
		function_2118c0();
		scenario_ramps_update(seconds);
		if (memcmp(&g_runtime_device_configuration, &g_43ed80, sizeof(g_43ed80)) != 0)
		{
			function_14980();
			function_128c0(&g_43ed80);
			function_14600();
		}
		g_game_loop_frame_seconds = seconds;
		function_39e50(seconds);
	}
	function_176f60(seconds);
	g_4ba040 += seconds;
}

/* 0x12be90 (unknown_12be90.cpp): whether the main loop has an event pending
   that must stop the ticks */
bool function_12be90(void);
void game_loop_tick(void);

/* runs up to tick_count ticks; when an event stops them early, takes the
   ticks not run off the elapsed seconds */
// @retail 0x137f70
void game_loop_run_ticks(long tick_count, real *elapsed_seconds)
{
	long ticks_run = 0;
	while (ticks_run < tick_count)
	{
		if (function_12be90())
			break;
		game_loop_tick();
		ticks_run++;
	}
	if (ticks_run < tick_count)
	{
		if (ticks_run == 0)
		{
			*elapsed_seconds = 0.0f;
		}
		else
		{
			real seconds_left = *elapsed_seconds - (tick_count - ticks_run) * g_510c54->rate;
			*elapsed_seconds = seconds_left > 0.0f ? seconds_left : 0.0f;
		}
	}
}

struct s_unknown_78;
extern s_unknown_78 *g_510c6c;

void function_682e0(s_simulation_block_data *block);
void __stdcall function_683a0(byte *block);
void function_684e0(byte *block);
void __stdcall function_14d3a0(byte const *input);
void game_random_seed_stir(s_game_input_view const *input);
void function_1c83c0();
void function_1fb510(void);
void function_20b660(void);
void function_15c150();
void function_195ed0(void);
void hs_scheduler_update(void);
void game_clusters_update(void);
void function_b6f10();
void function_1c3d30();
void function_b7060();
void function_b7150();
void impacts_update(void);
void function_176e40(void);
void function_c1400();
void function_15ca10();
void function_14d8b0(void);
void function_1df500(void);
void game_revert_countdown_update();
void game_finish_countdown_update();
void function_226050(void);
void function_13c680(void);
void animation_cache_update(void);
void function_225730(void);
void function_165dc1(void);
void function_153950(void);
void function_1917b2(void);
void function_16f200(void);

static __forceinline void game_loop_tick_update(s_simulation_block_data *block)
{
	function_14d3a0((byte const *)block);
	game_random_seed_stir((s_game_input_view const *)block);
	function_1c83c0();
	function_1fb510();
	function_20b660();
	function_15c150();
	function_195ed0();
	hs_scheduler_update();
	game_clusters_update();
	function_b6f10();
	function_1c3d30();
	function_b7060();
	function_b7150();
	impacts_update();
	function_176e40();
	function_c1400();
	function_15ca10();
	function_14d8b0();
	function_1df500();
	game_revert_countdown_update();
	game_finish_countdown_update();
	function_226050();
	function_13c680();
	animation_cache_update();
	function_225730();
	function_165dc1();
	function_153950();
	function_1917b2();
	function_16f200();
	s_game_loop_tick_timer *timer = (s_game_loop_tick_timer *)g_510c6c;
	timer->ticks_left = timer->ticks_left - 1 < 0 ? 0 : timer->ticks_left - 1;
	timer->elapsed_seconds += g_510c54->rate;
}

static __forceinline bool game_loop_tick_end(s_simulation_block_data *block)
{
	function_684e0((byte *)block);
	return block->bytes[0] != 0;
}

/* one game tick: reads the next input block, then updates every subsystem
   in turn and advances the game time.
   "standard" stops LTCG assuming an 8-aligned entry: it gives retail's frame
   (0x4048) and late ebx/esi saves, and keeps 0x69350, 0x137f70 and 0xb7150
   matching */
// @retail 0x137fe0 standard
void game_loop_tick(void)
{
	s_simulation_block_data block;
#ifndef H2_NATIVE_PORT
	/* x87 control word: no equivalent on the ARM guest */
	_control87(0x9001f, 0xfffff);
#endif
	function_682e0(&block);
	function_683a0((byte *)&block);
	if (block.bytes[0])
		game_loop_tick_update(&block);
	if (game_loop_tick_end(&block))
		g_510c54->game_time++;
}

/* ---- the game options ---- */

/* never set by the decompiled code */
bool g_4f55e7;

void game_state_rollback_done();

/* whether a campaign is running with the options' flag at +0x134 */
// @retail 0x138820
bool function_138820()
{
	return g_4e6948->state == _game_state_campaign && g_4e6948->flag134;
}

/* whether the options' mode is 2 to 5 */
// @retail 0x138840
bool function_138840()
{
	bool result = false;
	long mode = g_4e6948->mode;
	if (mode >= 2 && mode <= 5)
	{
		result = true;
	}
	return result;
}

/* whether the options' mode is 4 */
// @retail 0x138880
bool function_138880()
{
	bool result = false;
	if (g_4e6948->mode == 4)
	{
		result = true;
	}
	return result;
}

/* whether the options' mode is not 4 */
// @retail 0x138860
bool function_138860()
{
	return !function_138880();
}

static inline bool game_mode_campaign_active()
{
	return g_4e6948->state == _game_state_campaign;
}

static inline short game_active_difficulty()
{
	short difficulty = 0;
	if (game_mode_campaign_active())
	{
		difficulty = g_4e6948->difficulty;
	}
	return difficulty;
}

PRIVATE __forceinline short game_options_difficulty(s_game_options_view const *options)
{
	return options->state == _game_state_campaign ? options->difficulty : 0;
}

/* false in a campaign while g_4f55e7 is set, or at difficulty 3 */
// @retail 0x1388a0
bool function_1388a0()
{
	s_game_options_view *options = g_4e6948;
	long state = options->state;
	return !(state == _game_state_campaign && g_4f55e7) && game_options_difficulty(options) != 3;
}

/* starts or stops the countdown that reverts the game to its saved state
   (main_globals.unknown6f, which the main loop's 12ba30 handles) */
// @retail 0x138960
void game_revert_countdown_set(bool start)
{
	if (start)
	{
		if (!g_4e6948->flag1121)
		{
			g_4e6948->flag1121 = true;
			real seconds = g_510c54->field_2_3 * k_game_revert_countdown_seconds;
			long ticks;
#ifdef H2_NATIVE_PORT
			ticks = (long)(seconds + 0.5f);
#else
			__asm
			{
				fld seconds
				fistp ticks
			}
#endif
			g_4e6948->revert_ticks_left = ticks;
			game_state_rollback_done();
		}
	}
	else if (g_4e6948->flag1121)
	{
		g_4e6948->flag1121 = false;
	}
}

/* starts the countdown that finishes the game */
// @retail 0x1389c0
void game_finish_countdown_start()
{
	s_game_options_view *options = g_4e6948;
	if (!options->flag1128)
	{
		options->flag1128 = true;
		real seconds = g_510c54->field_2_3 * k_game_finish_countdown_seconds;
		long ticks;
#ifdef H2_NATIVE_PORT
		ticks = (long)(seconds + 0.5f);
#else
		__asm
		{
			fld seconds
			fistp ticks
		}
#endif
		options->finish_ticks_left = ticks;
		options->finish_sound_faded = false;
	}
}

/* whether the finish countdown has run out */
// @retail 0x138a10
bool game_finish_countdown_done()
{
	return g_4e6948->flag1128 && g_4e6948->finish_ticks_left == 0;
}

/* 0x188cd0 (unknown_1887d0.cpp): fades the ambient sound classes out */
void function_188cd0(void);

/* counts the finish countdown down a tick, fading the sound out for its
   last two seconds */
// @retail 0x138eb0
void game_finish_countdown_update()
{
	s_game_options_view *options = g_4e6948;
	if (options->flag1128 && options->finish_ticks_left > 0)
	{
		options->finish_ticks_left--;
		if (!options->finish_sound_faded && k_game_finish_sound_fade_seconds >= options->finish_ticks_left * g_510c54->rate)
		{
			function_188cd0();
			g_4e6948->finish_sound_faded = true;
		}
	}
}

/* in a campaign, runs the revert countdown while the players globals ask
   for it and counts it down a tick; at zero, asks the main loop to revert */
// @retail 0x138e40
void game_revert_countdown_update()
{
	s_game_options_view *options = g_4e6948;
	if (options->state == _game_state_campaign && options->mode != 4)
	{
		bool start;
		if (options->flag134 && !function_1388a0())
		{
			start = ((s_game_players_view *)g_4e8c20)->revert_requested_flag134;
		}
		else
		{
			start = ((s_game_players_view *)g_4e8c20)->revert_requested;
		}
		game_revert_countdown_set(start);

		if (options->flag1121)
		{
			if (options->revert_ticks_left > 0)
			{
				options->revert_ticks_left--;
			}
			if (options->revert_ticks_left == 0)
			{
				main_globals.unknown6f = true;
			}
		}
	}
}

/* ---- the cluster the game focuses on ---- */

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
void function_11bed0(s_location *location, point3f const *point);

/* focuses the game on the cluster holding the scenario point, or on nothing */
// @retail 0x138a40
void game_focus_point_set(short point_index)
{
	if (point_index == NONE)
	{
		g_4e6948->value11fa = _game_focus_none;
		return;
	}

	s_location location;
	function_11bed0(&location, &((s_scenario_points_view *)g_4e0350)->points[point_index].position);
	if (location.cluster_index == NONE)
	{
		g_4e6948->value11fa = _game_focus_none;
	}
	else
	{
		g_4e6948->value11fa = _game_focus_cluster;
		g_4e6948->focus_cluster_index = location.cluster_index;
	}
}

/* the cluster the game focuses on: the focus object's or the focus
   cluster, or NONE */
// @retail 0x138ab0
short game_focus_cluster_get()
{
	short result = NONE;
	s_game_options_view *options = g_4e6948;
	switch (options->value11fa)
	{
	case _game_focus_object:
		if (function_badc0(options->value11fc, NONE))
		{
			long object_index = function_baf80(g_4e6948->value11fc);
			s_object_location_view *object = ((s_object_header_location_view *)g_4e0300->data)[object_index & 0xffff].object;
			if (TEST_FIELD_BIT(object->has_location) && object->location.cluster_index != NONE)
				result = object->location.cluster_index;
		}
		else
			options->value11fa = _game_focus_none;
		break;
	case _game_focus_cluster:
		result = options->focus_cluster_index;
		break;
	}
	return result;
}

/* stirs the game's random seed with the input's sticks */
// @retail 0x138db0
void game_random_seed_stir(s_game_input_view const *input)
{
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == _game_state_campaign)
	{
		dword *seed = &g_4e7408->unknown0;
		for (unsigned long i = 0; i < 16; i++)
		{
			if (input->controller_mask & (1 << i))
			{
				long count = 0;
				if (input->controllers[i].y > 0.0f)
				{
					count = 1;
				}
				else if (0.0f > input->controllers[i].y)
				{
					count = 2;
				}
				if (input->controllers[i].x > 0.0f)
				{
					count |= 4;
				}
				else
				{
					count |= 8;
				}
				for (; count > 0; count--)
				{
					_random(seed, __FILE__, __LINE__);
				}
			}
		}
	}
}
bool player_iterator_next(long *iterator);

/* with at most 8 players and the options' value at +0x1130 under 3, true
   for type 0, and for type 1 when that value is under 2 */
// @retail 0x138fa0
bool function_138fa0(long type)
{
	bool result = false;
	struct
	{
		byte *datum;
		s_record_pool *data;
		long datum_index;
		long index;
	} iterator;
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		count++;
	}

	if (count <= 8 && g_4e6948->value1130 < 3)
	{
		switch (type)
		{
		case 0:
			result = true;
			break;
		case 1:
			result = g_4e6948->value1130 < 2;
			break;
		default:
			__assume(0);
		}
	}

	return result;
}
long __stdcall hs_thread_for_script_name(char const *name);
void __stdcall hs_thread_wake(long index);

/* reads the script name on the third line of d:\launch.txt and wakes that
   script's thread */
// @retail 0x138f10
void game_launch_file_wake_script(void)
{
	FILE *file = fopen("d:\\launch.txt", "r");

	if (file)
	{
		char name[256];

		if (!fscanf(file, "%*s\n%*s\n") && fgets(name, 255, file))
		{
			char *end = strpbrk(name, "\r\n\t");
			long index;

			if (end)
			{
				*end = 0;
			}
			index = hs_thread_for_script_name(name);
			if (index != NONE)
			{
				hs_thread_wake(index);
			}
		}
		fclose(file);
	}
}

/* ---- the cluster masks ---- */

/* 0x150820 (unknown_1502e0.cpp): ors the clusters the players (or only the
   local players) see into the mask */
void function_150820(dword *bits, bool local_only);
/* 0x13e3e0 (bitset.cpp): destination = a | b, over bit_count bits */
void bitset_or(dword const *a, dword const *b, dword *destination, long bit_count);
long camera_scripting_cluster_get(void);

/* both cluster mask functions match only with inline_depth(0) (without it
   0x138b30 inlines its callees and grows from 224 to 257 bytes) */
#pragma inline_depth(0)
/* the clusters the players (or only the local players) see; on the main
   menu the clusters the first local player's view sees; plus those the
   scripted camera's cluster sees */
// @retail 0x138b30
void game_visible_clusters_get(dword *clusters, bool local_only, long unused)
{
	/* taking the address of the unused third argument keeps retail's
	   code; without it both functions differ */
	long const *unused_reference = &unused;
	s_game_cluster_bsp *bsp = (s_game_cluster_bsp *)g_4e0348;
	memset(clusters, 0, 0x40);
	s_game_options_view *options = g_4e6948;
	if (options->state != _game_loop_state_main_menu)
	{
		function_150820(clusters, local_only);
	}
	else
	{
		long local_index = NONE;
		for (long i = 0; i < 4; i++)
		{
			if (g_4e8c20->entries[i] != NONE)
			{
				local_index = i;
				break;
			}
		}
		s_game_local_player_location const *player = 0;
		if (local_index != NONE && g_4686c4 != NONE)
		{
			player = (s_game_local_player_location const *)&g_4e9bd4[local_index].state;
		}
		short cluster = player->location.cluster_index;
		if (cluster != NONE)
		{
			bitset_or(clusters,
				bsp->cluster_visibility + GAME_CLUSTER_MASK_DWORDS(bsp->cluster_count) * cluster, clusters, bsp->cluster_count);
		}
	}
	if (options->flag11f8 && *g_4e8c34)
	{
		short camera_cluster = (short)camera_scripting_cluster_get();
		if (camera_cluster != NONE)
		{
			bitset_or(clusters,
				bsp->cluster_visibility + GAME_CLUSTER_MASK_DWORDS(bsp->cluster_count) * camera_cluster, clusters, bsp->cluster_count);
		}
	}
}

/* 0x1c9b50 (ai.cpp): the actors' clusters */
bool function_1c9b50(dword *mask, short count);

/* recomputes the game options' cluster masks and tells each subsystem
   (g_440dd8) whose mask changed */
// @retail 0x138c10
void game_clusters_update(void)
{
	s_game_cluster_bsp *bsp = (s_game_cluster_bsp *)g_4e0348;
	dword old_visible[16];
	dword actor_clusters[16];
	dword old_active[16];
	memcpy(old_visible, g_4e6948->visible_clusters, sizeof(old_visible));
	memcpy(old_active, g_4e6948->active_clusters, sizeof(old_active));
	game_visible_clusters_get(g_4e6948->visible_clusters, false, 0);
	game_visible_clusters_get(g_4e6948->locally_visible_clusters, true, 0);
	if (function_1c9b50(actor_clusters, (short)bsp->cluster_count))
	{
		bitset_or(g_4e6948->visible_clusters, actor_clusters, g_4e6948->active_clusters, bsp->cluster_count);
	}
	else
	{
		memcpy(g_4e6948->active_clusters, g_4e6948->visible_clusters, GAME_CLUSTER_MASK_DWORDS(bsp->cluster_count) * sizeof(dword));
	}
	short cluster = game_focus_cluster_get();
	if (cluster != NONE)
	{
		dword *active = g_4e6948->active_clusters;
		bitset_or(active, bsp->cluster_visibility + GAME_CLUSTER_MASK_DWORDS(bsp->cluster_count) * cluster, active, bsp->cluster_count);
	}
	if (memcmp(old_visible, g_4e6948->visible_clusters, sizeof(old_visible)))
	{
		for (dword i = 0; i < 68; i++)
		{
			if (g_440dd8[i].visible_clusters_changed)
				g_440dd8[i].visible_clusters_changed(old_visible, g_4e6948->visible_clusters, bsp->cluster_count);
		}
	}
	if (memcmp(old_active, g_4e6948->active_clusters, sizeof(old_active)))
	{
		for (dword i = 0; i < 68; i++)
		{
			if (g_440dd8[i].active_clusters_changed)
				g_440dd8[i].active_clusters_changed(old_active, g_4e6948->active_clusters, bsp->cluster_count);
		}
	}
}
