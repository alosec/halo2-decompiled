#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Native-selective exact ownership of the recovered main-thread sound idle
 * service.  The real sound clock, voice, driver, and physical-cache providers
 * remain explicit imports; this source does not replace any of them with a
 * yield or a no-op. */

struct s_4e6380
{
	byte unknown00[0x7c];
	bool unknown7c;
	bool changing_pause;
	byte unknown7e[2];
	long last_tick;
};

struct s_game_options_view
{
	byte unknown00[0x1120];
	bool flag1120;
};

struct s_game_time_globals
{
	byte unknown00[4];
	real rate;
};

static_assert(offsetof(s_4e6380, unknown7c) == 0x7c,
	"sound idle active offset");
static_assert(offsetof(s_4e6380, changing_pause) == 0x7d,
	"sound pause guard offset");
static_assert(offsetof(s_4e6380, last_tick) == 0x80,
	"sound last tick offset");
static_assert(offsetof(s_game_options_view, flag1120) == 0x1120,
	"game options active flag offset");
static_assert(offsetof(s_game_time_globals, rate) == 4,
	"game time rate offset");

extern s_4e6380 *g_4e6380;
extern s_game_options_view *g_4e6948;
extern s_game_time_globals *g_510c54;

extern "C" unsigned long __stdcall GetTickCount(void);

long sound_system_available(void);
void sound_system_update_time(void);
void sound_voices_update_chunks(void);
void function_1293f0(void);
void sound_cache_new_frame(void);
void function_21ec00(void);
void function_21ee80(void);

// @retail 0x125d60
void function_125d60(void)
{
	long elapsed = static_cast<long>(
		GetTickCount() - static_cast<unsigned long>(g_4e6380->last_tick));
	if (g_4e6948 && g_4e6948->flag1120 &&
		static_cast<real>(elapsed) > g_510c54->rate * 1000.0f)
	{
		s_4e6380 *sound_system = g_4e6380;
		sound_system->unknown7c = true;
		if (!sound_system->changing_pause)
		{
			sound_system->changing_pause = true;
			if (static_cast<byte>(sound_system_available()))
			{
				function_21ec00();
				sound_system_update_time();
				sound_voices_update_chunks();
				function_1293f0();
				function_21ee80();
				sound_system = g_4e6380;
			}
			sound_cache_new_frame();
			sound_system->changing_pause = false;
		}
		sound_system->unknown7c = false;
	}
}

#endif
