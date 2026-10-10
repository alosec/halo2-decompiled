#include "data_array.h"
#include "unknown_124f90.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered per-playback fade gain policy. */

struct s_4e6380
{
	byte unknown00[0x84];
	long time;
};

static_assert(sizeof(s_sound_playback) == 0xbc, "sound playback size");
static_assert(offsetof(s_sound_playback, fade_gain) == 0xb0,
	"sound fade gain offset");
static_assert(offsetof(s_sound_playback, fade_start_time) == 0xb4,
	"sound fade start offset");
static_assert(offsetof(s_sound_playback, fade_end_time) == 0xb8,
	"sound fade end offset");
static_assert(offsetof(s_4e6380, time) == 0x84,
	"sound system time offset");

extern s_record_pool *g_4e637c;
extern s_4e6380 *g_4e6380;
extern long g_440c48;
extern long g_440c4c;

real function_12aff0(real a, real b, real c, bool flag);
real function_2195f0(real decibels);
long function_2197f0(real gain);

static long decibels_pin(long decibels)
{
	if (*reinterpret_cast<real *>(&decibels) < -64.0f)
		return 0xc2800000;
	if (*reinterpret_cast<real *>(&decibels) > 0.0f)
		return 0;
	return decibels;
}

static s_sound_playback *sound_playback_get(long sound_index)
{
	return reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(sound_index & 0xffff);
}

// @retail 0x12a810
long function_12a810(long sound_index)
{
	s_sound_playback *sound = sound_playback_get(sound_index);
	long result = 0;

	if (sound->fading)
	{
		s_4e6380 *sound_system = g_4e6380;
		if (sound->fade_start_time == NONE || sound->fade_end_time == NONE)
		{
			sound->fade_start_time += sound_system->time + 1;
			sound->fade_end_time += sound_system->time + 1;
		}

		long const start = sound->fade_start_time;
		long const end = sound->fade_end_time;
		long const latest = start > end ? start : end;
		real const t = function_12aff0(static_cast<real>(start - latest),
			static_cast<real>(end - latest),
			static_cast<real>(sound_system->time - latest), start < end);
		long const lower = *(start < end ? &sound->fade_gain : &g_440c4c);
		long const upper = *(start < end ? &g_440c48 : &sound->fade_gain);
		real const lower_gain = function_2195f0(
			*reinterpret_cast<real const *>(&lower));
		real const upper_gain = function_2195f0(
			*reinterpret_cast<real const *>(&upper));

		if (sound->fade_curve == 0)
		{
			result = function_2197f0(
				(upper_gain - lower_gain) * t + lower_gain);
		}
		else if (sound->fade_curve == 1)
		{
			real const fraction = upper_gain > lower_gain
				? static_cast<real>(sqrt(t))
				: 1.0f - static_cast<real>(sqrt(1.0f - t));
			result = function_2197f0(
				(upper_gain - lower_gain) * fraction + lower_gain);
		}
		return decibels_pin(result);
	}

	return result;
}

#endif
