/* UNKNOWN_218AC0.H: a sound tag's definition and the tables of the sound
   globals (g_51ebd4) it indexes: playback parameters, pitch ranges and their
   bounds, rate limits (src/unknown_218ac0.cpp, src/unknown_124f90.cpp) */
#ifndef UNKNOWN_218AC0_H
#define UNKNOWN_218AC0_H

#include "unknown_11c920.h"
#include "globals.h"

/* a sound tag's definition */
struct s_sound_definition
{
	word flags;
	char promotion_index;
	byte unknown03;
	byte type;
	byte format;
	short class_index;
	short pitch_range_base;
	char pitch_range_count;
	char playback_index;
	char rate_limit_index;
};

/* the playback parameters of a sound (0x14 bytes) */
struct s_sound_playback_parameters
{
	real gain_lower;
	real gain_upper;
	short pitch_lower;
	short pitch_upper;
	real skip_fraction_lower;
	real skip_fraction_upper;
};

/* the pitch bounds of a pitch range (10 bytes) */
struct s_sound_pitch_bounds
{
	short unknown00;
	short lower;
	short upper;
	short playback_lower;
	short playback_upper;
};

/* a pitch range (12 bytes) */
struct s_sound_pitch_range
{
	short unknown00;
	short bounds_index;
	byte unknown04[4];
	short first_permutation;
	short permutation_count;
};

/* a stage of a rate limit (16 bytes): how many starts it allows */
struct s_sound_rate_limit_stage
{
	short pitch_range_index;
	short count;
	real duration;
	long threshold;
	long increment;
};

/* a rate limit (0x1c bytes) */
struct s_sound_rate_limit
{
	long stage_count;
	s_sound_rate_limit_stage *stages;
	long counter_count;
	long *counters;
	long field_0;
	long field_14_2;
	long end_time;
};

/* the sound globals' tables, as the definition lookups read them */
struct s_sound_globals_definitions_view
{
	byte unknown00[0xc];
	s_sound_playback_parameters *playback_parameters;
	byte unknown10[0xc];
	s_sound_pitch_bounds *pitch_bounds;
	byte unknown20[4];
	s_sound_pitch_range *pitch_ranges;
	byte unknown28[0x24];
	s_sound_rate_limit *rate_limits;
};

#define SOUND_GLOBALS_DEFINITIONS ((s_sound_globals_definitions_view *)g_51ebd4)

static inline s_sound_definition *sound_definition_get(long definition_index)
{
	return (s_sound_definition *)g_4e3b44[definition_index & 0xffff].bytes;
}

static inline s_sound_rate_limit *sound_rate_limit_get(char index)
{
	if (index == NONE)
		return NULL;
	return &SOUND_GLOBALS_DEFINITIONS->rate_limits[index];
}

/* gains in decibels are kept as real bits in longs */
static inline long decibels_add(long a, long b)
{
	real result = *(real *)&a + *(real *)&b;

	return *(long *)&result;
}

#endif
