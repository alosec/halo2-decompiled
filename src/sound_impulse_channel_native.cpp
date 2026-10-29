#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>
#include <string.h>

struct s_looping_channel_spatialization;
struct s_looping_track_sound;
struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_looping_impulse_parameters;
struct s_unknown_5c;
struct s_sound_permutation;
struct s_bink_sound_settings;

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

namespace
{
struct s_impulse_channel_properties
{
	dword flags;
	long format;
	real pitch;
	long gain_bits;
	byte unknown10[0x1c];
	long controller_index;
	dword effects_flags;
	short effect;
	byte unknown36[0x12];
	byte effect_playback[0x420];
	byte impulse[0x10];
};

struct s_impulse_permutation
{
	byte unknown00[4];
	signed char gain;
	byte unknown05[7];
	short first_chunk;
	short chunk_count;
};

struct s_sound_globals_chunks_view
{
	byte unknown00[0x24];
	struct s_pitch_range
	{
		byte unknown00[8];
		short first_permutation;
		short count;
	} *pitch_ranges;
	byte unknown28[4];
	s_impulse_permutation *permutations;
	byte unknown30[0x14];
	void *chunks;
};

struct s_sound_controller_view
{
	byte unknown00[3];
	byte playing_count;
	long definition_index;
	real listener_values[4];
};

struct s_driver_stream
{
	byte flags[4];
	byte unknown04[0x30];
};

struct s_driver_streams_view
{
	byte unknown00[0xc];
	s_driver_stream streams[1];
};

struct s_spatial_view
{
	dword flags;
	long spatial_gain;
	long direct_gain;
	long field_c;
	long field_10;
	real doppler_scale;
	real attenuation;
	vector3f position;
	real field_28[4];
};

static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(sizeof(s_impulse_channel_properties) == 0x478,
	"impulse channel properties size");
static_assert(offsetof(s_impulse_channel_properties, controller_index) == 0x2c,
	"channel controller offset");
static_assert(offsetof(s_impulse_channel_properties, effect_playback) == 0x48,
	"effect playback offset");
static_assert(offsetof(s_impulse_channel_properties, impulse) == 0x468,
	"impulse parameter offset");
static_assert(sizeof(s_impulse_permutation) == 0x10, "permutation size");
static_assert(sizeof(s_sound_controller_view) == 0x18, "controller view size");
static_assert(sizeof(s_driver_stream) == 0x34, "driver stream stride");
static_assert(sizeof(s_spatial_view) == 0x38, "spatialization size");

}

extern s_sound_voice *g_4e6378;
extern s_record_pool *g_4e637c;
extern void *g_51ebd8;
extern s_bink_sound_settings *g_51ebe4;

long function_1251e0(void const *definition, long gain_bits, real scale);
void function_12a1b0(short voice_index, s_looping_track_sound *sound,
	s_looping_channel_spatialization const *spatialization,
	s_looping_channel_properties *properties);
real function_12a9d0(long listener_index, s_sound_position const *position);
s_unknown_5c *function_221810(short index);
void function_21f8a0(long channel_index,
	s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects);
void __stdcall function_21f720(s_looping_impulse_parameters const *parameters);
void sound_voice_queue_chunk(short voice_index, ::s_sound_permutation const *permutation,
	short chunk_index);
void function_21fa80(long channel_index,
	s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects, bool force);
long __stdcall function_125e60(s_looping_track_sound *sound);
void sound_playback_set_chunk(long sound_index, long definition_index,
	char pitch_range_index, char permutation_index, short chunk_index);

// @retail 0x128ca0
void function_128ca0(short voice_index, long gain,
	s_looping_channel_spatialization const *spatialization)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_playback *playback =
		reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(voice->sound_index & 0xffff);
	s_sound_definition *definition = sound_definition_get(playback->definition_index);
	s_impulse_channel_properties properties;
	properties.flags = 0;
	memset(reinterpret_cast<byte *>(&properties) + 4, 0, 0x2c);
	properties.effects_flags = 0;
	memset(reinterpret_cast<byte *>(&properties) + 0x34, 0, 0x444);
	s_sound_playback_parameters *parameters =
		&SOUND_GLOBALS_DEFINITIONS->playback_parameters[definition->playback_index];
	real pitch = (static_cast<real>(parameters->pitch_upper) -
		static_cast<real>(parameters->pitch_lower)) * playback->location.scale +
		playback->pitch + static_cast<real>(parameters->pitch_lower);
	long total_gain = decibels_add(*reinterpret_cast<long *>(&playback->location.unknown08),
		decibels_add(*reinterpret_cast<long *>(&playback->gain), gain));
	total_gain = function_1251e0(definition, total_gain, playback->location.scale);
	function_12a1b0(voice_index,
		reinterpret_cast<s_looping_track_sound *>(playback), spatialization,
		reinterpret_cast<s_looping_channel_properties *>(&properties));
	s_sound_pitch_range *range =
		&SOUND_GLOBALS_DEFINITIONS->pitch_ranges[
			definition->pitch_range_base + playback->pitch_range_index];
	s_sound_globals_chunks_view *chunks =
		reinterpret_cast<s_sound_globals_chunks_view *>(g_51ebd4);
	s_impulse_permutation const *permutation =
		&chunks->permutations[range->first_permutation + playback->permutation_index];
	*reinterpret_cast<short *>(reinterpret_cast<byte *>(&properties) + 0x34) = 0;
	properties.controller_index = playback->effect_index;
	properties.pitch = pitch - static_cast<real>(
		SOUND_GLOBALS_DEFINITIONS->pitch_bounds[range->bounds_index].unknown00);
	if (playback->priority & 2)
	{
		s_unknown_5c *promotion = function_221810(definition->promotion_index);
		real doppler;
		if (playback->effect_index != NONE)
		{
			s_sound_controller_view *controller =
				reinterpret_cast<s_sound_controller_view *>(reinterpret_cast<s_record_pool *>(g_51ebd8)->data) +
				(playback->effect_index & 0xffff);
			doppler = controller->definition_index != NONE ?
				controller->listener_values[playback->listener_index] : 0.0f;
		}
		else
			doppler = function_12a9d0(playback->listener_index,
				&playback->location.spatial);
		properties.pitch += *reinterpret_cast<real *>(
			reinterpret_cast<byte *>(promotion) + 0x48) *
			reinterpret_cast<s_spatial_view const *>(spatialization)->doppler_scale *
			doppler;
	}
	if (!(playback->flags & 0x200))
	{
		real final_gain = *reinterpret_cast<real *>(&total_gain) +
			static_cast<real>(permutation->gain);
		if (final_gain < -64.0f)
			final_gain = -64.0f;
		else if (final_gain > 0.0f)
			final_gain = 0.0f;
		properties.gain_bits = *reinterpret_cast<long *>(&final_gain);
		function_21f8a0(voice->channel_index,
			reinterpret_cast<s_looping_channel_properties *>(&properties),
			reinterpret_cast<s_looping_effect_playback *>(properties.effect_playback));
		function_21f720(reinterpret_cast<s_looping_impulse_parameters *>(properties.impulse));
		sound_voice_queue_chunk(voice_index,
			reinterpret_cast<struct ::s_sound_permutation const *>(permutation),
			playback->chunk_index);
		if (playback->effect_index != NONE)
		{
			s_sound_controller_view *controller =
				reinterpret_cast<s_sound_controller_view *>(reinterpret_cast<s_record_pool *>(g_51ebd8)->data) +
				(playback->effect_index & 0xffff);
			byte synchronization_count = 0xff;
			if (controller->definition_index != NONE)
			{
				synchronization_count = controller->playing_count & 0x7f;
				controller->playing_count = (controller->playing_count - 1) & 0x7f;
			}
			playback->unknown03 = synchronization_count;
		}
		playback->flags |= 0x200;
	}
	else if (voice->permutation)
	{
		real final_gain = *reinterpret_cast<real *>(&total_gain) +
			static_cast<real>(*reinterpret_cast<signed char const *>(
				reinterpret_cast<byte const *>(voice->permutation) + 4));
		if (final_gain < -64.0f)
			final_gain = -64.0f;
		else if (final_gain > 0.0f)
			final_gain = 0.0f;
		properties.gain_bits = *reinterpret_cast<long *>(&final_gain);
		function_21fa80(voice->channel_index,
			reinterpret_cast<s_looping_channel_properties *>(&properties),
			reinterpret_cast<s_looping_effect_playback *>(properties.effect_playback), true);
	}
	if (!playback->flag6)
	{
		if (playback->flag3 && !voice->next_permutation &&
			static_cast<byte>(function_125e60(
				reinterpret_cast<s_looping_track_sound *>(playback))))
		{
			sound_voice_queue_chunk(voice_index,
				reinterpret_cast<struct ::s_sound_permutation const *>(permutation),
				playback->chunk_index);
			playback->flag3 = false;
		}
		if (!playback->flag3)
		{
			short next_chunk = NONE;
			if (permutation && playback->chunk_index < permutation->chunk_count - 1)
				next_chunk = playback->chunk_index + 1;
			if (next_chunk != NONE)
			{
				sound_playback_set_chunk(voice->sound_index,
					playback->definition_index, playback->pitch_range_index,
					playback->permutation_index, next_chunk);
				function_125e60(reinterpret_cast<s_looping_track_sound *>(playback));
				playback->flag3 = true;
			}
			else
			{
				playback->flag6 = true;
				s_driver_stream &stream =
					reinterpret_cast<s_driver_streams_view *>(g_51ebe4)
					->streams[g_4e6378[voice_index].channel_index];
				stream.flags[3] |= 1u << 3;
			}
		}
	}
}

#endif
