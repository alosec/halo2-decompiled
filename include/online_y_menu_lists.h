/* ONLINE_Y_MENU_LISTS.H: the data the online Y menu's friends and recent
   players lists read (src/online_y_menu_lists.cpp and
   src/online_y_menu_recent_players_item.cpp) */

#ifndef ONLINE_Y_MENU_LISTS_H
#define ONLINE_Y_MENU_LISTS_H

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "unknown_2312b4.h"

/* the start of a friend of the friends list (s_friend in
   unknown_1a2ca7.cpp): its datum salt, then its id; the empty item has no
   id */
#pragma pack(push, 4)
struct s_friend_list_entry
{
	word salt;
	short unknown02;
	unsigned __int64 xuid;
};
#pragma pack(pop)
typedef char assert_friend_list_entry_xuid[(offsetof(s_friend_list_entry, xuid) == 4) ? 1 : -1];

/* a datum of the recent players list's data */
struct s_recent_player_datum
{
	word salt;
	word unknown02;
	s_recent_player player;
};
typedef char assert_recent_player_datum_size[(sizeof(s_recent_player_datum) == 0x5c) ? 1 : -1];

/* a player's id and gamertag (in ASCII), as a name request holds them */
struct s_player_name_request
{
	dword id[3];
	char name[16];
	byte unknown1c[0x70 - 0x1c];
};
typedef char assert_player_name_request_size[(sizeof(s_player_name_request) == 0x70) ? 1 : -1];

/* what function_148893 takes to select the player the online screens act
   on: a player (type 1) or a friend (type 2) */
#pragma pack(push, 4)
struct s_name_request
{
	long type;
	union
	{
		s_player_name_request player;
		XONLINE_FRIEND friend_;
		unsigned __int64 friend_xuid;
	};
	byte unknown74[4];
};
#pragma pack(pop)
typedef char assert_name_request_size[(sizeof(s_name_request) == 0x78) ? 1 : -1];
typedef char assert_name_request_friend[(offsetof(s_name_request, friend_) == 4) ? 1 : -1];

void __stdcall function_148893(s_name_request *request, long flag);

/* the online flags of the friend with this id (0 when not a friend) */
dword friend_get_flags(XUID const *xuid);

#endif
