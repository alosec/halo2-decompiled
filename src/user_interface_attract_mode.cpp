// @flags /O1 /Ob1 /arch:SSE /Gr
/* USER_INTERFACE_ATTRACT_MODE.CPP: the attract, intro and credits movies,
   and the main menu's countdown to the attract movie (named to link after
   unknown_*.cpp, which keeps 0x73360 matching) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "files.h"

s_type_acf665 *file_ref_create(s_type_acf665 *file, bool replace, const char *name);
bool function_1368f0(s_type_acf665 *file);
char *csprintf_256(char *buffer, const char *format, ...);
void function_156090(char const *name, dword flags);
bool function_155f60(void);

extern dword g_51ebec;
extern dword g_54d5b8;
dword g_54d5bc;

bool game_loop_running();
bool function_163870(void);
bool network_session_manager_session_unready(void);
long function_147f4f();
dword main_menu_music_silence_time(void);

/* whether the main menu may count down to the attract movie (main menu
   music reads it), and how long it waits (in milliseconds) */
bool g_4701b8 = true;
dword g_attract_mode_delay = 75000;

/* whether the attract movie should start: the main menu has been idle for
   long enough */
// @retail 0x223976
bool attract_movie_due(void)
{
	bool result = false;

	if (game_loop_running() && g_4e6948->state == 3 && function_163870() && !network_session_manager_session_unready() &&
		!function_155f60())
	{
		dword idle_time = g_54d5b8 - (g_51ebec > g_54d5bc ? g_51ebec : g_54d5bc);
		bool idle_screen = function_147f4f() == 9 || function_147f4f() == 6;
		dword delay = main_menu_music_silence_time();

		g_4701b8 = !(idle_time >= g_attract_mode_delay - delay && idle_screen);
		if (idle_time >= g_attract_mode_delay && idle_screen)
		{
			result = true;
		}
	}
	return result;
}

/* the path of a movie, which is looked up on the disc */
// @retail 0x223aa8
bool attract_mode_movie_path(char *path, char const *name)
{
	s_type_acf665 file;

	csprintf_256(path, "d:\\bink\\%s_%d.bik", name, 60);
	function_1368f0(file_ref_create(&file, false, path));
	return true;
}

/* the path of the attract (0), intro (1) or credits movie */
// @retail 0x223a7d
bool attract_mode_movie_path_by_type(long type, char *path)
{
	switch (type)
	{
	case 0:
		return attract_mode_movie_path(path, "attract");
	case 1:
		return attract_mode_movie_path(path, "intro");
	default:
		return attract_mode_movie_path(path, "credits");
	}
}

/* plays the attract movie */
// @retail 0x223a21
void attract_mode_play_movie(void)
{
	char path[256];
	long type = random_index(&g_4e7408->seed, 1);

	path[0] = 0;
	if (attract_mode_movie_path_by_type(type, path))
	{
		function_156090(path, 14);
	}
	if (!function_155f60())
	{
		g_51ebec = g_54d5b8;
	}
}
