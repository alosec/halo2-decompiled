#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact scalar ownership of the recovered sound clock/fade update.  The
 * genuine busy-voice query remains an explicit dependency. */

struct s_4e6380
{
	byte unknown00[0x80];
	long last_tick;
	long time;
	byte unknown88[0x1e0 - 0x88];
	real elapsed_time;
	real master_fade;
	long master_fade_delay;
	struct
	{
		real i;
		real j;
		real k;
	} master_fade_times;
	long ambience_index;
	real ambience_fade;
	long previous_ambience_index;
	real previous_ambience_fade;
};

struct s_game_options_view
{
	byte unknown00[8];
	long state;
	byte unknown0c[0x1120 - 0x0c];
	bool flag1120;
	bool flag1121;
};

static_assert(offsetof(s_4e6380, last_tick) == 0x80,
	"sound last tick offset");
static_assert(offsetof(s_4e6380, time) == 0x84,
	"sound time offset");
static_assert(offsetof(s_4e6380, elapsed_time) == 0x1e0,
	"sound elapsed time offset");
static_assert(offsetof(s_4e6380, master_fade) == 0x1e4,
	"sound master fade offset");
static_assert(offsetof(s_4e6380, master_fade_delay) == 0x1e8,
	"sound master fade delay offset");
static_assert(offsetof(s_4e6380, master_fade_times) == 0x1ec,
	"sound master fade times offset");
static_assert(offsetof(s_4e6380, ambience_fade) == 0x1fc,
	"sound ambience fade offset");
static_assert(offsetof(s_4e6380, previous_ambience_fade) == 0x204,
	"sound previous ambience fade offset");
static_assert(offsetof(s_game_options_view, state) == 8,
	"game options state offset");
static_assert(offsetof(s_game_options_view, flag1120) == 0x1120,
	"game options active flag offset");
static_assert(offsetof(s_game_options_view, flag1121) == 0x1121,
	"game options pause flag offset");

extern s_4e6380 *g_4e6380;
extern s_game_options_view *g_4e6948;
extern "C" unsigned long __stdcall GetTickCount(void);
bool function_12be90(void);

static real pin_sound_fade(real value, real lower, real upper)
{
	return lower > value ? lower : (value > upper ? upper : value);
}

// @retail 0x1269f0
void sound_system_update_time(void)
{
	unsigned long const now = GetTickCount();
	s_4e6380 *const sound_system = g_4e6380;
	long const elapsed = static_cast<long>(
		now - static_cast<unsigned long>(sound_system->last_tick));

	sound_system->last_tick = static_cast<long>(now);
	sound_system->time = static_cast<long>(
		static_cast<unsigned long>(sound_system->time) +
		static_cast<unsigned long>(elapsed));
	sound_system->ambience_fade += static_cast<real>(elapsed) * 0.001f;
	sound_system->elapsed_time = static_cast<real>(elapsed) * 0.001f;
	sound_system->previous_ambience_fade +=
		static_cast<real>(elapsed) * 0.001f;
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
	{
		if (g_4e6948->flag1121)
		{
			if (sound_system->master_fade_delay == NONE)
				sound_system->master_fade_delay = static_cast<long>(
					sound_system->master_fade_times.i * 1000.0f);
			if (sound_system->master_fade_delay > 0)
			{
				long const remaining = sound_system->master_fade_delay - elapsed;
				sound_system->master_fade_delay = remaining > 0 ? remaining : 0;
			}
			if (sound_system->master_fade_delay == 0)
			{
				real const denominator =
					0.001f > sound_system->master_fade_times.j ?
					0.001f : sound_system->master_fade_times.j;
				real const step = sound_system->elapsed_time / denominator;
				sound_system->master_fade += pin_sound_fade(
					0.0f - sound_system->master_fade, 0.0f - step, step);
			}
		}
		else if (!function_12be90())
		{
			real const denominator =
				0.001f > sound_system->master_fade_times.k ?
				0.001f : sound_system->master_fade_times.k;
			real const step = sound_system->elapsed_time / denominator;
			sound_system->master_fade += pin_sound_fade(
				1.0f - sound_system->master_fade, 0.0f - step, step);
			sound_system->master_fade_delay = NONE;
		}
	}
	else
	{
		sound_system->master_fade = 1.0f;
	}
}

#endif
