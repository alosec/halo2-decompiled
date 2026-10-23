#include "filter_storage.h"
// @flags /O1 /Oi /Gr
/* WINDOW_MANAGER_EVENTS.CPP: the window manager's queries of its channels
   and their dialogs, its handlers of the users' events (one per game mode,
   e_shell_mode), the screens it opens for them, the resets of its windows, the
   online tasks that read the selected player's clan, and the pending join
   (the rest of the window manager is in unknown_147f6d.cpp) */

#include "unknown_11c920.h"
#include "unknown_234c64.h"
#include "globals.h"
#include "unknown_19b516.h"
#include "online_tasks.h"
#include "unknown_19b510.h"
#include "ui_definition.h"
#include <xtl.h>
#include <xonline.h>

c_window_channel *function_148262(long channel, long index);
void function_23538b(c_window_channel *channel);

#define WINDOW_IN_USE(window) function_1473b6(window)

bool function_1900a5(long player);
bool __stdcall function_148e6d(long user_index);
bool game_loop_running();
long function_147f4f();
c_class_1473c9 *__stdcall online_y_menu_open(s_screen_parameters *request);
long function_199d7c(void);
long player_slot_find_signed_in(long mode);
void __stdcall function_238ea7(long user_index);
void __stdcall function_238eb5(long user_index, long type);


/* the screen ids of the dialogs: 7 and 8 are the choice and "ok" dialogs
   (unknown_19b510.cpp) */
enum
{
	k_choice_dialog_screen_id = 7,
	k_ok_dialog_screen_id = 8,
	k_dialog_screen_id_f0 = 0xf0
};
typedef char assert_dialog_screen_dialog_id[(offsetof(c_dialog_screen, dialog_id) == 0x610) ? 1 : -1];

/* whether a channel's window (only index 4 for the single windows) has a
   screen or one coming */
// @retail 0x1490ec
bool window_manager_channel_window_in_use(long channel, long index)
{
	bool channel_slot_busy = false;

	switch (channel)
	{
	case 0:
		channel_slot_busy = index == 4 && WINDOW_IN_USE(&g_54d598.window_0);
		break;
	case 1:
		channel_slot_busy = WINDOW_IN_USE(&g_54d598.windows_1[index]);
		break;
	case 2:
		channel_slot_busy = index == 4 && WINDOW_IN_USE(&g_54d598.window_2);
		break;
	case 3:
		channel_slot_busy = WINDOW_IN_USE(&g_54d598.windows_3[index]);
		break;
	case 4:
		channel_slot_busy = index == 4 && WINDOW_IN_USE(&g_54d598.window_4);
		break;
	case 5:
		channel_slot_busy = WINDOW_IN_USE(&g_54d598.windows_5[index]);
		break;
	default:
		channel_slot_busy = index == 4 && WINDOW_IN_USE(&g_54d598.default_window);
		break;
	}
	return channel_slot_busy;
}

/* whether any window of a channel has a screen or one coming */
// @retail 0x14917c
bool window_manager_channel_in_use(long channel)
{
	bool result = false;

	for (long index = 0; index < 5 && !result; index++)
	{
		switch (channel)
		{
		case 0:
			result = WINDOW_IN_USE(&g_54d598.window_0);
			index = 5;
			break;
		case 1:
			result = WINDOW_IN_USE(&g_54d598.windows_1[index]);
			break;
		case 2:
			result = WINDOW_IN_USE(&g_54d598.window_2);
			index = 5;
			break;
		case 3:
			result = WINDOW_IN_USE(&g_54d598.windows_3[index]);
			break;
		case 4:
			result = WINDOW_IN_USE(&g_54d598.window_4);
			index = 5;
			break;
		case 5:
			result = WINDOW_IN_USE(&g_54d598.windows_5[index]);
			break;
		case 6:
			result = WINDOW_IN_USE(&g_54d598.default_window);
			index = 5;
			break;
		}
	}
	return result;
}

/* whether any window of any channel has a screen or one coming */
// @retail 0x149227
bool window_manager_any_window_in_use(void)
{
	if (window_manager_channel_in_use(0) || window_manager_channel_in_use(1) || window_manager_channel_in_use(3) ||
		window_manager_channel_in_use(5) || window_manager_channel_in_use(6) || window_manager_channel_in_use(4) ||
		window_manager_channel_in_use(2))
	{
		return true;
	}
	return false;
}

/* whether a screen is a dialog */
// @retail 0x149278
bool screen_is_dialog(c_class_1473c9 *screen)
{
	bool result = false;

	if (screen)
	{
		long screen_id = screen->screen_id;

		if (screen_id >= k_choice_dialog_screen_id && (screen_id <= k_ok_dialog_screen_id || screen_id == k_dialog_screen_id_f0))
		{
			result = true;
		}
	}
	return result;
}

/* whether a window of a channel shows a dialog or has one coming */
// @retail 0x149296
bool window_manager_channel_has_dialog(long channel)
{
	bool result = false;

	for (long index = 0; index < 5; index++)
	{
		c_window_channel *window = function_148262(channel, index);

		if (window && (screen_is_dialog(window->current) || screen_is_dialog(window->next)))
		{
			result = true;
			break;
		}
	}
	return result;
}

/* whether a window shows the dialog of an id or has it coming */
// @retail 0x1492d6
bool window_manager_window_has_dialog(long channel, long index, long dialog_id)
{
	bool result = false;
	c_window_channel *window = function_148262(channel, index);

	if (window)
	{
		c_class_1473c9 *current = window->current;
		c_class_1473c9 *next = window->next;

		if (screen_is_dialog(current) && ((c_dialog_screen *)current)->dialog_id == dialog_id)
		{
			result = true;
		}
		if (screen_is_dialog(next) && ((c_dialog_screen *)next)->dialog_id == dialog_id)
		{
			result = true;
		}
	}
	return result;
}

/* whether channel 1's shared window has one of three dialogs (ids 0x3a,
   0x23 and 0x38) */
// @retail 0x149318
bool window_manager_has_listed_dialog(void)
{
	long dialog_ids[3] = { 0x3a, 0x23, 0x38 };
	bool result = false;

	for (dword i = 0; i < sizeof(dialog_ids) / sizeof(dialog_ids[0]); i++)
	{
		if (window_manager_window_has_dialog(1, 4, dialog_ids[i]))
		{
			result = true;
		}
	}
	return result;
}

/* opens a user's online menu (0x231995) in channel 4's window */
// @retail 0x1496c7
void window_manager_online_menu_open(long user_index)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 6, 0, 1 << user_index, 4, 4, (long)online_y_menu_open);
	parameters.load(&parameters);
}

/* the main menu's handler of a user's event: event 3 opens the user's
   online menu, when the user is signed in online and no other channel is
   busy */
// @retail 0x1494af
void window_manager_handle_main_menu_event(s_event const *event)
{
	if (event->type == 5 && event->param == 3 && function_6c7e0() && TEST_FIELD_BIT(g_54e8e0[event->unknown04].flag5) &&
		!function_1900a5(event->unknown04) && !window_manager_channel_in_use(4) && !window_manager_channel_in_use(2) &&
		!window_manager_channel_in_use(0) && !window_manager_channel_in_use(1) && !window_manager_channel_in_use(3) &&
		function_199d7c() != 6 && function_199d7c() != 7 && !g_4670cd)
	{
		window_manager_online_menu_open(event->unknown04);
	}
}

/* opens a screen in channel 3's shared window */
// @retail 0x149ef3
c_class_1473c9 *window_manager_open_channel_3_screen(word user_flags, long load)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, user_flags, 3, 4, load);
	return parameters.load(&parameters);
}

/* opens a screen in channel 5's shared window */
// @retail 0x149f1e
c_class_1473c9 *window_manager_open_channel_5_screen(short user_flags, long load)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, user_flags, 5, 4, load);
	return parameters.load(&parameters);
}

/* starts the pending join for the first signed in user, once */
// @retail 0x14a08f
void window_manager_send_pending_join(void)
{
	long join_type = g_54d598.pending_join_type;

	if (g_54d598.pending_join_type)
	{
		if (function_6c7e0())
		{
			long user_index = player_slot_find_signed_in(0);

			if (user_index != NONE)
			{
				if (join_type == 1)
				{
					function_238ea7(user_index);
				}
				else
				{
					if (join_type == 3)
					{
						function_238eb5(user_index, 2);
					}
					else
					{
						function_238eb5(user_index, 3);
					}
				}
			}
		}
		g_54d598.pending_join_type = 0;
	}
}

/* whether event 3 may open a user's online menu outside the main menu:
   the user is signed in online, only channel 5 is busy and the user's
   focused screen takes the user's input */
// @retail 0x14a0d7
bool window_manager_online_menu_can_open(long user_index)
{
	if (function_6c7e0() && TEST_FIELD_BIT(g_54e8e0[user_index].flag5) && !function_1900a5(user_index) &&
		!window_manager_channel_in_use(4) && !window_manager_channel_in_use(2) && !window_manager_channel_in_use(0) &&
		!window_manager_channel_in_use(1) && !window_manager_channel_in_use(3) && window_manager_channel_in_use(5) &&
		function_148e6d(user_index))
	{
		return true;
	}
	return false;
}

/* resets every window of every channel */
// @retail 0x14a152
void window_manager_reset_all_windows(void)
{
	c_window_channel *window = &g_54d598.default_window;

	window->dispose();
	for (long index = 0; index < 5; index++)
	{
		g_54d598.windows_5[index].reset();
		window = &g_54d598.windows_3[index];
		window->reset();
		window = &g_54d598.windows_1[index];
		window->reset();
		if (index == 4)
		{
			window = &g_54d598.window_0;
			window->dispose();
			window = &g_54d598.window_4;
			window->dispose();
			window = &g_54d598.window_2;
			window->dispose();
		}
	}
}

/* resets every window but those of channels 1 and 5 */
// @retail 0x14a1c3
void window_manager_reset_windows_except_channels_1_and_5(void)
{
	c_window_channel *window = &g_54d598.default_window;

	window->dispose();
	for (long index = 0; index < 5; index++)
	{
		g_54d598.windows_3[index].reset();
		if (index == 4)
		{
			window = &g_54d598.window_0;
			window->dispose();
			window = &g_54d598.window_4;
			window->dispose();
			window = &g_54d598.window_2;
			window->dispose();
		}
	}
}

/* starts animation 3 of channel 2's screen (0x23538b) */
// @retail 0x14935c
void window_manager_window_2_start_animation(void)
{
	function_23538b(&g_54d598.window_2);
}

/* whether the game is at the main menu: channel 5's focused screen is the
   main menu (6) or the "press start" screen (9) */
// @retail 0x14a224
bool window_manager_main_menu_focused(void)
{
	bool result = false;

	if (game_loop_running() && g_4e6948->state == 3)
	{
		long screen = function_147f4f();

		if (screen == 6 || screen == 9)
		{
			result = true;
		}
	}
	return result;
}

/* the window manager's clock, in milliseconds */
extern dword g_54d5b8;
/* the window manager's clock (+0x1264) when the main menu was last focused
   (set by unknown_147f6d.cpp) */
dword g_54e7fc;

/* whether the main menu has been left for 200 milliseconds */
// @retail 0x14a250
long window_manager_away_from_main_menu(void)
{
	if (!window_manager_main_menu_focused() && g_54d5b8 - g_54e7fc >= 200)
	{
		return true;
	}
	return false;
}

void online_teams_enumerate_get_results(long task_index, DWORD *count, XUID *teams);
void online_team_get_details(long task_index, XUID const *team, XONLINE_TEAM *details);
long online_team_members_enumerate(long controller_index, XUID const *team);
void online_team_members_enumerate_get_results(long task_index, DWORD *count, XUID *members);
void online_team_member_get_details(long task_index, XUID const *member_user_id, XONLINE_TEAM_MEMBER *member);

/* once the user's clans are enumerated, reads the first one's details and
   starts listing its members */
// @retail 0x1495aa
void window_manager_update_team_task(void)
{
	if (g_54d598.team_task != NONE)
	{
		long status = online_task_poll(g_54d598.team_task);
		bool finished = status != 0;

		if (status > 0 && status <= 2)
		{
			XUID teams[8];
			DWORD count = 8;

			online_teams_enumerate_get_results(g_54d598.team_task, &count, teams);
			if ((long)count > 0)
			{
				long controller_index = player_slot_find_signed_in(1);

				if (controller_index >= 0 && controller_index < 4)
				{
					online_team_get_details(g_54d598.team_task, &teams[0], (XONLINE_TEAM *)&g_54d598.team_details);
					g_54d598.team_members_task = online_team_members_enumerate(controller_index, &teams[0]);
				}
			}
		}

		if (finished)
		{
			function_6b640(g_54d598.team_task);
			g_54d598.team_task = NONE;
		}
	}
}

/* once the clan's members are listed, reads the details of the member the
   screen settings name */
// @retail 0x14963e
void window_manager_update_team_members_task(void)
{
	if (g_54d598.team_members_task != NONE)
	{
		long status = online_task_poll(g_54d598.team_members_task);
		bool finished = status != 0 && status != 1;

		switch (status)
		{
		case 2:
		{
			XUID const *xuid = NULL;
			XUID members[100];
			DWORD count;

			switch (g_54d598.settings.data[0])
			{
			case 1:
				xuid = (XUID const *)&g_54d598.settings.data[1];
				break;
			case 2:
				xuid = (XUID const *)&g_54d598.settings.data[1];
				break;
			}
			count = 100;
			online_team_members_enumerate_get_results(g_54d598.team_members_task, &count, members);
			online_team_member_get_details(g_54d598.team_members_task, xuid, (XONLINE_TEAM_MEMBER *)&g_54d598.team_member_details);
			break;
		}
		}

		if (finished)
		{
			function_6b640(g_54d598.team_members_task);
			g_54d598.team_members_task = NONE;
		}
	}
}

struct s_main_menu_music;
bool main_menu_music_silence_done(s_main_menu_music *music);

/* whether the main menu music has gone silent */
// @retail 0x14a217
bool window_manager_main_menu_music_silence_done(void)
{
	return main_menu_music_silence_done((s_main_menu_music *)&g_54d598.main_menu_music);
}

bool online_get_logon_status(long *status);
void function_1486b8(long error, bool keep);

/* reports the online logon status and, while signed in, updates the clan
   tasks */
// @retail 0x149570
void window_manager_update_online(void)
{
	long status;
	if (online_get_logon_status(&status))
	{
		bool connected = status == 0 || status == 1;
		function_1486b8(status, false);
		if (connected)
		{
			window_manager_update_team_task();
			window_manager_update_team_members_task();
		}
	}
}

bool function_147d13(void);
bool function_13cb40(void);
bool function_68290(void);
long function_1910b8(long controller);
void function_236299(long sound);
c_class_1473c9 *__stdcall function_2320c4(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall mp_pause_game_open(s_screen_parameters *parameters);

/* a player slot's player (NONE when the user has none) */
struct s_player_slot_player_view
{
	dword flags;
	long player_index;
};
typedef char assert_player_slot_player_view_size[(sizeof(s_player_slot_player_view) == 0x8) ? 1 : -1];

/* the campaign's handler of a user's event: event 12 opens the pause screen
   (0x2320c4) for a user with a player, event 3 the user's online menu */
// @retail 0x149366
void window_manager_handle_campaign_event(s_event const *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 12:
		{
			long user = event->unknown04;
			if (((s_player_slot_player_view *)&g_54e8e0[user])->player_index != NONE)
			{
				if (function_147d13())
				{
					function_236299(2);
				}
				else if (!function_13cb40())
				{
					window_manager_open_channel_5_screen(1 << user, (long)function_2320c4);
				}
			}
			break;
		}
		case 3:
			if (window_manager_online_menu_can_open(event->unknown04))
			{
				for (long i = 0; i < 5; i++)
				{
					c_window_channel *window = &g_54d598.windows_5[i];
					window->dispose();
				}
				window_manager_online_menu_open(event->unknown04);
			}
			break;
		}
	}
}

/* the multiplayer handler of a user's event: event 12 opens the multiplayer
   pause screen (0x1475c7) in the player's window, event 3 the user's online
   menu */
// @retail 0x1493ef
void window_manager_handle_multiplayer_event(s_event const *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 12:
		{
			long user = event->unknown04;
			if (((s_player_slot_player_view *)&g_54e8e0[user])->player_index != NONE)
			{
				if (function_147d13())
				{
					function_236299(2);
				}
				else if (!function_68290())
				{
					long player = function_1910b8(user);
					if (player >= 0 && player <= 3)
					{
						s_screen_parameters parameters;
						parameters.field_c = 0;
						function_149f49((s_message *)&parameters, 0, 0, 1 << user, 5, player, (long)mp_pause_game_open);
						parameters.load(&parameters);
					}
				}
			}
			break;
		}
		case 3:
			if (window_manager_online_menu_can_open(event->unknown04))
			{
				for (long i = 0; i < 5; i++)
				{
					c_window_channel *window = &g_54d598.windows_5[i];
					window->dispose();
				}
				window_manager_online_menu_open(event->unknown04);
			}
			break;
		}
	}
}

/* passes a user's event to the handler of the game mode */
// @retail 0x14954a
void window_manager_handle_event(s_event const *event)
{
	switch (g_54d598.shell_mode)
	{
	case _shell_mode_campaign:
		window_manager_handle_campaign_event(event);
		break;
	case _shell_mode_multiplayer:
		window_manager_handle_multiplayer_event(event);
		break;
	case _shell_mode_main_menu:
		window_manager_handle_main_menu_event(event);
		break;
	}
}
