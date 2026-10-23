// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_218C60.CPP: sound definition queries: a permutation's sample
   count, a sound's maximum distance, and its gain bounds (unknown_218ac0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_221810.h"

/* a sound tag's definition, as these functions read it */
struct s_sound_definition_view
{
	word flags;
	char promotion_index;
	byte unknown03[3];
	short class_index;
	short first_pitch_range_index;
	char pitch_range_count;
	char playback_index;
};

struct s_sound_pitch_range_view
{
	byte unknown00[8];
	short first_permutation_index;
	short permutation_count;
};

struct s_sound_permutation_view
{
	byte unknown00[6];
	word sample_count;
	byte unknown08[8];
};

struct s_sound_globals_view
{
	byte unknown00[4];
	struct
	{
		real minimum_distance;
		real maximum_distance;
		byte unknown08[0x38 - 8];
	} *classes;
	byte unknown08[0x24 - 8];
	s_sound_pitch_range_view *pitch_ranges;
	byte unknown28[4];
	s_sound_permutation_view *permutations;
};

/* the playback parameters of a sound (0x14 bytes), as decibels held as
   real bits */
struct s_sound_playback_gain_view
{
	long gain_lower;
	long gain_upper;
	byte unknown08[0xc];
};

struct s_sound_globals_playback_view
{
	byte unknown00[0xc];
	s_sound_playback_gain_view *playback_parameters;
};

/* a promotion's distances (function_221810) */
struct s_sound_promotion_view
{
	byte unknown00[0x18];
	real minimum_distance;
	real maximum_distance;
	long gain_lower;
	long gain_upper;
};


static inline s_sound_definition_view *sound_definition_get(long definition_index)
{
	return (s_sound_definition_view *)g_4e3b44[definition_index & 0xffff].bytes;
}

// @retail 0x218c60
real function_218c60(long definition_index, long pitch_range_index, long permutation_index)
{
	s_sound_definition_view *definition = sound_definition_get(definition_index);
	s_sound_pitch_range_view *arg_58ecd0 = &((s_sound_globals_view *)g_51ebd4)->pitch_ranges[definition->first_pitch_range_index + pitch_range_index];

	return (real)((s_sound_globals_view *)g_51ebd4)->permutations[arg_58ecd0->first_permutation_index + permutation_index].sample_count;
}

#pragma inline_depth(0)
// @retail 0x218d30
real function_218d30(long definition_index)
{
	s_sound_definition_view *definition = (s_sound_definition_view *)g_4e3b44[definition_index & 0xffff].bytes;
	if (definition->flags & 0x800)
	{
		return ((s_sound_promotion_view *)function_221810(definition->promotion_index))->maximum_distance;
	}
	return ((s_sound_globals_view *)g_51ebd4)->classes[definition->class_index].maximum_distance;
}
#pragma inline_depth(255)

struct s_sound_definition;

/* a gain in decibels, copied as a whole */
struct s_decibels_view
{
	real value;
};

/* the sum of two gains in decibels, held as real bits */
static inline long decibels_add(long a, long b)
{
	real result = *(real *)&a + *(real *)&b;

	return *(long *)&result;
}

/* the lower bound of a sound's gain in decibels: its class's and its
   playback parameters'. The class's gain is copied as a whole (in a
   register) and stored only once the playback gain is loaded */
// @retail 0x218d70
long sound_definition_gain_lower(s_sound_definition const *definition)
{
	s_sound_definition_view const *view = (s_sound_definition_view const *)definition;
	s_decibels_view local_0;
	long local_1;
	s_decibels_view local_2 = *(s_decibels_view const *)&((s_sound_promotion_view *)sound_class_definition_get(view->promotion_index))->gain_lower;
	local_1 = ((s_sound_playback_gain_view *)((s_sound_globals_playback_view *)g_51ebd4)->playback_parameters)[view->playback_index].gain_lower;
	real const *local_3 = (real const *)&local_1;
	local_0 = local_2;
	local_0.value = *local_3 + local_0.value;
	return *(long *)&local_0;
}

/* the upper bound of a sound's gain in decibels */
// @retail 0x218de0
long sound_definition_gain_upper(s_sound_definition const *definition)
{
	s_sound_definition_view const *view = (s_sound_definition_view const *)definition;
	s_decibels_view local_0;
	long local_1;
	s_decibels_view local_2 = *(s_decibels_view const *)&((s_sound_promotion_view *)sound_class_definition_get(view->promotion_index))->gain_upper;
	local_1 = ((s_sound_playback_gain_view *)((s_sound_globals_playback_view *)g_51ebd4)->playback_parameters)[view->playback_index].gain_upper;
	real const *local_3 = (real const *)&local_1;
	local_0 = local_2;
	local_0.value = *local_3 + local_0.value;
	return *(long *)&local_0;
}
