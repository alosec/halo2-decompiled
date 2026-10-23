// @flags /O1 /Gr
/* SESSION_LOBBY_SCREEN.CPP: the network session's lobby screen (screen 0xe):
   the session's players, each local controller's team, the session's state
   and the countdown to the game. The rest of its methods are in
   unknown_250155.cpp. */

#include "session_lobby_screen.h"
#include "unknown_11c920.h"
#include <stdlib.h>
#include <string.h>

// @retail 0x24fa12
c_session_lobby_local_team::c_session_lobby_local_team()
{
	team = NONE;
	valid = false;
}

/* the lobby screen as unknown_250155.cpp passes it here */
struct s_session_screen_state
{
	byte unknown00[0x814];
	c_session_lobby_local_team teams[4];
};

/* whether any local controller has a team */
// @retail 0x24fa23
long session_lobby_screen_any_team_valid(s_session_screen_state const *screen)
{
	return screen->teams[0].valid || screen->teams[1].valid ||
		screen->teams[2].valid || screen->teams[3].valid;
}

c_class_1473c9 *__stdcall session_lobby_screen_load(s_screen_parameters *parameters);

// @retail 0x24fa1d
screen_load_proc c_session_lobby_screen::get_load_proc()
{
	return session_lobby_screen_load;
}

// @retail 0x24fa4c
c_class_1473c9 *__stdcall session_lobby_screen_load(s_screen_parameters *parameters)
{
	c_session_lobby_screen *screen = new c_session_lobby_screen(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x24fa88
c_session_lobby_screen::c_session_lobby_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xe, a, b, user_flags),
	button0(0, user_flags),
	button1(1, user_flags),
	value810(true),
	value811(true),
	value812(0),
	value813(false),
	countdown_stage(NONE),
	countdown_time(0),
	mode(0),
	settings_invalid(false),
	choice0(this, (session_choice_method)&c_session_lobby_screen::handle_lobby_choice),
	choice1(this, (session_choice_method)&c_session_lobby_screen::handle_lobby_choice),
	value1460(false),
	value1464(0),
	task_active(false),
	value146c(NONE),
	state_shown_time(0)
{
}

// @retail 0x24fba2 destructor c_session_lobby_screen

// @retail 0x24fb86 deleting c_session_lobby_screen

/* set elsewhere when a game was chosen before the lobby opened (see
   unknown_23068b.cpp) */
extern bool g_51ec98;
bool function_199971(void);
bool function_592f0(void);
long window_manager_get_game_variant_index(void);
bool function_199e6d(long index);
void function_19a0af(long value);
long player_slot_find_signed_in(long mode);
bool function_19a78e(long controller, long countdown, long minimum);
void function_236299(long sound);
bool function_2510e1(void);

/* on entry: when the session is ready (state 6) with a game chosen, applies
   the game variant and starts the countdown from three seconds (with a sound
   when it does not start); otherwise notes whether the session's settings
   are invalid */
// @retail 0x24fbea
void session_lobby_screen_enter(c_session_lobby_screen *screen)
{
	if (function_199971())
	{
		if (function_592f0() && g_51ec98 && function_199e6d(window_manager_get_game_variant_index()))
		{
			function_19a0af(window_manager_get_game_variant_index());
			if (!function_19a78e(player_slot_find_signed_in(0), 3, 3))
				function_236299(2);
		}
	}
	else if (!function_2510e1())
		screen->settings_invalid = true;
}

long function_19989d(void);

/* shows the pane that fits the session's state (function_19989d) */
// @retail 0x24ff0a
void session_lobby_screen_update_pane(c_session_lobby_screen *screen)
{
	short mode = 0;
	switch (function_19989d())
	{
	case 0: mode = 1; break;
	case 1: mode = 0; break;
	case 2: mode = 1; break;
	case 3: mode = 0; break;
	case 4: mode = 1; break;
	case 5: mode = 0; break;
	case 6: mode = 2; break;
	default: mode = screen->mode; break;
	}
	if (mode != screen->mode)
	{
		long pane = mode;
		screen->select_pane((short *)&pane);
		screen->mode = mode;
	}
}

struct s_session_player_view;
bool function_19a902(void);
long function_251364(s_session_player_view *player);

/* the help text: one pair of strings while the countdown runs, otherwise
   one pair for team games and one for the others; each pair by leads_party */
// @retail 0x24ff61
void session_lobby_screen_update_help_text(c_class_1a2c81 *screen, bool leads_party, s_session_player_view *session)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)screen->find_child(6, 1, false);
	if (text)
	{
		long string_handle;
		if (function_19a902())
			string_handle = leads_party ? 0x2700021b : 0x2000021a;
		else
		{
			if (leads_party)
				string_handle = (byte)function_251364(session) ? 0x25000217 : 0x26000216;
			else
				string_handle = (byte)function_251364(session) ? 0x1d000219 : 0x1e000218;
		}
		text->function_253b1a(string_handle);
	}
}

long function_1480ff(long screen_id);
bool function_13ee20(word const *string, long font);
void function_199b45(void);
byte *function_19aaa5(long player_index);

/* builds the screen from its definition: two lists whose choices go to
   handle_lobby_choice */
// @retail 0x24fc53
void c_session_lobby_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	c_class_1a2c81 *buttons[2] = { &button0, &button1 };
	s_screen_layout layout = { 0, 1, { { 2, buttons, 0, 0 } } };
	build(&layout);
	delegate_register(&button0.handlers, &choice0);
	delegate_register(&button1.handlers, &choice1);
	value1464 = 0;
	value146c = NONE;
	task_active = false;
	value1460 = false;
	c_class_1a2c81::v1();
	v7(&button0);
	if (!(*(byte *)parameters & 1))
		session_lobby_screen_enter(this);
	function_13ee20((word const *)L"0123456789", 4);
	g_51ec98 = false;
}

/* stops the task the screen started */
// @retail 0x24fd21
void c_session_lobby_screen::v2()
{
	if (task_active)
	{
		function_199b45();
		task_active = false;
	}
	c_class_1a2c81::v2();
}

/* sorts session players by team (a qsort comparator) */
// @retail 0x24fd41
int __cdecl session_lobby_player_compare_team(void const *left, void const *right)
{
	/* The comparator's first argument is stack-resident in retail. */
	void const *const *left_slot_ref = &left;
	s_session_lobby_player_properties *left_player = (s_session_lobby_player_properties *)function_19aaa5(*(long const *)*left_slot_ref);
	s_session_lobby_player_properties *right_player = (s_session_lobby_player_properties *)function_19aaa5(*(long const *)right);
	char left_team = left_player->team;
	char right_team = right_player->team;
	return left_team > right_team ? 1 : left_team < right_team ? -1 : 0;
}

long function_19a2ce(real *progress);
long function_19a50f(long state);
long network_session_interface_find_player_49b8();
bool function_19abe4(long player_index);
bool function_19a8ef();
bool function_199994();
byte function_199eaa();
long function_251139();
long function_63ec0();
long function_199ebc();
bool function_251188(long state);

/* the status text: names the session's state (function_19a2ce), with the
   host's states and the entry count spelled out */
// @retail 0x24ffc9
void session_lobby_screen_update_status_text(c_session_lobby_screen *screen, bool leads_party)
{
	c_session_lobby_screen *const *screen_reference = &screen;
	short text_index;
	switch ((*screen_reference)->mode)
	{
	case 0: text_index = 2; break;
	case 1: text_index = 2; break;
	case 2: text_index = 2; break;
	default: text_index = NONE; break;
	}
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)(*screen_reference)->find_text(text_index);
	if (text)
	{
		long state = function_19a2ce(0);
		long string_handle;
		switch (state)
		{
		case 10:
		{
			long player_index = network_session_interface_find_player_49b8();
			if (player_index != NONE)
				string_handle = function_19abe4(player_index) ? 0x1200020d : 0x1b00020c;
			else if (function_19a8ef())
				string_handle = function_199994() ? 0x1500015d : 0xa000208;
			else
				string_handle = leads_party ? 0x200001f0 : 0x2b00020b;
			break;
		}
		case 8:
			string_handle = function_199eaa() ? 0x170001ee : 0x100001ed;
			break;
		case 7:
			if (function_199994())
				string_handle = 0xe0001ec;
			else
			{
				switch (function_251139())
				{
				case 2: string_handle = 0xc0001e5; break;
				case 3: string_handle = 0xc0001e6; break;
				case 4: string_handle = 0xc0001e7; break;
				case 5: string_handle = 0xc0001e8; break;
				case 6: string_handle = 0xc0001e9; break;
				case 7: string_handle = 0xc0001ea; break;
				case 8: string_handle = 0xc0001eb; break;
				default: string_handle = 0xc0001e5; break;
				}
			}
			break;
		default:
			string_handle = function_19a50f(state);
			break;
		}
		if (state == 9)
		{
			long limit = function_63ec0();
			if (function_199ebc() > limit)
				string_handle = (*screen_reference)->get_definition_value(0, 3);
		}
		text->function_253b1a(string_handle);
		c_class_22cc8e *text_object = text->function_22f52e();
		if (function_251188(state))
			text_object->value14 |= 1;
		else
			text_object->value14 &= ~1;
	}
}

byte *network_session_interface_get_data_4db0();
word function_1901fc();
long player_slot_iterate_next(long player_index);
bool function_19a951(long player_index);
long function_19aa17(long player_index);
bool function_251121();
void function_250dae(c_session_lobby_screen *screen);
void function_250332(c_class_1a2c81 *screen);
void function_2503c0(c_class_1a2c81 *screen);
struct s_game_variant;
struct s_session_flags_view;
void function_2501c0(c_session_lobby_screen *screen, s_game_variant *variant);
void function_25042d(c_session_lobby_screen *screen, long valid, s_session_flags_view *session,
	long *players, long *local_list, long count);
void function_2507dc();

#pragma intrinsic(memset)

/* updates the screen each frame: the local teams, the session's players
   (sorted by team in team games), the countdown, and every pane's text */
// @retail 0x24fd74
void c_session_lobby_screen::v3()
{
	long session_state = function_19989d();
	s_session_lobby_session_view *session = (s_session_lobby_session_view *)network_session_interface_get_data_4db0();
	bool leads_party = function_592f0();
	bool host_state = function_199994();
	if (session && !session->kind)
		session = 0;
	if (settings_invalid && function_251121())
		settings_invalid = false;
	set_user_flags(function_1901fc());
	*(short *)&user_flags = NONE;
	if (!(byte)function_251364((s_session_player_view *)session))
	{
		for (long controller = 0; controller != NONE; controller = player_slot_iterate_next(controller))
		{
			teams[controller].valid = false;
			teams[controller].team = NONE;
		}
	}
	long controller_players[4];
	for (long controller = 0; controller != NONE; controller = player_slot_iterate_next(controller))
		controller_players[controller] = function_19aa17(controller);
	long player_count = 0;
	long players[16];
	memset(players, 0xff, sizeof(players));
	for (long player_index = 0; player_index < 16; ++player_index)
	{
		if (function_19a951(player_index))
			players[player_count++] = player_index;
	}
	if (!host_state && session && (session->flags & 1))
		qsort(players, player_count, sizeof(long), session_lobby_player_compare_team);
	function_250eb7();
	update_countdown(leads_party);
	if (session_state != NONE)
	{
		session_lobby_screen_update_pane(this);
		function_250dae(this);
		session_lobby_screen_update_help_text(this, leads_party, (s_session_player_view *)session);
		session_lobby_screen_update_status_text(this, leads_party);
		function_250155();
		if (host_state)
			function_2503c0(this);
		else
			function_2501c0(this, (s_game_variant *)session);
		function_250332(this);
		function_2508a8();
		function_25042d(this, session_state, (s_session_flags_view *)session, players, controller_players, player_count);
		function_2507dc();
		show_session_state();
	}
	function_250f3a((byte *)session);
	c_class_1a2c81::v3();
}
