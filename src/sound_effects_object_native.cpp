// Exact process-lifetime sound-effects object and its seven-slot retail
// virtual interface. Direct initialization avoids a host CRT constructor.
// DirectSound filter, I3DL2, volume, and mixbin work remains genuine.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundBuffer;

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

struct s_sound_effect_buffers
{
	long mixbin00;
	long mixbin04;
	long mixbin08;
	IDirectSoundBuffer *buffer0c;
	IDirectSoundBuffer *buffer10;
	IDirectSoundBuffer *buffer14;
	IDirectSoundBuffer *buffer18;
	IDirectSoundBuffer *buffer1c;
	IDirectSoundBuffer *buffer20;
};

struct s_sound_effect_settings
{
	real level00;
	real level04;
	real level08;
	real level0c;
	real level10;
	real gain;
	real unknown18;
	real level1c;
	real level20;
	real level24;
	real level28;
	real level2c;
	byte unknown30[0x10];
};

struct s_i3dl2_buffer
{
	long direct;
	long direct_hf;
	long room;
	long room_hf;
	real room_rolloff_factor;
	long obstruction_hf;
	real obstruction_lf_ratio;
	long occlusion_hf;
	real occlusion_lf_ratio;
};

struct s_sound_i3dl2_parameters
{
	s_i3dl2_buffer i3dl2;
	long cutoff;
	real gain;
};

struct s_filter_description
{
	dword mode;
	dword q_coefficient;
	dword coefficients[4];
};

struct s_sound_send_parameters
{
	dword flags;
	real level;
	real level_offset;
	real gain0c;
	real gain10;
	byte unknown14[4];
	real left_gain;
	real right_gain;
	real rear_left_gain;
	real rear_right_gain;
};

struct s_sound_mixbin_source
{
	byte unknown00[0x12];
	short mixbin;
	byte unknown14[0x1c];
	dword flags;
	word speaker_mode;
};

extern real const g_44f70c = 0.0f;
extern real const g_44f710 = -64.0f;

real function_12aff0(real lower, real upper, real value, bool clamp);
long sound_filter_frequency_coefficient(real frequency);
dword sound_filter_gain_coefficient(real gain);
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin,
	real decibels);

extern "C" long __stdcall IDirectSoundBuffer_SetMixBinVolumes(
	IDirectSoundBuffer *buffer, s_mixbins const *mixbins);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSoundBuffer_SetI3DL2Source(
	IDirectSoundBuffer *buffer, s_i3dl2_buffer const *parameters,
	dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetFilter(
	IDirectSoundBuffer *buffer, s_filter_description const *description);

typedef void (*s_sound_effects_method)(void);

class c_sound_effects
{
public:
	s_sound_effects_method const *vtable;
	s_sound_effect_settings settings;
	s_sound_effect_buffers *buffers;
};

static_assert(sizeof(s_mixbin_settings) == 0x48, "mixbin settings size");
static_assert(sizeof(s_sound_effect_buffers) == 0x24,
	"sound effect buffers size");
static_assert(sizeof(s_sound_effect_settings) == 0x40,
	"sound effect settings size");
static_assert(sizeof(s_i3dl2_buffer) == 0x24, "I3DL2 buffer size");
static_assert(sizeof(s_sound_i3dl2_parameters) == 0x2c,
	"I3DL2 parameter size");
static_assert(sizeof(s_filter_description) == 0x18,
	"filter description size");
static_assert(sizeof(s_sound_send_parameters) == 0x28,
	"sound send parameter size");
static_assert(offsetof(s_sound_mixbin_source, mixbin) == 0x12,
	"sound source mixbin offset");
static_assert(offsetof(s_sound_mixbin_source, flags) == 0x30,
	"sound source flags offset");
static_assert(sizeof(c_sound_effects) == 0x48, "sound effects object size");
static_assert(offsetof(c_sound_effects, settings) == 0x04,
	"sound effects settings offset");
static_assert(offsetof(c_sound_effects, buffers) == 0x44,
	"sound effects buffers offset");

static inline bool test_flag(dword flags, long bit)
{
	return (flags & (1u << bit)) != 0;
}

static inline void sound_mixbins_initialize(s_mixbin_settings *settings)
{
	settings->mixbins.count = 0;
	settings->mixbins.pairs = settings->pairs;
}

static inline void sound_stream_append_mixbin(s_mixbin_settings *settings,
	long mixbin, real decibels)
{
	dword index = settings->mixbins.count;
	settings->pairs[index].mixbin = static_cast<dword>(mixbin);
	settings->pairs[index].volume = static_cast<long>(
		function_12aff0(-64.0f, 0.0f, decibels, true) * 6400.0f - 6400.0f);
	settings->mixbins.count++;
}

void h2_sound_effects_initialize(c_sound_effects *self,
	s_sound_effect_settings *settings, s_sound_effect_buffers *buffers,
	bool rear)
	__asm__("?initialize@c_sound_effects@@UAAXPAUs_sound_effect_settings@@PAUs_sound_effect_buffers@@_N@Z");
void h2_sound_effects_initialize(c_sound_effects *self,
	s_sound_effect_settings *settings, s_sound_effect_buffers *buffers,
	bool rear)
{
	s_mixbin_settings mixbins;
	self->buffers = buffers;
	self->settings = *settings;

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, self->settings.level00);
	sound_stream_append_mixbin(&mixbins, 8,
		rear ? self->settings.level04 : -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, -64.0f);
	sound_stream_append_mixbin(&mixbins, 9, -64.0f);
	IDirectSoundBuffer_SetMixBinVolumes(buffers->buffer0c, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(buffers->buffer0c, 0);
	IDirectSoundBuffer_SetVolume(buffers->buffer18, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, -64.0f);
	sound_stream_append_mixbin(&mixbins, 8, -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, self->settings.level00);
	sound_stream_append_mixbin(&mixbins, 9,
		rear ? self->settings.level04 : -64.0f);
	IDirectSoundBuffer_SetMixBinVolumes(buffers->buffer10, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(buffers->buffer10, 0);
	IDirectSoundBuffer_SetVolume(buffers->buffer1c, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, -64.0f);
	sound_stream_append_mixbin(&mixbins, 8, -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, -64.0f);
	sound_stream_append_mixbin(&mixbins, 9, -64.0f);
	sound_stream_append_mixbin(&mixbins, 10, 0.0f);
	IDirectSoundBuffer_SetMixBinVolumes(buffers->buffer20, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(buffers->buffer20, 0);
	IDirectSoundBuffer_SetVolume(buffers->buffer14, -6400);
}

void h2_sound_effects_dispose(c_sound_effects *self)
	__asm__("?v1@c_sound_effects@@UAAXXZ");
void h2_sound_effects_dispose(c_sound_effects *self)
{
	(void)self;
}

void h2_sound_effects_set_i3dl2(c_sound_effects *self,
	s_sound_i3dl2_parameters *parameters, long unused)
	__asm__("?set_i3dl2@c_sound_effects@@UAAXPAUs_sound_i3dl2_parameters@@J@Z");
void h2_sound_effects_set_i3dl2(c_sound_effects *self,
	s_sound_i3dl2_parameters *parameters, long unused)
{
	(void)unused;
	s_filter_description filter;
	IDirectSoundBuffer_SetI3DL2Source(self->buffers->buffer20,
		&parameters->i3dl2, 1);
	filter.mode = 1;
	filter.q_coefficient = 0;
	filter.coefficients[0] = static_cast<dword>(
		sound_filter_frequency_coefficient(static_cast<real>(parameters->cutoff)));
	filter.coefficients[1] = static_cast<dword>(
		sound_filter_gain_coefficient(parameters->gain));
	filter.coefficients[2] = 0;
	filter.coefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(self->buffers->buffer0c, &filter);
	IDirectSoundBuffer_SetFilter(self->buffers->buffer10, &filter);
}

static inline real sound_effects_send_level(c_sound_effects *self,
	s_sound_send_parameters *parameters, long mode)
{
	return mode == 1
		? (test_flag(parameters->flags, 6) ? g_44f70c : self->settings.level2c)
		: self->settings.level10;
}

void h2_sound_effects_add_send(c_sound_effects *self, long mixbin,
	long mode, real gain, s_mixbin_settings *mixbins)
	__asm__("?add_send@c_sound_effects@@UAAXJJMPAUs_mixbin_settings@@@Z");
void h2_sound_effects_add_send(c_sound_effects *self, long mixbin,
	long mode, real gain, s_mixbin_settings *mixbins)
{
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, mixbin, gain);
		break;
	case 1:
	{
		union s_send_level { real value; long bits; };
		long gain_bits = *reinterpret_cast<long *>(&gain);
		volatile s_send_level first;
		first.bits = gain_bits;
		first.value = self->settings.gain + first.value;
		sound_stream_append_mixbin(mixbins, mixbin, first.value);
		volatile s_send_level second;
		second.bits = gain_bits;
		second.value = self->settings.gain + second.value;
		sound_stream_append_mixbin(mixbins, mixbin, second.value);
		break;
	}
	}
}

static void sound_effects_add_direct(c_sound_effects *self,
	s_sound_send_parameters *parameters, bool rear, long mode,
	s_mixbin_settings *mixbins)
{
	if (test_flag(parameters->flags, 2))
	{
		bool alternate = test_flag(parameters->flags, 3);
		real level = rear ? self->settings.level1c : self->settings.level24;
		real level2 = rear ? self->settings.level20 : self->settings.level28;
		sound_stream_append_mixbin(mixbins, alternate ? 0 : 6,
			level + parameters->level);
		sound_stream_append_mixbin(mixbins, alternate ? 1 : 7,
			parameters->level + level);
		h2_sound_effects_add_send(self, 2, mode,
			level2 + parameters->level, mixbins);
		h2_sound_effects_add_send(self, 10, mode,
			parameters->level_offset, mixbins);
	}
	else
	{
		real send_level = sound_effects_send_level(self, parameters, mode);
		real offset = parameters->level_offset + parameters->level;
		if (!test_flag(parameters->flags, 3))
		{
			sound_mixbins_add(mixbins, self->buffers->mixbin00,
				parameters->gain0c + (send_level + parameters->level));
			sound_mixbins_add(mixbins, self->buffers->mixbin04,
				parameters->gain10 + (parameters->level + send_level));
			h2_sound_effects_add_send(self, self->buffers->mixbin08,
				mode, offset, mixbins);
		}
		else
		{
			if (self->settings.level00 > -64.0f)
			{
				sound_mixbins_add(mixbins, 0,
					parameters->gain0c + (self->settings.level00 +
					(parameters->level + send_level)));
				sound_mixbins_add(mixbins, 1,
					parameters->gain10 + (self->settings.level00 +
					(parameters->level + send_level)));
			}
			if (rear && self->settings.level04 > -64.0f)
			{
				sound_mixbins_add(mixbins, 4,
					parameters->gain0c + (parameters->level + send_level +
					self->settings.level04));
				sound_mixbins_add(mixbins, 5,
					parameters->gain10 + (parameters->level + send_level +
					self->settings.level04));
			}
			if (mode == 0)
				sound_mixbins_add(mixbins, 10, offset);
			else if (mode == 1)
			{
				sound_mixbins_add(mixbins, 10, parameters->gain0c + offset);
				sound_mixbins_add(mixbins, 10, parameters->gain10 + offset);
			}
		}
	}
}

static void sound_effects_add_split(c_sound_effects *self,
	s_sound_send_parameters *parameters, bool rear, long mode,
	s_mixbin_settings *mixbins)
{
	real send_level = sound_effects_send_level(self, parameters, mode);
	real offset = parameters->level_offset;
	real level = parameters->level;
	bool alternate = test_flag(parameters->flags, 3);
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;
	if (self->settings.level08 > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, left,
			parameters->gain0c + (self->settings.level08 +
			(send_level + level)));
		sound_stream_append_mixbin(mixbins, right,
			parameters->gain10 + (self->settings.level08 +
			(level + send_level)));
	}
	if (rear && self->settings.level0c > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, rear_left,
			parameters->gain0c + (self->settings.level0c +
			(level + send_level)));
		sound_stream_append_mixbin(mixbins, rear_right,
			parameters->gain10 + (self->settings.level0c +
			(level + send_level)));
	}
	if (mode == 0)
		sound_stream_append_mixbin(mixbins, 10, offset);
	else if (mode == 1)
	{
		sound_stream_append_mixbin(mixbins, 10, parameters->gain0c + offset);
		sound_stream_append_mixbin(mixbins, 10, parameters->gain10 + offset);
	}
}

void h2_sound_effects_add_effect_sends(c_sound_effects *self,
	s_sound_send_parameters *parameters, bool rear, long mode,
	s_mixbin_settings *mixbins)
	__asm__("?add_effect_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@_NJPAUs_mixbin_settings@@@Z");
void h2_sound_effects_add_effect_sends(c_sound_effects *self,
	s_sound_send_parameters *parameters, bool rear, long mode,
	s_mixbin_settings *mixbins)
{
	if (test_flag(parameters->flags, 0))
	{
		if (test_flag(parameters->flags, 4))
			sound_effects_add_split(self, parameters, rear, mode, mixbins);
		else
			sound_effects_add_direct(self, parameters, rear, mode, mixbins);
	}
}

void h2_sound_effects_add_source_mixbins(c_sound_effects *self,
	s_sound_mixbin_source *source, bool rear, long mode,
	s_mixbin_settings *mixbins)
	__asm__("?add_source_mixbins@c_sound_effects@@UAAXPAUs_sound_mixbin_source@@_NJPAUs_mixbin_settings@@@Z");
void h2_sound_effects_add_source_mixbins(c_sound_effects *self,
	s_sound_mixbin_source *source, bool rear, long mode,
	s_mixbin_settings *mixbins)
{
	bool alternate = test_flag(source->flags, 3);
	long remaining = 8 - static_cast<long>(mixbins->mixbins.count);
	real gain = 0.0f;
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;
	switch (source->speaker_mode)
	{
	case 0:
		break;
	case 1:
		if (remaining >= 2)
		{
			sound_mixbins_add(mixbins, left, gain);
			sound_mixbins_add(mixbins, right, gain);
		}
		break;
	case 2:
		if (remaining >= 2)
		{
			sound_mixbins_add(mixbins, rear ? rear_left : left, gain);
			sound_mixbins_add(mixbins, rear ? rear_right : right, gain);
		}
		break;
	case 3:
		if (rear)
		{
			if (remaining >= 2)
			{
				sound_mixbins_add(mixbins, 2, gain);
				sound_mixbins_add(mixbins, 3, gain);
			}
		}
		else if (remaining >= 4)
		{
			sound_mixbins_add(mixbins, left, gain - 6.0206f);
			sound_mixbins_add(mixbins, left, g_44f710);
			sound_mixbins_add(mixbins, right, gain - 6.0206f);
			sound_mixbins_add(mixbins, right, g_44f710);
		}
		break;
	default:
		break;
	}
	if (source->mixbin != static_cast<short>(-1))
		h2_sound_effects_add_send(self, source->mixbin, mode, gain, mixbins);
}

void h2_sound_effects_add_sends(c_sound_effects *self,
	s_sound_send_parameters *parameters, long unused, long mode,
	s_mixbin_settings *mixbins)
	__asm__("?add_sends@c_sound_effects@@UAAXPAUs_sound_send_parameters@@JJPAUs_mixbin_settings@@@Z");
void h2_sound_effects_add_sends(c_sound_effects *self,
	s_sound_send_parameters *parameters, long unused, long mode,
	s_mixbin_settings *mixbins)
{
	(void)unused;
	bool alternate = test_flag(parameters->flags, 3);
	real level = parameters->level_offset + parameters->level;
	sound_stream_append_mixbin(mixbins, alternate ? 0 : 6,
		parameters->left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 1 : 7,
		parameters->right_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 4 : 8,
		parameters->rear_left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 5 : 9,
		parameters->rear_right_gain);
	h2_sound_effects_add_send(self, 10, mode, level, mixbins);
}

// Retail vtable 0x457140.
s_sound_effects_method const h2_sound_effects_vtable[7] =
{
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_initialize),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_dispose),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_set_i3dl2),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_add_effect_sends),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_add_send),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_add_source_mixbins),
	reinterpret_cast<s_sound_effects_method>(h2_sound_effects_add_sends),
};

c_sound_effects g_47f088 =
{
	h2_sound_effects_vtable,
	{},
	NULL,
};

#endif
