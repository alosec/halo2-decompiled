// Exact DirectSound channel filter, LFO, and effect-data submission isolated
// from the broad recovered Xbox audio unit.  All stream operations and effect
// image state remain genuine external boundaries.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundStream;

struct s_sound_random_parameter
{
	real lower;
	real upper;
	real offset;
	real variation;
};

struct s_sound_filter_parameters
{
	long mode;
	long quality;
	s_sound_random_parameter coefficients[4];
};

struct s_sound_lfo_parameters
{
	s_sound_random_parameter values[4];
};

struct s_sound_channel_parameters
{
	dword seed;
	real interpolation;
	byte unknown08[8];
	s_sound_filter_parameters const *filter;
	s_sound_lfo_parameters const *pitch_lfo;
	s_sound_lfo_parameters const *combined_lfo;
	dword effect_words[0x100];
	dword effect_data_size;
};

namespace
{
struct s_sound_stream
{
	byte unknown00;
	byte unknown01;
	byte unknown02;
	byte flags;
	byte unknown04[0x20];
	IDirectSoundStream *stream;
	long unknown28;
	long offset;
	void *codec;
};

struct s_sound_driver_channel_parameter_view
{
	byte unknown0000[0xc];
	s_sound_stream channels[0x80];
};

struct s_filter_description
{
	dword mode;
	dword quality;
	dword coefficients[4];
};

struct s_lfo_description
{
	dword lfo;
	dword delay;
	dword delta;
	long pitch_modulation;
	long filter_cutoff_range;
	long amplitude_modulation;
};

// Matching retail writes these exact public Xbox descriptor mode values.
static const dword k_filter_mode_bypass = 0;
static const dword k_filter_mode_dls2 = 1;
static const dword k_filter_mode_parametric_equalizer = 2;
static const dword k_filter_mode_multiband = 3;

template<typename T>
static inline T pin_value(T value, T lower, T upper)
{
	return value < lower ? lower : value > upper ? upper : value;
}

static inline long stream_channel_count(s_sound_stream const *stream)
{
	return static_cast<signed char>(stream->unknown01 << 2) >> 5;
}

static_assert(sizeof(s_sound_random_parameter) == 0x10,
	"sound random parameter size");
static_assert(sizeof(s_sound_filter_parameters) == 0x48,
	"sound filter parameter size");
static_assert(sizeof(s_sound_lfo_parameters) == 0x40,
	"sound LFO parameter size");
static_assert(offsetof(s_sound_channel_parameters, filter) == 0x10,
	"sound channel filter offset");
static_assert(offsetof(s_sound_channel_parameters, effect_words) == 0x1c,
	"sound channel effect data offset");
static_assert(offsetof(s_sound_channel_parameters, effect_data_size) == 0x41c,
	"sound channel effect size offset");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
static_assert(offsetof(s_sound_stream, stream) == 0x24,
	"sound stream interface offset");
static_assert(sizeof(s_filter_description) == 0x18,
	"DirectSound filter description size");
static_assert(sizeof(s_lfo_description) == 0x18,
	"DirectSound LFO description size");
static_assert(offsetof(s_sound_driver_channel_parameter_view, channels) == 0xc,
	"sound driver channels offset");
}

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

real function_219880(s_sound_random_parameter const *parameter,
	dword *seed, real interpolation);
long function_21e330(real decibels);
long sound_filter_frequency_coefficient(real frequency);
dword sound_filter_gain_coefficient(real decibels);
void function_21f960(dword size, dword const *buffer);

extern "C" long __stdcall IDirectSoundStream_SetFilter(
	IDirectSoundStream *stream, s_filter_description const *description);
extern "C" long __stdcall IDirectSoundStream_SetLFO(
	IDirectSoundStream *stream, s_lfo_description const *description);

// @retail 0x2201f0
void function_2201f0(long channel_index,
	s_sound_channel_parameters const *parameters, bool force)
{
	if (!parameters)
		return;

	s_sound_driver_channel_parameter_view *globals =
		reinterpret_cast<s_sound_driver_channel_parameter_view *>(g_51ebe4);
	s_sound_stream *stream = &globals->channels[channel_index];
	real interpolation = parameters->interpolation;
	dword seed = parameters->seed;
	bool filter_set = false;
	bool pitch_set = false;
	bool combined_set = false;

	s_sound_filter_parameters const *filter = parameters->filter;
	if (filter)
	{
		if (!force && fabs(interpolation -
			*reinterpret_cast<real *>(stream->unknown04)) < 0.0001f)
			goto finished;

		s_filter_description description = {};
		real a = function_219880(&filter->coefficients[0], &seed, interpolation);
		real b = function_219880(&filter->coefficients[1], &seed, interpolation);
		real c = function_219880(&filter->coefficients[2], &seed, interpolation);
		real d = function_219880(&filter->coefficients[3], &seed, interpolation);

		if (filter->mode == 0)
		{
			description.mode = k_filter_mode_parametric_equalizer;
			description.quality = static_cast<dword>(
				pin_value(filter->quality, 0L, 7L));
			description.coefficients[0] = sound_filter_frequency_coefficient(a);
			description.coefficients[1] = function_21e330(b);
			description.coefficients[2] = sound_filter_frequency_coefficient(c);
			description.coefficients[3] = function_21e330(d);
		}
		else if (filter->mode == 1)
		{
			description.mode = k_filter_mode_dls2;
			description.coefficients[0] = sound_filter_frequency_coefficient(a);
			description.coefficients[1] = sound_filter_gain_coefficient(b);
			description.coefficients[2] = sound_filter_frequency_coefficient(c);
			description.coefficients[3] = sound_filter_gain_coefficient(d);
		}
		else if (filter->mode == 2 && stream_channel_count(stream) == 0)
		{
			description.mode = k_filter_mode_multiband;
			description.quality = static_cast<dword>(
				pin_value(filter->quality, 0L, 7L));
			description.coefficients[0] = sound_filter_frequency_coefficient(a);
			description.coefficients[1] = sound_filter_gain_coefficient(b);
			description.coefficients[2] = sound_filter_frequency_coefficient(c);
			description.coefficients[3] = function_21e330(d);
		}
		else
			goto filter_finished;

		filter_set = true;
		IDirectSoundStream_SetFilter(stream->stream, &description);
	}

filter_finished:
	if (force)
	{
		s_sound_lfo_parameters const *pitch = parameters->pitch_lfo;
		if (pitch)
		{
			s_lfo_description description = {};
			description.lfo = 1;
			real delay = function_219880(&pitch->values[0], &seed, interpolation);
			real frequency = function_219880(&pitch->values[1], &seed, interpolation);
			real amount = function_219880(&pitch->values[2], &seed, interpolation);
			description.delay = static_cast<dword>(pin_value(
				real_truncate(delay * 1500.0f), 0L, 0xffffL));
			description.delta = static_cast<dword>(pin_value(
				real_truncate(frequency * 43.69066619873047f), 0L, 0x3ffL));
			description.pitch_modulation = pin_value(
				real_truncate(amount * 128.0f), -128L, 127L);
			IDirectSoundStream_SetLFO(stream->stream, &description);
			pitch_set = true;
		}

		s_sound_lfo_parameters const *combined = parameters->combined_lfo;
		if (combined)
		{
			s_lfo_description description = {};
			description.lfo = 0;
			real delay = function_219880(&combined->values[0], &seed, interpolation);
			real frequency = function_219880(&combined->values[1], &seed, interpolation);
			real cutoff = function_219880(&combined->values[2], &seed, interpolation);
			real amplitude = function_219880(&combined->values[3], &seed, interpolation);
			description.delay = static_cast<dword>(pin_value(
				real_truncate(delay * 1500.0f), 0L, 0xffffL));
			description.delta = static_cast<dword>(pin_value(
				real_truncate(frequency * 43.69066619873047f), 0L, 0x3ffL));
			description.filter_cutoff_range = pin_value(
				real_truncate(cutoff * 16.0f), -128L, 127L);
			description.amplitude_modulation = pin_value(
				real_truncate(amplitude * 16.0f), -128L, 128L);
			IDirectSoundStream_SetLFO(stream->stream, &description);
			combined_set = true;
		}

		if (!filter_set)
		{
			s_filter_description description = {};
			description.mode = k_filter_mode_bypass;
			IDirectSoundStream_SetFilter(stream->stream, &description);
		}
		if (!pitch_set)
		{
			s_lfo_description description = {};
			description.lfo = 1;
			IDirectSoundStream_SetLFO(stream->stream, &description);
		}
		if (!combined_set)
		{
			s_lfo_description description = {};
			description.lfo = 0;
			IDirectSoundStream_SetLFO(stream->stream, &description);
		}
	}

finished:
	*reinterpret_cast<real *>(stream->unknown04) = interpolation;
	function_21f960(parameters->effect_data_size, parameters->effect_words);
}

#endif
