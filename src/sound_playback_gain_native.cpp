#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_218ac0.h"
#include "unknown_221810.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered sound-definition and class/ambience gain policy. */

struct s_sound_definition_gain_view
{
	word flags;
	char promotion_index;
	byte unknown03[3];
	short class_index;
	short pitch_range_base;
	char pitch_range_count;
	char playback_index;
};

struct s_sound_playback_gain_view
{
	long gain_lower;
	long gain_upper;
	byte unknown08[0x0c];
};

struct s_sound_globals_playback_view
{
	byte unknown00[0x0c];
	s_sound_playback_gain_view *playback_parameters;
};

struct s_sound_class_ducking
{
	long gain;
	real fade_in_time;
	real hold_time;
	real fade_out_time;
};

struct s_sound_promotion_gain_view
{
	short definition_voice_limit;
	short source_voice_limit;
	long preemption_time;
	byte unknown08[4];
	short priority;
	byte unknown0e[0x0a];
	real minimum_distance;
	real maximum_distance;
	long gain_lower;
	long gain_upper;
	s_sound_class_ducking duckings[2];
	byte unknown48[0x14];
};

struct s_sound_class_fade
{
	dword target;
	dword current;
	real time;
	byte flags;
	byte unknown0d[3];
};

struct s_4e6380
{
	byte unknown00[0x1f8];
	long ambience_index;
	real ambience_fade;
	long previous_ambience_index;
	real previous_ambience_fade;
};

static_assert(sizeof(s_sound_definition_gain_view) == 0x0c,
	"sound definition gain view size");
static_assert(offsetof(s_sound_definition_gain_view, playback_index) == 0x0b,
	"sound definition playback index offset");
static_assert(sizeof(s_sound_playback_gain_view) == 0x14,
	"sound playback gain size");
static_assert(offsetof(s_sound_globals_playback_view, playback_parameters) == 0x0c,
	"sound playback table offset");
static_assert(sizeof(s_sound_class_ducking) == 0x10,
	"sound class ducking size");
static_assert(sizeof(s_sound_promotion_gain_view) == 0x5c,
	"sound promotion gain size");
static_assert(offsetof(s_sound_promotion_gain_view, gain_lower) == 0x20,
	"sound promotion lower gain offset");
static_assert(offsetof(s_sound_promotion_gain_view, duckings) == 0x28,
	"sound promotion ducking offset");
static_assert(sizeof(s_sound_class_fade) == 0x10,
	"sound class fade size");
static_assert(offsetof(s_4e6380, ambience_index) == 0x1f8,
	"sound ambience index offset");
static_assert(offsetof(s_4e6380, previous_ambience_fade) == 0x204,
	"sound previous ambience fade offset");

extern s_sound_class_fade *g_502118;
extern s_4e6380 *g_4e6380;
real function_2195f0(real decibels);
long function_2197f0(real gain);


static inline long decibels_interpolate(long a, long b, real t)
{
	real result = (*reinterpret_cast<real *>(&b) -
		*reinterpret_cast<real *>(&a)) * t +
		*reinterpret_cast<real *>(&a);
	return *reinterpret_cast<long *>(&result);
}

// @retail 0x218d70
__declspec(noinline) long sound_definition_gain_lower(s_sound_definition const *definition)
{
	s_sound_definition_gain_view const *view =
		reinterpret_cast<s_sound_definition_gain_view const *>(definition);
	s_sound_promotion_gain_view const *sound_class =
		reinterpret_cast<s_sound_promotion_gain_view const *>(
			sound_class_definition_get(view->promotion_index));
	s_sound_globals_playback_view *tables =
		reinterpret_cast<s_sound_globals_playback_view *>(g_51ebd4);
	return decibels_add(sound_class->gain_lower,
		tables->playback_parameters[view->playback_index].gain_lower);
}

// @retail 0x218de0
__declspec(noinline) long sound_definition_gain_upper(s_sound_definition const *definition)
{
	s_sound_definition_gain_view const *view =
		reinterpret_cast<s_sound_definition_gain_view const *>(definition);
	s_sound_promotion_gain_view const *sound_class =
		reinterpret_cast<s_sound_promotion_gain_view const *>(
			sound_class_definition_get(view->promotion_index));
	s_sound_globals_playback_view *tables =
		reinterpret_cast<s_sound_globals_playback_view *>(g_51ebd4);
	return decibels_add(sound_class->gain_upper,
		tables->playback_parameters[view->playback_index].gain_upper);
}

// @retail 0x127010
long function_127010(short class_index)
{
	s_sound_class_fade *fade = &g_502118[class_index];
	long gain = fade->current;
	if (!(fade->flags & 1))
	{
		s_sound_promotion_gain_view *sound_class =
			reinterpret_cast<s_sound_promotion_gain_view *>(
				function_221810(class_index));
		s_4e6380 *sound_system = g_4e6380;
		if (sound_system->previous_ambience_index != NONE &&
			sound_system->ambience_index ==
				sound_system->previous_ambience_index)
		{
			real elapsed = sound_system->ambience_fade -
				sound_system->previous_ambience_fade;
			s_sound_class_ducking *ducking =
				&sound_class->duckings[sound_system->ambience_index];
			if (elapsed > ducking->hold_time)
			{
				if (ducking->fade_in_time >= 0.001f)
				{
					real t;
					if (ducking->fade_out_time >= 0.001f &&
						ducking->fade_out_time + ducking->hold_time > elapsed)
					{
						real inverse = 1.0f / ducking->fade_in_time;
						real time = sound_system->ambience_fade - elapsed +
							(elapsed - ducking->hold_time) * inverse *
							ducking->fade_out_time;
						t = 1.0f > time * inverse ?
							time / ducking->fade_in_time : 1.0f;
					}
					else
					{
						real time = (sound_system->ambience_fade - elapsed) /
							ducking->fade_in_time;
						t = 1.0f > time ? time : 1.0f;
					}
					return decibels_add(gain,
						decibels_interpolate(0, ducking->gain, t));
				}
			}
			else
			{
				return decibels_add(ducking->gain, gain);
			}
		}
		else
		{
			if (sound_system->previous_ambience_index != NONE)
			{
				s_sound_class_ducking *ducking = &sound_class->duckings[
					sound_system->previous_ambience_index];
				if (ducking->fade_out_time >= 0.001f)
				{
					real time = sound_system->previous_ambience_fade -
						ducking->hold_time / ducking->fade_out_time;
					real t = 0.0f > time ? 0.0f :
						(time > 1.0f ? 1.0f : time);
					gain = decibels_add(
						decibels_interpolate(ducking->gain, 0, t), gain);
				}
			}
			if (sound_system->ambience_index != NONE)
			{
				s_sound_class_ducking *ducking =
					&sound_class->duckings[sound_system->ambience_index];
				if (ducking->fade_in_time >= 0.001f)
				{
					real time = sound_system->ambience_fade /
						ducking->fade_in_time;
					real t = 1.0f > time ? time : 1.0f;
					return decibels_add(
						decibels_interpolate(0, ducking->gain, t), gain);
				}
			}
		}
	}
	return gain;
}

// @retail 0x1251e0
long function_1251e0(void const *definition_pointer, long gain,
	real interpolation)
{
	s_sound_definition const *definition =
		reinterpret_cast<s_sound_definition const *>(definition_pointer);
	long upper_decibels = sound_definition_gain_upper(definition);
	long lower_decibels = sound_definition_gain_lower(definition);
	real lower = function_2195f0(*reinterpret_cast<real *>(&lower_decibels));
	real upper = function_2195f0(*reinterpret_cast<real *>(&upper_decibels));
	long decibels = function_2197f0((upper - lower) * interpolation + lower);
	long class_gain = function_127010(definition->promotion_index);
	return decibels_add(decibels, decibels_add(class_gain, gain));
}

#endif
