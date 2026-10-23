#pragma once
/* UNKNOWN_24B5BC.H: the profile and Live user a player
   slot signs in with (unknown_24b5bc.cpp) */

#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "globals.h"

/* told whether a player slot signed in */
typedef void (__stdcall *player_sign_in_callback)(long player, bool signed_in);

/* the profile a player slot signs in with (0x264 bytes, at +0x208 of each
   player slot) */
struct s_player_slot_profile
{
	s_player_slot_profile();

	void initialize(long player);
	void set_profile_index(long profile_index);
	void set_profile(long profile_index);
	void show_dialog(player_sign_in_callback callback, long dialog_id);
	void sign_in(player_sign_in_callback callback);
	void sign_in_live();
	void sign_in_failed();
	void sign_out();
	void signed_out();
	void task_succeeded();
	void show_task_text(class c_online_task_screen *screen);

	long player;
	long profile_index;
	s_player_profile_settings settings;
	XONLINE_USER user;
	dword value258;
	player_sign_in_callback callback;
	/* the online task under way: 1 signs in, 2 signs out */
	long task_type;
};

/* a player slot as the sign in sees it */
struct s_player_slot_sign_in_view
{
	dword flags0 : 4;
	dword signed_in : 1;
	dword live : 1;
	dword : 26;
	byte unknown004[0x208 - 0x4];
	s_player_slot_profile profile;
	byte unknown46c[0xc70 - 0x46c];
};

/* a player slot's value at +4 (NONE when no player has the slot) */
struct s_player_slot_view_04
{
	byte unknown00[4];
	long value04;
	byte unknown08[0xc70 - 8];
};

inline s_player_slot_profile *player_slot_profile_get(long player)
{
	return &((s_player_slot_sign_in_view *)g_54e8e0)[player].profile;
}

/* the next controller after this one (NONE after the last) */
inline long next_controller_index(long index)
{
	long next = NONE;

	if (index >= 0 && index < 3)
	{
		next = index + 1;
	}
	return next;
}
