#include "data_array.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

struct s_sound;
struct s_sound_location_source;
struct s_looping_channel_spatialization;
struct s_4e6380;

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
struct s_sound_listener
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07;
	real velocity_scale;
	vector3f forward;
	vector3f left;
	vector3f up;
	point3f position;
	vector3f velocity;
};

struct s_sound_system_spatial_view
{
	byte unknown00[0x84];
	long time;
	s_sound_listener listeners[4];
};

struct s_sound_spatial_update
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

static_assert(sizeof(s_sound_listener) == 0x48, "sound listener size");
static_assert(sizeof(s_sound_voice) == 0x24, "sound voice size");
static_assert(sizeof(s_sound_spatial_update) == 0x38, "sound spatial size");
static_assert(offsetof(s_sound_playback, location) == 0x18,
	"playback location offset");
static_assert(offsetof(s_sound_playback, listener_index) == 0xa1,
	"playback listener offset");
static_assert(offsetof(s_sound_system_spatial_view, listeners) == 0x88,
	"sound listener array offset");

}

extern s_sound_voice *g_4e6378;
extern s_record_pool *g_4e637c;
extern s_4e6380 *g_4e6380;
extern long g_440c48;
extern long g_440c4c;
extern vector3f *g_4687a4;

void sound_source_get_position(s_sound_location_source const *source,
	long listener_index, point3f *position);
point3f *function_142700(transform4x3f const *matrix,
	point3f const *point, point3f *out);
long function_125060(s_sound const *sound, long definition_index);
vector3f *vector3d_decompress(dword compressed, vector3f *vector);
vector3f *function_1427f0(transform4x3f const *matrix,
	vector3f const *vector, vector3f *out);
real function_12ad30(vector3f const *offset, s_sound const *sound,
	long definition_index, vector3f const *direction);
long function_2197f0(real gain);
long function_12a6d0(short curve, real value, real range);
real function_2195f0(real decibels);
s_sound_definition *sound_definition_get(long definition_index);
struct s_unknown_5c;
s_unknown_5c *function_221810(short index);

// @retail 0x129aa0
void function_129aa0(short voice_index, long *gain,
	s_looping_channel_spatialization *spatialization)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_playback *playback =
		reinterpret_cast<s_sound_playback *>(g_4e637c->data) +
		(voice->sound_index & 0xffff);
	s_sound_spatial_update *output =
		reinterpret_cast<s_sound_spatial_update *>(spatialization);
	voice->unknown0b = voice->unknown0a;
	voice->unknown0a = 0;
	if (playback->location.audible == 1 || playback->location.audible == 2)
	{
		s_sound_listener *listener =
			&reinterpret_cast<s_sound_system_spatial_view *>(g_4e6380)
				->listeners[playback->listener_index];
		point3f position;
		vector3f direction = *g_4687a4;
		vector3f velocity;
		if (playback->location.audible == 1)
		{
			transform4x3f const *transform =
				reinterpret_cast<transform4x3f const *>(&listener->velocity_scale);
			sound_source_get_position(
				reinterpret_cast<s_sound_location_source const *>(&playback->location),
				playback->listener_index, &position);
			function_142700(transform, &position, &position);
			if (static_cast<byte>(function_125060(
				reinterpret_cast<s_sound const *>(&playback->location),
				playback->definition_index)))
			{
				vector3f *decompressed = vector3d_decompress(
					playback->location.spatial.compressed_forward, &direction);
				real const i = decompressed->i;
				real const j = decompressed->j;
				real const k = decompressed->k;
				direction.i = transform->rotation.forward.k * k +
					transform->rotation.forward.j * j +
					transform->rotation.forward.i * i;
				direction.j = transform->rotation.left.k * k +
					transform->rotation.left.j * j +
					transform->rotation.left.i * i;
				direction.k = transform->rotation.up.k * k +
					transform->rotation.up.j * j +
					transform->rotation.up.i * i;
			}
			function_1427f0(transform, &playback->location.spatial.velocity,
				&velocity);
		}
		else
		{
			position = playback->location.spatial.position;
			if (static_cast<byte>(function_125060(
				reinterpret_cast<s_sound const *>(&playback->location),
				playback->definition_index)))
			{
				vector3d_decompress(
					playback->location.spatial.compressed_forward, &direction);
			}
			velocity = playback->location.spatial.velocity;
		}
		(void)velocity;
		output->attenuation = function_12ad30(
			reinterpret_cast<vector3f const *>(&position),
			reinterpret_cast<s_sound const *>(&playback->location),
			playback->definition_index, &direction);
		if (playback->priority & 2)
			voice->unknown0a |= 2;
		else
		{
			*gain = decibels_add(*gain, function_2197f0(output->attenuation));
			voice->unknown0a |= 1;
		}
		if (playback->location.flag11)
			voice->unknown0a |= 1;
		output->position = *reinterpret_cast<vector3f *>(&position);
	}
	else if (playback->location.audible == 0)
	{
		voice->unknown0a |= 1;
		output->attenuation = playback->location.flag8 ? 0.0f : 1.0f;
		output->position = *reinterpret_cast<vector3f *>(g_468788);
	}
	output->doppler_scale = (voice->unknown0a & 1) ? 0.0f : 1.0f;
	long const now = reinterpret_cast<s_sound_system_spatial_view *>(g_4e6380)->time;
	long *transition_start = reinterpret_cast<long *>(voice->unknown14);
	if (!voice->unknown0b)
	{
		*transition_start = 0;
		output->direct_gain = (voice->unknown0a & 1) ? g_440c48 : g_440c4c;
		output->spatial_gain = (voice->unknown0a & 2) ?
			function_2197f0(output->attenuation) : g_440c4c;
	}
	else
	{
		if (voice->unknown0b != voice->unknown0a)
		{
			long remaining = 1000 - *transition_start - now;
			if (remaining < 0)
				remaining = 0;
			else if (remaining > 1000)
				remaining = 1000;
			*transition_start = now - remaining;
		}
		long fade_in = 0;
		long fade_out = 0xc2800000;
		if (*transition_start > 0)
		{
			long const elapsed = now - *transition_start;
			if (elapsed > 1000)
				*transition_start = 0;
			else
			{
				fade_in = function_12a6d0(1, static_cast<real>(elapsed), 1000.0f);
				fade_out = function_12a6d0(1, static_cast<real>(elapsed), -1000.0f);
			}
		}
		long const spatial_fade = (voice->unknown0a & 2) ? fade_in : fade_out;
		output->doppler_scale = function_2195f0(
			*reinterpret_cast<real const *>(&spatial_fade));
		voice->unknown0b = voice->unknown0a;
		output->direct_gain = (voice->unknown0a & 1) ? fade_in : fade_out;
		output->spatial_gain = decibels_add(
			(voice->unknown0a & 2) ? fade_in : fade_out,
			function_2197f0(output->attenuation));
	}
	output->flags = voice->unknown0a;
	output->field_c = 0;
	output->field_10 = 0;
	if (playback->location.flag11)
	{
		s_sound_definition *definition = sound_definition_get(playback->definition_index);
		long const promoted_gain = *reinterpret_cast<long *>(
			reinterpret_cast<byte *>(function_221810(definition->promotion_index)) + 0x14);
		output->direct_gain = decibels_add(promoted_gain, output->direct_gain);
	}
	if (!playback->source_updated && playback->source && playback->source->spatialize)
	{
		s_sound_spatial_update changed = *output;
		if (playback->source->spatialize(playback->object_index,
			playback->definition_index,
			reinterpret_cast<s_sound_source_view const *>(&playback->marker),
			reinterpret_cast<s_sound_spatialization_view *>(&changed)))
		{
			*output = changed;
		}
	}
}

#endif
