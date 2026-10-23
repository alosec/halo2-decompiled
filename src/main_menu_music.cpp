// @flags /O1 /arch:SSE /Gr
/* MAIN_MENU_MUSIC.CPP: the main menu's music: the screens that want it, the
   looping sound that plays it, and how long it has been silent */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "main_menu_music.h"
#include "ui_definition.h"
#include "unknown_19c1d0.h"

long function_147f4f();
long function_18a5a0(long tag_index, long object_index, real value);
word *function_18a5e0(long datum_index);
long map_location_progress_get(char const *map_name);
long function_122db0(long mode, char const *map_name);
s_entry_a *function_148d61();

/* cleared by the attract mode's idle timer (unknown_223976.cpp) a silence
   time before it starts, so the music stops in time */
extern bool g_4701b8;

/* the user interface time (the window manager's) */
extern dword g_54d5b8;

/* the music's looping sound tag, or NONE */
// @retail 0x235b46
long main_menu_music_tag_index()
{
	long result = NONE;
	s_type_954545 *ui_globals = ui_definition_get();

	if (ui_globals)
	{
		result = ui_globals->main_menu_music.tag_index;
	}
	return result;
}

/* how long the music stays silent (milliseconds) */
// @retail 0x235ca9
dword main_menu_music_silence_time()
{
	dword result = 0;
	s_type_954545 *ui_globals = ui_definition_get();

	if (ui_globals)
	{
		result = ui_globals->main_menu_music_silence_time;
	}
	return result;
}

/* starts the looping sound */
// @retail 0x235b5d
void main_menu_music_start(s_main_menu_music *music)
{
	long tag_index = main_menu_music_tag_index();

	if (tag_index != NONE)
	{
		music->sound_index = function_18a5a0(tag_index, NONE, 1.0f);
	}
}

/* stops the looping sound and notes when */
// @retail 0x235b82
void main_menu_music_stop(s_main_menu_music *music)
{
	if (music->sound_index != NONE)
	{
		function_18a5e0(music->sound_index);
		music->sound_index = NONE;
		music->stop_tick = GetTickCount();
	}
}

/* starts or stops the music as wanted, at most every six seconds */
// @retail 0x235b9c
void main_menu_music_update_playing(s_main_menu_music *music)
{
	if (music->playing != music->wanted)
	{
		dword time = g_54d5b8;

		if (!music->time || (long)(g_54d5b8 - music->time) > 6000)
		{
			if (music->wanted == 1)
			{
				main_menu_music_start(music);
			}
			else
			{
				main_menu_music_stop(music);
			}
			music->playing = music->wanted;
			music->time = time;
		}
	}
}

/* the campaign screen wants the music unless its map is still being copied
   and the copy will end within the silence time, or the map is open */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x235c4a
void main_menu_music_check_campaign(s_main_menu_music *music)
{
	s_entry_a *campaign_map = function_148d61();
	char const *map_name = 0;
	long progress = 3;

	if (campaign_map)
	{
		map_name = campaign_map->name;
		if (map_name)
		{
			progress = map_location_progress_get(map_name);
		}
	}
	switch (progress)
	{
	/* map location none, or copied (map_location_progress_get) */
	case 0:
	case 3:
		music->enabled = 1;
		break;
	/* queued or copying: the remaining copy time (milliseconds) */
	case 1:
	{
		long volatile silence_time = main_menu_music_silence_time();
		long remaining_time = function_122db0(0, map_name);
		long threshold = silence_time;
		_ReadWriteBarrier();
		if (remaining_time <= threshold)
		{
			music->enabled = 0;
		}
		break;
	}
	/* map location open */
	case 2:
		music->enabled = 0;
		break;
	}
}

/* whether the current screen wants the music: only in the main menu's
   scenario, on screens 6 and 9 (the ones the attract mode's idle timer
   watches) and the campaign screen (0xd1) */
// @retail 0x235bfa
void main_menu_music_check_screen(s_main_menu_music *music)
{
	s_scenario_shell_mode_view *scenario = (s_scenario_shell_mode_view *)g_4e0350;

	if (scenario && scenario->shell_mode == _shell_mode_main_menu)
	{
		switch (function_147f4f())
		{
		case 6:
		case 9:
			music->enabled = 1;
			return;
		case 13:
		case 14:
		case 0xba:
			break;
		case 0xd1:
			main_menu_music_check_campaign(music);
			return;
		default:
			return;
		}
	}
	music->enabled = 0;
}

/* each frame (window manager): what the screen wants, unless the attract
   mode is near, then start or stop */
// @retail 0x235bdb
void main_menu_music_update(s_main_menu_music *music)
{
	main_menu_music_check_screen(music);
	long wanted = music->enabled;

	if (!g_4701b8)
	{
		wanted = 0;
	}
	music->wanted = wanted;
	main_menu_music_update_playing(music);
}

/* whether the music has been silent for its silence time */
// @retail 0x235cbf
bool main_menu_music_silence_done(s_main_menu_music *music)
{
	if (!music->enabled && music->sound_index == NONE)
	{
		dword silence_time = main_menu_music_silence_time();

		return silence_time < GetTickCount() - music->stop_tick;
	}
	return false;
}

/* how far into its silence time the music is (0 to 1) */
// @retail 0x235ce6
real main_menu_music_silence_fraction(s_main_menu_music const *music)
{
	real result = 0.0f;

	if (!music->enabled && music->sound_index == NONE)
	{
		real silence_time = (real)(long)main_menu_music_silence_time();
		real elapsed = (real)(GetTickCount() - music->stop_tick);

		if (silence_time > 1.0f)
		{
			result = elapsed / silence_time;
			if (result < 0.0f)
			{
				result = 0.0f;
			}
			else if (result > 1.0f)
			{
				result = 1.0f;
			}
		}
	}
	return result;
}
