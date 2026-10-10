// Exact dormant ordinary sound-driver voice-buffer construction isolated from
// the broader Xbox driver source.  Device creation and every 3D/mixbin update
// remain genuine DirectSound operations.

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

struct s_driver_globals_view
{
	byte unknown0000[0x1a18];
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
extern "C" long __stdcall IDirectSoundBuffer_SetMixBins(
	IDirectSoundBuffer *buffer, s_mixbins *mixbins);
extern "C" long __stdcall IDirectSoundBuffer_SetMaxDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetMinDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetRolloffFactor(
	IDirectSoundBuffer *buffer, real factor, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetDopplerFactor(
	IDirectSoundBuffer *buffer, real factor, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetConeOutsideVolume(
	IDirectSoundBuffer *buffer, long volume, dword apply);

static_assert(sizeof(s_buffer_description) == 0x18,
	"DirectSound buffer description size");
static_assert(sizeof(s_mixbin_volume_pair) == 0x08,
	"mixbin volume-pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin list size");
static_assert(sizeof(s_driver_voice) == 0x3c, "driver voice size");
static_assert(offsetof(s_driver_voice, buffer) == 0x34,
	"driver voice buffer offset");
static_assert(offsetof(s_driver_voice, submix) == 0x38,
	"driver voice submix offset");
static_assert(offsetof(s_driver_globals_view, voices) == 0x1a18,
	"driver voice table offset");
static_assert(offsetof(s_driver_globals_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");

// @retail 0x220a70
bool sound_driver_voice_create_buffer(long voice_index)
{
	static const dword k_voice_buffer_flags = 0x2010;
	static const dword k_deferred = 1;
	static const dword k_max_finite_float = 0x7f7fffff;

	s_driver_globals_view *globals =
		reinterpret_cast<s_driver_globals_view *>(g_51ebe4);
	s_driver_voice *voice = &globals->voices[voice_index];
	s_buffer_description description = {};

	description.size = sizeof(description);
	description.flags = k_voice_buffer_flags;
	bool result = IDirectSound_CreateSoundBuffer(globals->direct_sound,
		&description, &voice->buffer, NULL) >= 0;
	if (result)
	{
		s_mixbin_volume_pair pairs[5] =
		{
			{ 6, 0 },
			{ 8, 0 },
			{ 7, 0 },
			{ 9, 0 },
			{ 10, 0 },
		};
		s_mixbins mixbins = { 5, pairs };
		IDirectSoundBuffer_SetMixBins(voice->buffer, &mixbins);
		voice->unknown00 = 0x1f;
		real max_distance = *reinterpret_cast<real const *>(
			&k_max_finite_float);
		IDirectSoundBuffer_SetMaxDistance(voice->buffer, max_distance,
			k_deferred);
		IDirectSoundBuffer_SetMinDistance(voice->buffer, max_distance,
			k_deferred);
		IDirectSoundBuffer_SetRolloffFactor(voice->buffer, 0.0f,
			k_deferred);
		IDirectSoundBuffer_SetDopplerFactor(voice->buffer, 0.0f,
			k_deferred);
		IDirectSoundBuffer_SetConeOutsideVolume(voice->buffer, 0,
			k_deferred);
	}
	voice->submix = NULL;
	return result;
}

#endif
