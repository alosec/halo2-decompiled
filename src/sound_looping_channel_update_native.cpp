// Exact shared looping-channel volume, routing, pitch, and parameter update
// isolated from the broad Xbox DirectSound unit.  Voice, submix, stream, and
// effects-interface operations remain genuine external boundaries.

#include "unknown_11c920.h"
#include <math.h>
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundStream;
struct IDirectSoundBuffer;
struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_sound_channel_parameters;

struct s_mixbin_volume_pair
{
	dword mixbin;
	long volume;
};

struct s_mixbins
{
	dword count;
	s_mixbin_volume_pair *pairs;
};

struct s_mixbin_settings
{
	s_mixbin_volume_pair pairs[8];
	s_mixbins mixbins;
};

struct s_mixbin_list
{
	long count;
	s_mixbin_volume_pair pairs[8];
};

struct s_channel_update_input
{
	dword field_0;
	long format;
	real pitch;
	real gain;
	byte field_10[4];
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	short voice_index;
	byte field_26[2];
	real field_28;
	long field_2c;
	dword field_30;
	byte field_34[4];
	real levels[4];
};

struct s_channel_send_state
{
	dword flags;
	real level;
	real level_offset;
	real gain0c;
	real gain10;
	real field_14;
	real left_gain;
	real right_gain;
	real rear_left_gain;
	real rear_right_gain;
};

class c_channel_effects_view
{
public:
	virtual void initialize(void *, void *, bool) = 0;
	virtual void dispose() = 0;
	virtual void set_i3dl2(void *, long) = 0;
	virtual void add_effect_sends(s_channel_send_state *, bool, long,
		s_mixbin_settings *) = 0;
	virtual void add_send(long, long, real, s_mixbin_settings *) = 0;
	virtual void add_source_mixbins(s_channel_update_input const *, bool,
		long, s_mixbin_settings *) = 0;
	virtual void add_sends(s_channel_send_state *, long, long,
		s_mixbin_settings *) = 0;
};

struct s_sound_stream
{
	byte unknown00;
	byte state_and_channels;
	byte chunks_and_started;
	byte flags;
	byte unknown04[4];
	long pitch;
	long gain_bits;
	byte send_cache[0xc];
	void *chunks[2];
	IDirectSoundStream *stream;
	long unknown28;
	long offset;
	void *codec;
};

struct s_driver_voice
{
	byte unknown00[0x34];
	IDirectSoundBuffer *buffer;
	IDirectSoundBuffer *submix;
};

struct s_driver_channel_update_view
{
	bool initialized;
	bool surround;
	byte unknown0002[6];
	short special_voice_threshold;
	byte unknown000a[2];
	s_sound_stream channels[0x80];
	byte unknown1a0c[0xc];
	s_driver_voice voices[0x40];
	byte unknown2918[0x198];
	void *direct_sound;
	c_channel_effects_view *effects;
	byte unknown2ab8[0x20];
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

real function_12aff0(real lower, real upper, real value, bool clamp);
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin,
	real decibels);
void function_2215d0(s_mixbin_list *list, s_mixbin_settings *settings,
	IDirectSoundStream *stream);
void function_2216f0(s_mixbin_list *list, s_mixbin_settings *settings,
	IDirectSoundBuffer *buffer);
void function_2201f0(long channel_index,
	s_sound_channel_parameters const *parameters, bool force);

extern "C" long __stdcall IDirectSoundStream_GetVoiceProperties(
	IDirectSoundStream *stream, s_mixbin_list *properties);
extern "C" long __stdcall IDirectSoundStream_SetVolume(
	IDirectSoundStream *stream, long volume);
extern "C" long __stdcall IDirectSoundStream_SetMixBins(
	IDirectSoundStream *stream, s_mixbins const *mixbins);
extern "C" long __stdcall IDirectSoundStream_SetPitch(
	IDirectSoundStream *stream, long pitch);
extern "C" long __stdcall IDirectSoundBuffer_GetVoiceProperties(
	IDirectSoundBuffer *buffer, s_mixbin_list *properties);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSoundBuffer_GetStatus(
	IDirectSoundBuffer *buffer, dword *status);
extern "C" long __stdcall IDirectSoundBuffer_Play(
	IDirectSoundBuffer *buffer, dword reserved1, dword reserved2, dword flags);
extern "C" long __stdcall IDirectSoundBuffer_StopEx(
	IDirectSoundBuffer *buffer, dword timestamp, dword flags);

namespace
{
static const real g_44a23c = -64.0f;
static const dword k_direct_sound_status_playing = 1;

template<typename T>
static inline T pin_value(T value, T lower, T upper)
{
	return value < lower ? lower : value > upper ? upper : value;
}

static inline real real_from_bits(long value)
{
	union
	{
		long integer;
		real scalar;
	} converted = { value };
	return converted.scalar;
}

static inline long real_bits(real value)
{
	union
	{
		real scalar;
		long integer;
	} converted = { value };
	return converted.integer;
}

static inline long sound_decibels_to_volume(real decibels)
{
	return static_cast<long>(
		function_12aff0(-64.0f, 0.0f, decibels, true) * 6400.0f - 6400.0f);
}

static inline void sound_mixbins_initialize(s_mixbin_settings *settings)
{
	settings->mixbins.count = 0;
	settings->mixbins.pairs = settings->pairs;
}

static inline long stream_channel_count(s_sound_stream const *stream)
{
	return static_cast<signed char>(stream->state_and_channels << 2) >> 5;
}

static inline real channel_pin_gain(real value)
{
	return pin_value(value, -64.0f, 0.0f);
}

static inline long channel_pin_gain_bits(long bits)
{
	real value = real_from_bits(bits);
	if (value < -64.0f)
		return static_cast<long>(0xc2800000u);
	if (value > 0.0f)
		return 0;
	return bits;
}

static inline real channel_linear_gain(real value)
{
	real scaled = real_from_bits(channel_pin_gain_bits(real_bits(value))) * 0.05f;
	return static_cast<real>(exp(scaled * 2.3025851f));
}

static inline c_channel_effects_view *channel_effects(
	s_driver_channel_update_view *globals)
{
	return globals->effects;
}

static_assert(sizeof(s_mixbin_volume_pair) == 0x08,
	"mixbin volume pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin header size");
static_assert(sizeof(s_mixbin_settings) == 0x48, "mixbin settings size");
static_assert(sizeof(s_mixbin_list) == 0x44, "captured mixbin list size");
static_assert(sizeof(s_channel_update_input) == 0x48,
	"looping channel input size");
static_assert(offsetof(s_channel_update_input, voice_index) == 0x24,
	"looping channel voice offset");
static_assert(offsetof(s_channel_update_input, field_30) == 0x30,
	"looping channel flags offset");
static_assert(offsetof(s_channel_update_input, levels) == 0x38,
	"looping channel levels offset");
static_assert(sizeof(s_channel_send_state) == 0x28,
	"looping channel send state size");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, pitch) == 0x08,
	"sound stream pitch offset");
static_assert(offsetof(s_sound_stream, gain_bits) == 0x0c,
	"sound stream gain offset");
static_assert(offsetof(s_sound_stream, send_cache) == 0x10,
	"sound stream send-cache offset");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(sizeof(s_driver_voice) == 0x3c, "sound driver voice size");
static_assert(offsetof(s_driver_voice, buffer) == 0x34,
	"sound driver voice buffer offset");
static_assert(sizeof(s_driver_channel_update_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_channel_update_view, channels) == 0x0c,
	"sound driver channels offset");
static_assert(offsetof(s_driver_channel_update_view, voices) == 0x1a18,
	"sound driver voices offset");
static_assert(offsetof(s_driver_channel_update_view, effects) == 0x2ab4,
	"sound driver effects offset");
}

// @retail 0x21fa80
void function_21fa80(long channel_index,
	s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects, bool force)
{
	s_channel_update_input const *input =
		reinterpret_cast<s_channel_update_input const *>(properties);
	s_driver_channel_update_view *globals =
		reinterpret_cast<s_driver_channel_update_view *>(g_51ebe4);
	s_sound_stream *stream = &globals->channels[channel_index];
	bool changed = !(stream->flags & 1) || !globals->initialized;
	long gain_bits = real_bits(input->gain > -64.0f ? input->gain : g_44a23c);
	real gain = real_from_bits(gain_bits);
	long mode = stream_channel_count(stream);
	bool direct = (input->field_30 & 1) != 0;

	if (changed || !(fabs(gain - real_from_bits(stream->gain_bits)) < 0.1f))
	{
		if (input->voice_index >= globals->special_voice_threshold)
		{
			s_mixbin_list voice_properties;
			IDirectSoundStream_GetVoiceProperties(stream->stream,
				&voice_properties);
			s_driver_voice *voice = &globals->voices[input->voice_index];
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if (mode == 1)
				sound_mixbins_add(&settings,
					*reinterpret_cast<long *>(voice->unknown00), gain);
			if (mode == 0 || mode == 1)
				sound_mixbins_add(&settings,
					*reinterpret_cast<long *>(voice->unknown00), gain);
			function_2215d0(&voice_properties, &settings, stream->stream);
			IDirectSoundStream_SetVolume(stream->stream,
				sound_decibels_to_volume(0.0f));
		}
		else
		{
			IDirectSoundStream_SetVolume(stream->stream,
				sound_decibels_to_volume(gain));
		}
		stream->gain_bits = gain_bits;
	}

	if (changed || direct != ((stream->flags >> 5 & 1) != 0) ||
		(!direct &&
			(!(fabs(input->field_18 -
				reinterpret_cast<real *>(stream->send_cache)[0]) < 0.1f) ||
			 !(fabs(input->field_14 -
				reinterpret_cast<real *>(stream->send_cache)[2]) < 0.1f) ||
			 !(fabs(input->field_1c) < 0.1f) ||
			 !(fabs(input->field_20) < 0.1f))) ||
		(input->voice_index != NONE &&
			!(fabs(input->field_28 -
				reinterpret_cast<real *>(stream->send_cache)[1]) < 0.1f)))
	{
		s_channel_send_state sends;
		sends.flags = (input->field_30 >> 1 & 1) ? 4 : 0;
		if (input->field_30 >> 3 & 1)
			sends.flags |= 8;
		else
			sends.flags &= ~8u;
		if (input->field_30 >> 4 & 1)
			sends.flags |= 16;
		else
			sends.flags &= ~16u;
		if (input->field_30 >> 6 & 1)
			sends.flags |= 64;
		else
			sends.flags &= ~64u;
		if (direct)
			stream->flags |= 0x20;
		else
			stream->flags &= ~0x20;

		if (input->field_30 >> 5 & 1)
		{
			sends.left_gain = input->levels[0];
			sends.right_gain = input->levels[1];
			sends.rear_left_gain = input->levels[2];
			sends.rear_right_gain = input->levels[3];
			real const *levels = &sends.left_gain;
			real energy = 0.0f;
			for (long i = 0; i < 4; i++)
			{
				real amplitude = channel_linear_gain(levels[i]);
				energy += amplitude * amplitude;
			}
			real amplitude = static_cast<real>(sqrt(energy));
			real bounded = pin_value(amplitude, 0.0f, 1.0f);
			real level = amplitude > 0.0f
				? channel_pin_gain(static_cast<real>(log10(bounded) * 20.0f))
				: -64.0f;
			sends.level = channel_pin_gain(level);
			sends.level_offset = input->field_14;
			sends.flags |= 0x20;
		}
		else
		{
			if (!(input->field_30 & 1))
			{
				sends.level = input->field_18;
				sends.level_offset = input->field_14;
				sends.gain0c = input->field_1c;
				sends.gain10 = input->field_20;
				reinterpret_cast<real *>(stream->send_cache)[0] =
					input->field_18;
				sends.flags |= 1;
				reinterpret_cast<real *>(stream->send_cache)[2] =
					input->field_14;
			}
			if (input->voice_index != NONE)
			{
				sends.field_14 = input->field_28;
				sends.flags |= 2;
				reinterpret_cast<real *>(stream->send_cache)[1] =
					input->field_28;
			}
		}

		if (input->voice_index >= globals->special_voice_threshold &&
			!(sends.flags & 0x20))
		{
			s_driver_voice *voice = &globals->voices[input->voice_index];
			s_mixbin_settings unused;
			sound_mixbins_initialize(&unused);
			s_mixbin_list buffer_properties;
			s_mixbin_list submix_properties;
			IDirectSoundBuffer_GetVoiceProperties(voice->buffer,
				&buffer_properties);
			IDirectSoundBuffer_GetVoiceProperties(voice->submix,
				&submix_properties);
			if (sends.flags & 2)
				IDirectSoundBuffer_SetVolume(voice->buffer,
					sound_decibels_to_volume(sends.field_14));
			else
				IDirectSoundBuffer_SetVolume(voice->buffer, -10000);

			dword status;
			if (sends.flags & 1)
			{
				s_mixbin_settings settings;
				sound_mixbins_initialize(&settings);
				channel_effects(globals)->add_effect_sends(&sends,
					globals->surround, 0, &settings);
				function_2216f0(&submix_properties, &settings,
					voice->submix);
				IDirectSoundBuffer_GetStatus(voice->submix, &status);
				if (!(status & k_direct_sound_status_playing))
					IDirectSoundBuffer_Play(voice->submix, 0, 0, 0);
			}
			else
			{
				IDirectSoundBuffer_GetStatus(voice->submix, &status);
				if (status & k_direct_sound_status_playing)
					IDirectSoundBuffer_StopEx(voice->submix, 0, 0);
			}
		}
		else
		{
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if (sends.flags & 1)
				channel_effects(globals)->add_effect_sends(&sends,
					globals->surround, mode, &settings);
			if (sends.flags & 2)
				channel_effects(globals)->add_send(31, mode,
					sends.field_14, &settings);
			if (sends.flags & 0x20)
				channel_effects(globals)->add_sends(&sends,
					globals->surround, mode, &settings);
			channel_effects(globals)->add_source_mixbins(input,
				globals->surround, mode, &settings);
			s_mixbin_settings unused;
			sound_mixbins_initialize(&unused);
			IDirectSoundStream_SetMixBins(stream->stream,
				&settings.mixbins);
		}
	}

	if (!force)
	{
		long pitch_offset;
		if (input->format == 0)
			pitch_offset = -4597;
		else if (input->format == 1)
			pitch_offset = -501;
		else
			pitch_offset = -2396;
		real pitch = input->pitch * 3.4133334159851074f +
			static_cast<real>(pitch_offset);
		pitch = pin_value(pitch, -32767.0f, 8191.0f);
		long value = static_cast<long>(pitch);
		long difference = value - stream->pitch;
		if (difference < 0)
			difference = -difference;
		if (changed || static_cast<real>(difference) >= 1.5f)
		{
			IDirectSoundStream_SetPitch(stream->stream, value);
			stream->pitch = value;
		}
	}

	function_2201f0(channel_index,
		reinterpret_cast<s_sound_channel_parameters const *>(effects), false);
}

#endif
