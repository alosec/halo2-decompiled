#ifndef SESSION_LOBBY_SCREEN_H
#define SESSION_LOBBY_SCREEN_H
/* SESSION_LOBBY_SCREEN.H: the network session's lobby screen (screen 0xe,
   src/session_lobby_screen.cpp; most of its methods are in
   unknown_250155.cpp): the session's players, each local controller's team,
   the session's state and the countdown to the game */

#include <stddef.h>
#include "screen_widgets.h"

/* a local controller's team on the lobby screen */
class c_session_lobby_local_team
{
public:
	c_session_lobby_local_team();
	bool valid;
	byte unknown01;
	short team;
};
typedef char assert_session_lobby_local_team_size[(sizeof(c_session_lobby_local_team) == 4) ? 1 : -1];

typedef void (c_class_1473c9::*session_choice_method)(s_controller_reference **controller, long *item);

/* calls a screen's method when an item of one of its lists is chosen */
class c_session_choice_handler : public c_list_item_delegate
{
public:
	c_session_choice_handler(c_class_1473c9 *owner, session_choice_method method) :
		owner(owner), method(method) {}
	virtual void invoke(s_controller_reference **controller, long *item);
	c_class_1473c9 *owner;
	session_choice_method method;
};

/* the network session's data (network_session_interface_get_data_4db0) as
   the lobby screen reads it */
struct s_session_lobby_session_view
{
	byte unknown00[0x44];
	long kind;	/* 0 when the session holds no game: the screen ignores the data */
	byte flags;	/* bit 0: the game has teams */
};
typedef char assert_session_lobby_session_view_kind[(offsetof(s_session_lobby_session_view, kind) == 0x44) ? 1 : -1];
typedef char assert_session_lobby_session_view_flags[(offsetof(s_session_lobby_session_view, flags) == 0x48) ? 1 : -1];

/* a session player's properties (function_19aaa5, 0x90 bytes) as the
   lobby's player list sorts them */
struct s_session_lobby_player_properties
{
	byte unknown00[0x7c];
	char team;
	byte unknown7d[0x90 - 0x7d];
};
typedef char assert_session_lobby_player_properties_size[(sizeof(s_session_lobby_player_properties) == 0x90) ? 1 : -1];
typedef char assert_session_lobby_player_properties_team[(offsetof(s_session_lobby_player_properties, team) == 0x7c) ? 1 : -1];

/* the lobby screen (screen 0xe, 0x1474 bytes) */
class c_session_lobby_screen : public c_class_1473c9
{
public:
	c_session_lobby_screen(long a, long b, word user_flags);
	virtual screen_load_proc get_load_proc();
	virtual void v18(void *parameters);
	virtual void v2();
	virtual void v3();
	void function_250155();
	void function_250eb7();
	void function_250cda(long index, bool update);
	void change_team(long index, long delta);
	void show_session_state();
	void update_countdown(bool signed_in_needed);
	void function_250f3a(byte *data);
	void function_2508a8();
	void handle_lobby_choice(s_controller_reference **controller, long *item);
	virtual bool v10(s_widget_event *event);

	c_class_19b8b1 button0;
	c_class_19b8b1 button1;
	bool value810;
	bool value811;
	byte value812;
	bool value813;
	c_session_lobby_local_team teams[4];
	s_text_256 text[6];
	long countdown_stage;
	dword countdown_time;
	short mode;		/* the pane shown: 0 to 2, by the session's state */
	bool settings_invalid;	/* the session's settings were not valid on entry */
	c_session_choice_handler choice0;
	c_session_choice_handler choice1;
	bool value1460;
	long value1464;
	bool task_active;	/* function_199b45 stops the task on dispose */
	long value146c;
	dword state_shown_time;
};
typedef char assert_session_lobby_screen_size[(sizeof(c_session_lobby_screen) == 0x1474) ? 1 : -1];
typedef char assert_session_lobby_screen_teams[(offsetof(c_session_lobby_screen, teams) == 0x814) ? 1 : -1];

inline void c_session_choice_handler::invoke(s_controller_reference **controller, long *item)
{
	(owner->*method)(controller, item);
}

#endif
