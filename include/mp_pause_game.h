#ifndef MP_PAUSE_GAME_H
#define MP_PAUSE_GAME_H
/* MP_PAUSE_GAME.H: the data the multiplayer pause screen
   (src/mp_pause_game.cpp) reads */

#include <stddef.h>
#include "unknown_11c920.h"

/* the network session's data (network_session_interface_get_data_4db0) as
   the pause menu reads it: the change teams item shows when flag6 and either
   flag0 or flag5 is set */
struct s_mp_pause_session_view
{
	byte unknown00[0x48];
	dword flag0 : 1;
	dword flags1 : 4;
	dword flag5 : 1;
	dword flag6 : 1;
};
typedef char assert_mp_pause_session_view_size[(sizeof(s_mp_pause_session_view) == 0x4c) ? 1 : -1];

/* a player slot (g_54e8e0, 0xc70 bytes each) as the pause screen reads it:
   the flag at +0x46d shows or hides the screen's bitmap 4 */
struct s_player_slot_pause_view
{
	byte unknown000[0x46d];
	bool bitmap_shown;
	byte unknown46e[0xc70 - 0x46e];
};
typedef char assert_player_slot_pause_view_size[(sizeof(s_player_slot_pause_view) == 0xc70) ? 1 : -1];
typedef char assert_player_slot_pause_view_bitmap_shown[(offsetof(s_player_slot_pause_view, bitmap_shown) == 0x46d) ? 1 : -1];

#endif
