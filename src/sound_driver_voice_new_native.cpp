// Exact dormant effect-input voice construction isolated from the broader
// Xbox driver source.  Every created submix or 3D buffer remains a genuine
// DirectSound object and failure preserves recovered voice-count rollback.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct IDirectSoundBuffer;

struct s_buffer_description
{
	dword size;
	dword flags;
	dword buffer_bytes;
	void *format;
	void *mixbins;
	dword input_mixbin;
};

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

struct s_driver_voice
{
	dword unknown00;
	byte flags;
	byte unknown05[3];
	byte position[0x0c];
	byte unknown14[0x18];
	real unknown2c;
	real unknown30;
	IDirectSoundBuffer *buffer;
	IDirectSoundBuffer *submix;
};

struct s_driver_voice_new_view
{
	byte unknown0000[6];
	short voice_count;
	byte unknown0008[0x1a18 - 0x0008];
	s_driver_voice voices[0x40];
	byte unknown2918[0x198];
	IDirectSound *direct_sound;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

extern "C" long __stdcall IDirectSound_CreateSoundBuffer(
	IDirectSound *direct_sound,
	s_buffer_description *description,
	IDirectSoundBuffer **buffer,
	void *unknown);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSoundBuffer_SetMixBins(
	IDirectSoundBuffer *buffer, s_mixbins *mixbins);
extern "C" long __stdcall IDirectSoundBuffer_Play(
	IDirectSoundBuffer *buffer, dword reserved1, dword reserved2, dword flags);
extern "C" long __stdcall IDirectSoundBuffer_SetMaxDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetMinDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);

static_assert(sizeof(s_buffer_description) == 0x18,
	"DirectSound buffer description size");
static_assert(sizeof(s_mixbin_volume_pair) == 0x08,
	"mixbin volume-pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin list size");
static_assert(sizeof(s_driver_voice) == 0x3c, "driver voice size");
static_assert(offsetof(s_driver_voice, buffer) == 0x34,
	"driver voice-new buffer offset");
static_assert(offsetof(s_driver_voice, submix) == 0x38,
	"driver voice-new submix offset");
static_assert(offsetof(s_driver_voice_new_view, voice_count) == 0x06,
	"driver voice-count offset");
static_assert(offsetof(s_driver_voice_new_view, voices) == 0x1a18,
	"driver voice-new table offset");
static_assert(offsetof(s_driver_voice_new_view, direct_sound) == 0x2ab0,
	"driver voice-new DirectSound offset");

// @retail 0x220b90
short sound_driver_voice_new(dword flags, dword input_mixbin)
{
	static const dword k_control_3d = 0x0010;
	static const dword k_effect_input = 0x1000;
	static const dword k_effect_input_2 = 0x2000;
	static const dword k_deferred = 1;
	static const dword k_max_finite_float = 0x7f7fffff;

	s_driver_voice_new_view *globals =
		reinterpret_cast<s_driver_voice_new_view *>(g_51ebe4);
	short voice_index = globals->voice_count;
	if (voice_index >= 0x40)
		return NONE;

	bool three_d = (flags & 1) != 0;
	bool play = (flags & 2) != 0;
	globals->voice_count = voice_index + 1;
	s_driver_voice *voice = &globals->voices[voice_index];

	s_buffer_description description = {};
	description.size = sizeof(description);
	description.flags = k_effect_input_2;
	description.input_mixbin = input_mixbin;
	if (IDirectSound_CreateSoundBuffer(globals->direct_sound, &description,
		&voice->submix, NULL) >= 0)
	{
		s_mixbin_volume_pair pairs[6] =
		{
			{ 0, 0 }, { 1, 0 }, { 2, 0 },
			{ 3, 0 }, { 4, 0 }, { 5, 0 },
		};
		s_mixbins mixbins = { 6, pairs };
		IDirectSoundBuffer_SetVolume(voice->submix, 0);
		IDirectSoundBuffer_SetMixBins(voice->submix, &mixbins);
		IDirectSoundBuffer_Play(voice->submix, 0, 0, 0);
	}

	if (three_d)
	{
		s_buffer_description buffer_description = {};
		buffer_description.size = sizeof(buffer_description);
		buffer_description.flags = k_effect_input | k_control_3d;
		if (play)
			buffer_description.flags += k_effect_input_2;
		buffer_description.input_mixbin = input_mixbin;
		if (IDirectSound_CreateSoundBuffer(globals->direct_sound,
			&buffer_description, &voice->buffer, NULL) < 0)
		{
			globals->voice_count--;
			return NONE;
		}

		s_mixbin_volume_pair pairs[5] =
		{
			{ 6, 0 }, { 8, 0 }, { 7, 0 }, { 9, 0 }, { 10, 0 },
		};
		s_mixbins mixbins = { 5, pairs };
		IDirectSoundBuffer_SetMixBins(voice->buffer, &mixbins);
		voice->unknown00 = input_mixbin;
		real max_distance = *reinterpret_cast<real const *>(
			&k_max_finite_float);
		IDirectSoundBuffer_SetMaxDistance(voice->buffer, max_distance,
			k_deferred);
		IDirectSoundBuffer_SetMinDistance(voice->buffer, max_distance,
			k_deferred);
		if (play)
			IDirectSoundBuffer_Play(voice->buffer, 0, 0, 0);
	}
	else
	{
		voice->buffer = NULL;
	}
	return voice_index;
}

#endif
