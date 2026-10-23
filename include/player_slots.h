/* PLAYER_SLOTS.H: the four local player slots (g_54e8e0, 0xc70 bytes each,
   one per controller): the profile each slot is signed in with, its session
   user, online user, identity and online tasks (player_slots.cpp) */

#ifndef PLAYER_SLOTS_H
#define PLAYER_SLOTS_H

#include <xtl.h>
#include <xonline.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_18f576.h"
#include "unknown_24b5bc.h"

#define k_player_slot_count 4

/* the slot after this one: 0 after NONE, NONE after the last
   (player_slot_iterator.cpp) */
long player_slot_iterate_next(long index);

/* the profile a slot is signed in with (0x1e0 bytes; a saved
   s_player_profile_settings, copied here as raw dwords) */
struct s_player_profile
{
	dword data[0x78];
};
typedef char assert_player_profile_size[(sizeof(s_player_profile) == 0x1e0) ? 1 : -1];

/* the identity at +0x4e0 (0x6a2 bytes; valid when its xuid is not zero) */
#pragma pack(push, 2)
struct s_player_identity
{
	unsigned __int64 xuid;
	byte data[0x6a2 - 8];
};
#pragma pack(pop)
typedef char assert_player_identity_size[(sizeof(s_player_identity) == 0x6a2) ? 1 : -1];

/* the flags at +0 of a slot (the bits of s_player_slot_view::flags) */
enum e_player_slot_flags
{
	/* the slot holds a profile */
	_player_slot_flag_signed_in = 0x10,
	/* the profile changed during a game and is not saved yet */
	_player_slot_flag_profile_unsaved = 0x40
};

/* a slot of g_54e8e0 */
struct s_player_slot_view
{
	s_player_slot_view();

	union
	{
		dword flags;
		struct
		{
			dword : 4;
			dword signed_in : 1;
			dword live : 1;
			dword profile_unsaved : 1;
		};
	};
	/* the network session interface user the slot added (NONE for none) */
	long session_user_index;
	/* the xuid the session user was added with */
	XUID xuid;
	byte unknown14[4];
	s_player_profile profile;
	union
	{
		/* the profile's saved game file handle */
		long profile_index;
		struct
		{
			dword : 21;
			/* the handle's flag bit (bit 13 above its type byte) */
			dword profile_index_flag : 1;
		};
	};
	long team;
	byte unknown200[0x208 - 0x200];
	s_player_slot_profile sign_in_profile;
	byte unknown46c[0x470 - 0x46c];
	XONLINE_USER user;
	s_player_identity identity;
	s_player_slot_blockb82 online_status;
	/* the online tasks the slot started (NONE when idle) */
	long live_task;
	long live_task2;
	/* the display name function_190eb3 writes */
	word name[32];
	byte unknownc5c[0xc70 - 0xc5c];
};
typedef char assert_player_slot_view_size[(sizeof(s_player_slot_view) == 0xc70) ? 1 : -1];
typedef char assert_player_slot_view_profile[(offsetof(s_player_slot_view, profile) == 0x18) ? 1 : -1];
typedef char assert_player_slot_view_profile_index[(offsetof(s_player_slot_view, profile_index) == 0x1f8) ? 1 : -1];
typedef char assert_player_slot_view_team[(offsetof(s_player_slot_view, team) == 0x1fc) ? 1 : -1];
typedef char assert_player_slot_view_sign_in_profile[(offsetof(s_player_slot_view, sign_in_profile) == 0x208) ? 1 : -1];
typedef char assert_player_slot_view_user[(offsetof(s_player_slot_view, user) == 0x470) ? 1 : -1];
typedef char assert_player_slot_view_identity[(offsetof(s_player_slot_view, identity) == 0x4e0) ? 1 : -1];
typedef char assert_player_slot_view_online_status[(offsetof(s_player_slot_view, online_status) == 0xb82) ? 1 : -1];
typedef char assert_player_slot_view_live_task[(offsetof(s_player_slot_view, live_task) == 0xc14) ? 1 : -1];

#endif
