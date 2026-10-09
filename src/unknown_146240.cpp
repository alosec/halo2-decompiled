// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_146240.CPP: the global random seed, random unit vectors and the game
time globals */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include <xtl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

vector3f g_4417f0[1026];

#ifdef H2_NATIVE_PORT
#define REAL_TO_LONG(x, result) ((result) = (long)((x) + 0.5f))
#else
#define REAL_TO_LONG(x, result) __asm { fld x } __asm { fistp result }
#endif

// @retail 0x146240
void random_initialize(void)
{
	s_random_globals *globals = (s_random_globals *)function_123d40("random", "random", sizeof(s_random_globals));

	g_4e7408 = globals;
	globals->unknown0 = 0x78a8;
	dword seed = (dword)time(0);
	seed = (dword)rand() ^ GetTickCount() ^ seed;
	g_4e7408->seed = seed;
}

// @retail 0x1462b0
dword function_1462b0(void)
{
	dword seed = (dword)time(0);
	seed = (dword)rand() ^ GetTickCount() ^ seed;
	return seed;
}

// @retail 0x1462e0
vector3f *random_unit_vector(vector3f *result, dword *seed)
{
	*seed = 1664525 * *seed + 1013904223;
	short index = (short)(((*seed >> 16) * 0x402) >> 16);
	*result = g_4417f0[index];
	return result;
}

// @retail 0x146320
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle)
{
	vector3f random_vector;
	vector3f axis;

	*result = *forward;

	*seed = 1664525 * *seed + 1013904223;
	random_vector = g_4417f0[(short)(((*seed >> 16) * 0x402) >> 16)];

	axis.i = random_vector.k * forward->j - random_vector.j * forward->k;
	axis.j = random_vector.i * forward->k - random_vector.k * forward->i;
	axis.k = random_vector.j * forward->i - random_vector.i * forward->j;

	real magnitude = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);
	if (fabs(magnitude) >= 0.0001f)
	{
		real scale = 1.0f / magnitude;
		axis.i *= scale;
		axis.j *= scale;
		axis.k *= scale;

		if (magnitude > 0.0001f)
		{
			real angle = function_259d0(seed, __FILE__, __LINE__, min_angle, max_angle);
			real s = (real)sin(angle);
			real c = (real)cos(angle);

			real dot = result->k * axis.k + result->j * axis.j + result->i * axis.i;
			real t = dot * (1.0f - c);
			real x = result->i;
			real y = result->j;
			real z = result->k;

			result->i = x * c + axis.i * t - (y * axis.k - z * axis.j) * s;
			result->j = y * c + axis.j * t - (z * axis.i - x * axis.k) * s;
			result->k = z * c + axis.k * t - (x * axis.j - y * axis.i) * s;
		}
	}
}

// @retail 0x146550
void game_time_globals_allocate(void)
{
	s_game_time_globals *globals = (s_game_time_globals *)function_123d40("game time", "game time", sizeof(s_game_time_globals));

	g_510c54 = globals;
	memset(globals, 0, sizeof(s_game_time_globals));
}

// @retail 0x1465b0
void game_time_globals_initialize(void)
{
	s_game_time_globals *globals = g_510c54;

	memset(globals, 0, sizeof(s_game_time_globals));
	short ticks = g_4e6948->field_2_3;
	globals->field_2_3 = ticks;
	globals->rate = 1.0f / (real)ticks;
	globals->scale = 1.0f;
	globals->active = true;
}

// @retail 0x146610
void game_time_stop(void)
{
	s_game_time_globals *globals = g_510c54;

	if (globals && globals->active)
	{
		globals->active = false;
	}
}

// @retail 0x146630
real game_time_get_seconds(void)
{
	s_game_time_globals *globals = g_510c54;

	if (globals && globals->active)
	{
		return (real)globals->game_time * globals->rate;
	}
	return 0.0f;
}

// @retail 0x146650
long function_146650(void)
{
	return g_510c54->game_time;
}

static inline long game_time_speed_to_ticks(s_game_options_view *options, real speed)
{
	real ticks = (real)options->field_2_3 / speed;
	long result;

	REAL_TO_LONG(ticks, result);
	return result;
}

/* the fields of the game options and the 0x510c50 state read here */
struct s_game_time_options_view
{
	byte unknown00[0xc];
	char mode;
	byte unknown0d[0x1120 - 0xd];
	bool field_1120;
};

struct s_510c50_time_view
{
	byte unknown00[5];
	bool unlimited;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

/* the number of ticks the network simulation allows (sets synchronous) */
long function_680c0(bool *synchronous);
PRIVATE void game_time_set_speed_internal(real speed);

/* advances the game time by a frame: the speed ramp, then the whole ticks
   due, the fraction carried to the next frame */
// @retail 0x146660
void function_146660(real seconds, real *elapsed, long *ticks_due)
{
	s_game_time_globals *globals = g_510c54;
	s_game_time_options_view *options = (s_game_time_options_view *)g_4e6948;
	long ticks = 0;
	real elapsed_seconds = 0.0f;
	real leftover = 0.0f;

	if (options && options->field_1120)
	{
		if (globals->speed_duration > 0.0f)
		{
			globals->speed_timer += seconds;
			if (globals->speed_duration > globals->speed_timer)
			{
				real t = globals->speed_timer / globals->speed_duration;

				game_time_set_speed_internal((1.0f - t) * globals->speed_initial + globals->speed_final * t);
			}
			else
			{
				game_time_set_speed_internal(globals->speed_final);
				globals->speed_duration = 0.0f;
			}
		}
		if (options->field_1120 && !globals->unknown01 && globals->scale > 0.0f)
		{
			bool synchronous = false;
			bool unlimited = false;
			long maximum_ticks;
			real ticks_elapsed;

			if (g_510c50 && ((s_510c50_time_view *)g_510c50)->unlimited)
			{
				unlimited = true;
			}
			if (options->mode >= 2 && options->mode <= 5)
			{
				maximum_ticks = function_680c0(&synchronous);
				globals = g_510c54;
			}
			else
			{
				maximum_ticks = globals->field_2_3 * 5;
			}
			if (!synchronous && !unlimited)
			{
				real scale = globals->scale;
				real limit;
				long limit_ticks;

				if (1.0f > scale)
				{
					scale = 1.0f;
				}
				else if (scale > 5.0f)
				{
					scale = 5.0f;
				}
				limit = scale * 2.0f;
				REAL_TO_LONG(limit, limit_ticks);
				if (maximum_ticks > limit_ticks)
				{
					maximum_ticks = limit_ticks;
				}
			}
			elapsed_seconds = globals->scale * seconds;
			ticks_elapsed = (real)globals->field_2_3 * elapsed_seconds + globals->leftover_ticks;
			ticks = (long)ticks_elapsed;
			if (synchronous ? (ticks <= maximum_ticks && ticks + 7 >= maximum_ticks && ticks + 1 == maximum_ticks) : ticks <= maximum_ticks)
			{
				leftover = ticks_elapsed - (real)ticks;
			}
			else
			{
				elapsed_seconds = (real)maximum_ticks * globals->rate;
				ticks = maximum_ticks;
			}
		}
	}
	globals->leftover_ticks = leftover;
	if (elapsed)
	{
		*elapsed = elapsed_seconds;
	}
	if (ticks_due)
	{
		*ticks_due = ticks;
	}
}

// @retail 0x146840
long function_146840(void)
{
	s_game_time_globals *globals = g_510c54;
#ifdef H2_NATIVE_PORT
	/* The static menu bootstrap runs before gameplay creates game-time state. */
	if (!globals)
		return false;
#endif

	if (globals->active && globals->unknown01)
	{
		return true;
	}
	return false;
}

PRIVATE void game_time_set_speed_internal(real speed);

// @retail 0x146860
void function_146860(real initial_speed, real speed, real duration)
{
	if (initial_speed < 0.2f)
	{
		initial_speed = 0.2f;
	}
	else if (initial_speed > 5.0f)
	{
		initial_speed = 5.0f;
	}

	if (speed < 0.2f)
	{
		speed = 0.2f;
	}
	else if (speed > 5.0f)
	{
		speed = 5.0f;
	}

	if (duration > 0.0f)
	{
		s_game_time_globals *globals = g_510c54;

		globals->speed_final = speed;
		globals->speed_timer = 0.0f;
		globals->speed_duration = duration;
		globals->speed_initial = initial_speed;
		speed = initial_speed;
	}
	game_time_set_speed_internal(speed);
}

// @retail 0x146980
PRIVATE void game_time_set_speed_internal(real speed)
{
	s_game_options_view *options = g_4e6948;

	if (!(speed > 0.01f))
	{
		speed = 0.01f;
	}

	long ticks = game_time_speed_to_ticks(options, speed);
	s_game_time_globals *globals = g_510c54;

	globals->rate = 1.0f / (real)ticks;
	globals->field_2_3 = (short)ticks;
	globals->scale = (real)options->field_2_3 / (real)ticks;
}
