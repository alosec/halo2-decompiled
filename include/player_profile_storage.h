#ifndef PLAYER_PROFILE_STORAGE_H
#define PLAYER_PROFILE_STORAGE_H

/* PLAYER_PROFILE_STORAGE.H: a player profile as player_profile_storage.cpp reads and writes
   it, the 0x1e0 bytes saved in a profile's saved game file */

#include "unknown_11c920.h"
#include <stddef.h>

/* the profile's appearance (0x10 bytes), which function_153750 checks */
struct s_player_profile_appearance
{
	char colors[4];          /* indices into the scenario's color table */
	char model;
	byte emblem_foreground;  /* 0 to 63 */
	byte emblem_background;  /* 0 to 31 */
	byte emblem_flags;
	byte unknown08[0x10 - 0x8];
};
typedef char assert_player_profile_appearance_size[(sizeof(s_player_profile_appearance) == 0x10) ? 1 : -1];

/* a player profile (0x1e0 bytes) */
struct s_player_profile
{
	byte unknown00[4];
	dword flags;             /* bit 0 set by the defaults, clear in a new profile */
	wchar_t name[32];
	byte unknown48[0xec - 0x48];
	long keys[4];            /* NONE when empty (player_profile_get_highest_key) */
	dword type_flags;        /* bit 0 set for default type 1 */
	byte value100;
	byte value101;
	byte value102;           /* 3 by default */
	byte unknown103[0x118 - 0x103];
	s_player_profile_appearance appearance;
	byte unknown128[0x151 - 0x128];
	byte value151;           /* 0 to 2; a larger value read from a file becomes 0 */
	byte unknown152[0x1e0 - 0x152];
};
typedef char assert_player_profile_name[(offsetof(s_player_profile, name) == 0x8) ? 1 : -1];
typedef char assert_player_profile_keys[(offsetof(s_player_profile, keys) == 0xec) ? 1 : -1];
typedef char assert_player_profile_type_flags[(offsetof(s_player_profile, type_flags) == 0xfc) ? 1 : -1];
typedef char assert_player_profile_appearance[(offsetof(s_player_profile, appearance) == 0x118) ? 1 : -1];
typedef char assert_player_profile_value151[(offsetof(s_player_profile, value151) == 0x151) ? 1 : -1];
typedef char assert_player_profile_size[(sizeof(s_player_profile) == 0x1e0) ? 1 : -1];

#endif
