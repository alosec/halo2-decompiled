/* MAIN_MENU_MUSIC.H: the main menu's music (src/main_menu_music.cpp): which
   screens want it, the looping sound that plays it, and how long it has been
   silent. The window manager (g_54d598, +0x1248) holds its state. */

#ifndef MAIN_MENU_MUSIC_H
#define MAIN_MENU_MUSIC_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_234c64.h"

/* the user interface globals tag's music fields (s_type_954545) */
typedef char assert_ui_globals_main_menu_music[(offsetof(s_type_954545, main_menu_music.tag_index) == 0x1bc) ? 1 : -1];
typedef char assert_ui_globals_main_menu_music_silence_time[(offsetof(s_type_954545, main_menu_music_silence_time) == 0x1c0) ? 1 : -1];

/* the music's state (the window manager's, at +0x1248) */
struct s_main_menu_music
{
	/* whether it plays now, and whether it should */
	long playing;
	long wanted;
	/* whether the current screen wants it */
	long enabled;
	/* when it last started or stopped (user interface time, g_54d5b8) */
	dword time;
	/* its looping sound, or NONE */
	long sound_index;
	/* when it stopped (tick count) */
	dword stop_tick;
};
typedef char assert_main_menu_music_size[(sizeof(s_main_menu_music) == 0x18) ? 1 : -1];
typedef char assert_main_menu_music_sound_index[(offsetof(s_main_menu_music, sound_index) == 0x10) ? 1 : -1];

long main_menu_music_tag_index();
dword main_menu_music_silence_time();
void main_menu_music_start(s_main_menu_music *music);
void main_menu_music_stop(s_main_menu_music *music);
void main_menu_music_update_playing(s_main_menu_music *music);
void main_menu_music_check_campaign(s_main_menu_music *music);
void main_menu_music_check_screen(s_main_menu_music *music);
void main_menu_music_update(s_main_menu_music *music);
bool main_menu_music_silence_done(s_main_menu_music *music);
real main_menu_music_silence_fraction(s_main_menu_music const *music);

#endif
