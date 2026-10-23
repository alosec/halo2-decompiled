/* UNKNOWN_2312B4.H: the online Y menu (vtable 0x458e58), the screen a
   press of Y opens on Xbox Live: three tabs (the friends, the players met and
   the recent players), each a screen with one list, and the user's pending
   online messages. The lists live in the screen_online_y_menu_*_list.cpp
   files. */

#ifndef UNKNOWN_2312B4_H
#define UNKNOWN_2312B4_H

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "online_message_entries.h"

/* the item sources of the friends and players lists are
   g_global_4acf62.field_4_4 and .field_8_2 */
#include "unknown_x8d43e5.h"

/* set while a friends or players list exists outside the main menu */
extern bool g_4e647b;

/* the base of the friends and players lists (vtable 0x45b440; its slots 18,
   19, 20 and 22 are pure in retail) */
class c_y_menu_list : public c_list_widget_with_items
{
public:
	c_y_menu_list(word user_flags);
	~c_y_menu_list();

	long value88;
};

/* the friends list (vtable 0x45b3e0), and the players list (vtable 0x45b510)
   with the same layout */
class c_y_menu_friends_list : public c_y_menu_list
{
public:
	c_y_menu_friends_list(word user_flags);

	virtual void v1();
	virtual void v3();
	virtual void v20(c_class_1a2c81 *item, long index);
	virtual void *get_item_data();
	virtual long get_item_count();
	virtual void *get_items(long *count);

	void handle_item(s_controller_reference **controller, long *item);

	/* the gamertag typed into the virtual keyboard */
	word name[0x10];
	c_class_14750b items[8];
	long item_count;
	/* filled in by the friends options screen */
	byte entries[100][0xc];
	c_list_item_handler handler;
};

class c_y_menu_players_list : public c_y_menu_list
{
public:
	c_y_menu_players_list(word user_flags);

	virtual void v1();
	virtual void v3();
	virtual void v20(c_class_1a2c81 *item, long index);
	/* slots 18, 19 and 22 are the friends list's (folded in retail) */

	void handle_item(s_controller_reference **controller, long *item);

	word name[0x10];
	c_class_14750b items[8];
	long item_count;
	byte entries[100][0xc];
	c_list_item_handler handler;
};

/* a recent player (0x58 bytes, copied from the player configuration cache):
   its XUID (the id, then its flags) and gamertag */
struct s_recent_player
{
	dword xuid[2];
	union
	{
		/* the player configuration cache reads these as bytes */
		byte unknown08[0x58 - 0x08];
		struct
		{
			dword xuid_flags;
			word name[16];
			byte unknown2c[0x56 - 0x2c];
			/* the value the player's list item takes its range for
			   (function_2b01a2) */
			signed char range_value;
			byte unknown57;
		};
	};
};
typedef char assert_recent_player_size[(sizeof(s_recent_player) == 0x58) ? 1 : -1];

/* the recent players list (vtable 0x45b4a0) */
class c_y_menu_recent_players_list : public c_class_1474e8
{
public:
	c_y_menu_recent_players_list(word user_flags);
	virtual void v20(c_class_1a2c81 *item, long index);

	/* slots 18 and 19 are folded with c_actions_list's and the friends
	   list's */

	void handle_item(s_controller_reference **controller, long *item);

	long value88;
	c_class_14750b items[8];
	c_list_item_handler handler;
};

/* the base of the three tabs (vtable 0x458d78; its slots 18 and 26 are pure in
   retail) */
class c_y_menu_tab_screen : public c_class_1473c9
{
public:
	c_y_menu_tab_screen(long a, long b, word user_flags);

	/* shows the user: gamertag, status and voice */
	virtual void v3();
	virtual void v18(void *parameters);

	bool shows_online_status;
	s_text_256 text;
};

/* the friends tab (vtable 0x458c98) */
class c_y_menu_friends_screen : public c_y_menu_tab_screen
{
public:
	c_y_menu_friends_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);

	long controller_index;
	c_y_menu_friends_list list;
};

/* the players tab (vtable 0x458ec8) */
class c_y_menu_players_screen : public c_y_menu_tab_screen
{
public:
	c_y_menu_players_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);

	long controller_index;
	c_y_menu_players_list list;
};

/* the recent players tab (vtable 0x458f38) */
class c_y_menu_recent_players_screen : public c_y_menu_tab_screen
{
public:
	c_y_menu_recent_players_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);

	long controller_index;
	c_y_menu_recent_players_list list;
};

/* the tab bar (vtable 0x45b570): slot 1 remembers its first child */
class c_y_menu_tab_bar : public c_class_1a2c81
{
public:
	c_y_menu_tab_bar(word user_flags);

	virtual void v1();
	virtual bool v10(s_widget_event *event);

	c_class_1a2c81 *focused;
};

/* the online Y menu (vtable 0x458e58) */
class c_online_y_menu_screen : public c_class_1473c9
{
public:
	c_online_y_menu_screen(long a, long b, word user_flags);
	~c_online_y_menu_screen();

	virtual void v2();
	virtual void v3();
	virtual void v17();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	/* whether the tab is the one that shows (out of line: 0x2b2d81,
	   0x2b3efc and 0x2b3923) */
	bool tab_is_current(c_class_1473c9 *tab)
	{
		return tab && tab == tab_bar.focused;
	}

	long controller_index;
	c_y_menu_tab_bar tab_bar;
	c_y_menu_friends_screen friends;
	c_y_menu_players_screen players;
	c_y_menu_recent_players_screen recent_players;
	byte unknown3664[4];
	/* the user's pending online messages */
	s_entry messages[0x7d];
	long message_count;
	bool strangers_messages_deleted;
};

#endif
