#include "data_array.h"
#include "unknown_124f90.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered per-frame playback update.  Driver channel submission,
 * spatialization, fade-stop, tag lookup, and source callbacks remain genuine
 * dependencies; this source does not invent an audio backend. */

struct s_4e6380
{
	byte unknown00[0x84];
	long time;
	byte unknown88[0x1e4 - 0x88];
	real master_fade;
	byte unknown1e8[0x208 - 0x1e8];
	short voice_count;
};

struct s_sound_voice
{
	long sound_index;
	long driver_voice_index;
	byte definition_type;
	bool stream_reset;
	byte unknown0a;
	byte unknown0b;
	short channel_index;
	short unknown0e;
	real unknown10;
	byte unknown14[4];
	short chunk_index;
	short next_chunk_index;
	void const *permutation;
	void const *next_permutation;
};

struct s_sound_driver_channel_usage;
struct s_looping_channel_spatialization;

struct s_sound_spatial_update
{
	dword field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	real field_14;
	real field_18;
	vector3f field_1c;
	real field_28[4];
};

static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(offsetof(s_sound_voice, channel_index) == 0x0c,
	"sound voice channel offset");
static_assert(offsetof(s_sound_voice, permutation) == 0x1c,
	"sound voice permutation offset");
static_assert(sizeof(s_sound_playback) == 0xbc, "sound playback size");
static_assert(offsetof(s_sound_playback, location) == 0x18,
	"sound playback location offset");
static_assert(offsetof(s_sound_playback, fade_start_time) == 0xb4,
	"sound fade start offset");
static_assert(offsetof(s_sound_playback, fade_end_time) == 0xb8,
	"sound fade end offset");
static_assert(offsetof(s_4e6380, time) == 0x84,
	"sound system time offset");
static_assert(offsetof(s_4e6380, master_fade) == 0x1e4,
	"sound master fade offset");
static_assert(offsetof(s_4e6380, voice_count) == 0x208,
	"sound voice count offset");
static_assert(sizeof(s_sound_spatial_update) == 0x38,
	"sound spatial update size");

extern s_4e6380 *g_4e6380;
extern s_sound_voice *g_4e6378;
extern s_record_pool *g_4e637c;

long function_2197f0(real gain);
void __stdcall function_21f360(s_sound_driver_channel_usage *usage);
long function_12a810(long sound_index);
void function_127320(long sound_index, long reason);
void function_129aa0(short voice_index, long *gain,
	s_looping_channel_spatialization *spatialization);
void function_21c400(short voice_index, real gain,
	s_looping_channel_spatialization const *spatialization);
void function_128ca0(short voice_index, long gain,
	s_looping_channel_spatialization const *spatialization);

static long decibels_add(long a, long b)
{
	real const result = *reinterpret_cast<real *>(&a) +
		*reinterpret_cast<real *>(&b);
	return *reinterpret_cast<long const *>(&result);
}

static s_sound_playback *sound_playback_get(long sound_index)
{
	return reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(sound_index & 0xffff);
}

// @retail 0x1293f0
void function_1293f0(void)
{
	s_4e6380 *sound_system = g_4e6380;
	long const master_gain = function_2197f0(sound_system->master_fade);
	function_21f360(reinterpret_cast<s_sound_driver_channel_usage *>(sound_system));
	for (long voice_index = 0; voice_index < g_4e6380->voice_count; voice_index++)
	{
		s_sound_voice *voice = &g_4e6378[static_cast<short>(voice_index)];
		if (voice->sound_index == NONE || voice->stream_reset)
			continue;
		s_sound_playback *playback = sound_playback_get(voice->sound_index);
		long gain = function_12a810(voice->sound_index);
		if (*reinterpret_cast<real *>(&gain) + playback->location.unknown08 <= -64.0f)
		{
			s_sound_playback *current = sound_playback_get(voice->sound_index);
			if (current->fading && current->fade_start_time > current->fade_end_time)
			{
				function_127320(voice->sound_index, 4);
				voice->sound_index = NONE;
				continue;
			}
		}
		s_sound_playback *current = sound_playback_get(voice->sound_index);
		if (current->fading && current->fade_end_time > current->fade_start_time &&
			g_4e6380->time >= current->fade_end_time)
		{
			current->fading = false;
		}
		if (voice->channel_index == NONE)
			continue;
		s_sound_spatial_update spatialization;
		function_129aa0(static_cast<short>(voice_index), &gain,
			reinterpret_cast<s_looping_channel_spatialization *>(&spatialization));
		gain = decibels_add(master_gain, gain);
		if ((1 << sound_playback_get(voice->sound_index)->state) & 0x1e)
		{
			function_21c400(static_cast<short>(voice_index),
				*reinterpret_cast<real *>(&gain),
				reinterpret_cast<s_looping_channel_spatialization const *>(
					&spatialization));
		}
		else
		{
			function_128ca0(static_cast<short>(voice_index), gain,
				reinterpret_cast<s_looping_channel_spatialization const *>(
					&spatialization));
		}
		if (!playback->source_updated && playback->source && playback->source->proc2)
		{
			playback->source->proc2(playback->object_index,
				reinterpret_cast<long>(playback->source_data),
				playback->definition_index, playback->pitch_range_index,
				reinterpret_cast<long>(voice->permutation),
				*reinterpret_cast<long *>(&voice->unknown10));
		}
	}
}

#endif
