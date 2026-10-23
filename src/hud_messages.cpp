// @flags /O2 /arch:SSE /Gr
/* HUD_MESSAGES.CPP: the one-time hud messages a script shows by index */

#include "unknown_11c920.h"
#include "globals.h"

/* whether each message has been shown (unknown_29f5b0.cpp) */
extern bool g_4f55dc[16];

/* the message string of each index */
long const g_hud_message_string_ids[15] =
{
	0x100006f6, 0x100006f7, 0x100006f8, 0x100006f9, 0x100006fa, 0x100006fb, 0x100006fc, 0x100006fd,
	0x100006fe, 0x110006ff, 0x11000700, 0x11000701, 0x11000702, 0x11000703, 0
};

/* the sounds g_4e034c names */
struct s_hud_message_sounds
{
	byte unknown000[0x118];
	long message_sound_index;
};

struct s_hud_message_globals
{
	byte unknown000[0x134];
	s_hud_message_sounds *sounds;
};
typedef char assert_hud_message_globals_sounds[(sizeof(s_hud_message_globals) == 0x138) ? 1 : -1];

void __fastcall function_24cdaf(void);
long players_first_active_local_player(void);
void function_24cbbf(long player_index, long string_handle);
void function_154220(real x, short seconds, real y, real z);
long function_1896c0(real scale, long tag_index);

/* shows the message once: clears the scripted messages, shows the index's
   string to the first local player for 20 seconds, and plays the sound */
// @retail 0x1df3e0
void hud_message_show_once(long message_index)
{
	if (message_index >= 0 && message_index < 15 && !g_4f55dc[message_index])
	{
		long sound_index;

		g_4f55dc[message_index] = true;
		function_24cdaf();
		function_24cbbf(players_first_active_local_player(), g_hud_message_string_ids[message_index]);
		function_154220(1.0f, 20, 1.0f, 1.0f);
		sound_index = ((s_hud_message_globals *)g_4e034c)->sounds->message_sound_index;
		if (sound_index != NONE)
		{
			function_1896c0(1.0f, sound_index);
		}
	}
}
