#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"
#include "unknown_221810.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

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

struct s_sound_voice_group
{
	short count;
	short voice_indices[16];
	short limit;
	bool started_this_tick;
	byte unknown25;
};

struct s_looping_voice_counts
{
	s_sound_voice_group definition;
	s_sound_voice_group source;
};

extern s_sound_voice *g_4e6378;
struct s_4e6380;
extern s_4e6380 *g_4e6380;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

namespace
{
struct s_sound_promotion_view
{
	short definition_voice_limit;
	short source_voice_limit;
	long preemption_time;
	byte unknown08[0x54];
};

struct s_sound_system_voice_view
{
	byte unknown00[0x84];
	long time;
	byte unknown88[0x180];
	short voice_count;
	short maximum_voice_count;
};

struct s_sound_channel
{
	byte unknown00[0xf];
	byte flags;
	byte unknown10[0x24];
};

static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(offsetof(s_sound_voice, channel_index) == 0xc,
	"sound voice channel offset");
static_assert(sizeof(s_sound_voice_group) == 0x26,
	"looping voice group size");
static_assert(sizeof(s_looping_voice_counts) == 0x4c,
	"looping voice counts size");
static_assert(offsetof(s_sound_system_voice_view, time) == 0x84,
	"sound system time offset");
static_assert(offsetof(s_sound_system_voice_view, voice_count) == 0x208,
	"sound system voice count offset");
static_assert(sizeof(s_sound_channel) == 0x34, "driver channel stride");
static_assert(offsetof(s_sound_channel, flags) == 0xf,
	"driver channel state offset");
static_assert(sizeof(s_sound_promotion_view) == 0x5c,
	"sound promotion size");
static_assert(sizeof(s_sound_playback) == 0xbc, "sound playback size");
static_assert(offsetof(s_sound_playback, start_time) == 0x8c,
	"sound playback start time offset");
static_assert(offsetof(s_sound_playback, value_a0) == 0xa0,
	"sound playback priority value offset");

s_sound_system_voice_view *sound_system()
{
	return reinterpret_cast<s_sound_system_voice_view *>(g_4e6380);
}

s_sound_promotion_view *sound_promotion(short index)
{
	return reinterpret_cast<s_sound_promotion_view *>(
		sound_class_definition_get(index));
}
}

// @retail 0x12a420
void function_12a420(short voice_index)
{
	s_sound_channel *channels = reinterpret_cast<s_sound_channel *>(g_51ebe4);
	channels[g_4e6378[voice_index].channel_index].flags |= 8;
}

// @retail 0x128a60
short function_128a60(long sound_index, short count,
	short const *voice_indices)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	long best_age = 0;
	short best = NONE;

	for (short i = 0; i < count; i++)
	{
		short voice_index = voice_indices[i];
		s_sound_playback *voice_sound =
			SOUND_PLAYBACK_GET(g_4e6378[voice_index].sound_index);
		long age = sound_system()->time - voice_sound->start_time;

		if ((age >= sound_promotion(definition->promotion_index)->preemption_time &&
			age > best_age) || sound->value_a0 > voice_sound->value_a0)
		{
			best = voice_index;
			best_age = age;
		}
	}
	return best;
}

// @retail 0x128500
void function_128500(long sound_index, s_looping_voice_counts *counts)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	s_sound_promotion_view *promotion =
		sound_promotion(definition->promotion_index);

	counts->definition.started_this_tick = false;
	counts->definition.count = 0;
	counts->source.count = 0;
	counts->definition.limit = promotion->definition_voice_limit;
	counts->source.limit = promotion->source_voice_limit;
	for (short i = 0; i < sound_system()->voice_count; i++)
	{
		s_sound_voice *voice = &g_4e6378[i];
		if (voice->sound_index != NONE && voice->sound_index != sound_index)
		{
			s_sound_playback *other = SOUND_PLAYBACK_GET(voice->sound_index);
			if (definition->type == voice->definition_type &&
				sound->definition_index == other->definition_index)
			{
				counts->definition.voice_indices[counts->definition.count++] = i;
				if (sound->object_index != NONE && other->object_index != NONE &&
					sound->source == other->source &&
					(sound->object_index == other->object_index ||
					(sound->source->same_source && sound->source->same_source(
						sound->object_index,
						reinterpret_cast<s_sound_source_state const *>(&sound->marker),
						other->object_index,
						reinterpret_cast<s_sound_source_state const *>(&other->marker)))))
				{
					counts->source.voice_indices[counts->source.count++] = i;
					if (sound_system()->time == other->start_time)
						counts->definition.started_this_tick = true;
				}
			}
		}
	}
}

#endif
