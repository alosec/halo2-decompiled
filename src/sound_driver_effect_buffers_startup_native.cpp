// Exact dormant effect/submix/impulse-buffer and defaults phase isolated from
// the broader sound-driver startup.  Every created buffer remains a genuine
// DirectSound object; creation failures retain null entries for ordered
// teardown rather than being replaced with synthetic success.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct IDirectSoundBuffer;
class c_sound_driver_effects;
class c_sound_effects;

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

struct s_driver_effect_buffers_view
{
	byte unknown0000[0x2a0c];
	dword effect_levels_a[2];
	dword effect_levels_b[2];
	byte effect_settings[0x30];
	long effect_mixbins[3];
	IDirectSoundBuffer *effect_buffers_a[3];
	IDirectSoundBuffer *effect_buffers_b[3];
	IDirectSoundBuffer *submix_buffers[8];
	IDirectSoundBuffer *impulse_buffers[2];
	long impulse_ids[2];
	byte caps[0x10];
	IDirectSound *direct_sound;
	c_sound_driver_effects *effects;
	byte unknown2ab8[4];
	real volume_a;
	real volume_b;
	real volume_c;
	real volume_d;
	long unknown2acc;
	real unknown2ad0;
	dword unknown2ad4;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern c_sound_effects g_47f088;

void function_21e3b0(c_sound_driver_effects *effects);
void function_221490(IDirectSoundBuffer **buffer_reference);

extern "C" long __stdcall IDirectSound_CreateSoundBuffer(
	IDirectSound *direct_sound,
	s_buffer_description *description,
	IDirectSoundBuffer **buffer,
	void *unknown);
extern "C" long __stdcall IDirectSoundBuffer_Play(
	IDirectSoundBuffer *buffer, dword reserved1, dword reserved2, dword flags);
extern "C" long __stdcall IDirectSoundBuffer_SetMaxDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetMinDistance(
	IDirectSoundBuffer *buffer, real distance, dword apply);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);
extern "C" long __stdcall IDirectSoundBuffer_SetMixBins(
	IDirectSoundBuffer *buffer, s_mixbins *mixbins);

static_assert(sizeof(s_buffer_description) == 0x18,
	"DirectSound buffer description size");
static_assert(sizeof(s_mixbin_volume_pair) == 0x08,
	"mixbin volume-pair size");
static_assert(sizeof(s_mixbins) == 0x08, "mixbin list size");
static_assert(sizeof(s_driver_effect_buffers_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_effect_buffers_view, effect_levels_a) == 0x2a0c,
	"effect level A offset");
static_assert(offsetof(s_driver_effect_buffers_view, effect_settings) == 0x2a1c,
	"effect settings offset");
static_assert(offsetof(s_driver_effect_buffers_view, effect_mixbins) == 0x2a4c,
	"effect mixbin offset");
static_assert(offsetof(s_driver_effect_buffers_view, effect_buffers_a) == 0x2a58,
	"effect buffer A offset");
static_assert(offsetof(s_driver_effect_buffers_view, effect_buffers_b) == 0x2a64,
	"effect buffer B offset");
static_assert(offsetof(s_driver_effect_buffers_view, submix_buffers) == 0x2a70,
	"submix buffer offset");
static_assert(offsetof(s_driver_effect_buffers_view, impulse_buffers) == 0x2a90,
	"impulse buffer offset");
static_assert(offsetof(s_driver_effect_buffers_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");
static_assert(offsetof(s_driver_effect_buffers_view, volume_a) == 0x2abc,
	"driver volume offset");

// Source-local exact phase of retail function_21e4b0.
void sound_driver_effect_buffers_startup(void)
{
	static const dword k_effect_input = 0x2000;
	static const dword k_control_3d = 0x10;
	static const dword k_deferred = 1;
	static const dword k_max_finite_float = 0x7f7fffff;

	s_driver_effect_buffers_view *globals =
		reinterpret_cast<s_driver_effect_buffers_view *>(g_51ebe4);
	globals->effect_levels_a[0] = 0;
	globals->effect_levels_a[1] = 0xc0c0a8c1;
	globals->effect_levels_b[0] = 0;
	globals->effect_levels_b[1] = 0xc0c0a8c1;
	reinterpret_cast<dword *>(globals->effect_settings)[0] = 0xc0c0a8c1;
	reinterpret_cast<dword *>(globals->effect_settings)[1] = 0xc0400000;
	reinterpret_cast<dword *>(globals->effect_settings)[2] = 0xc0c0a8c1;
	reinterpret_cast<dword *>(globals->effect_settings)[7] = 0xc0c0a8c1;
	globals->effect_mixbins[0] = 23;
	globals->effect_mixbins[1] = 24;
	globals->effect_mixbins[2] = 25;

	for (long send = 0; send < 3; send++)
	{
		long mixbin = globals->effect_mixbins[send];
		IDirectSoundBuffer **buffer = &globals->effect_buffers_a[send];
		IDirectSoundBuffer **spatial_buffer = &globals->effect_buffers_b[send];
		s_buffer_description description = {};
		description.size = sizeof(description);
		description.flags = k_effect_input;
		description.input_mixbin = mixbin;
		if (IDirectSound_CreateSoundBuffer(globals->direct_sound,
			&description, buffer, NULL) >= 0)
		{
			IDirectSoundBuffer_Play(*buffer, 0, 0, 0);
		}

		s_buffer_description spatial_description = {};
		spatial_description.size = sizeof(spatial_description);
		spatial_description.flags = k_effect_input | k_control_3d;
		spatial_description.input_mixbin = mixbin;
		if (IDirectSound_CreateSoundBuffer(globals->direct_sound,
			&spatial_description, spatial_buffer, NULL) >= 0)
		{
			real max_distance = *reinterpret_cast<real const *>(
				&k_max_finite_float);
			IDirectSoundBuffer_SetMaxDistance(*spatial_buffer, max_distance,
				k_deferred);
			IDirectSoundBuffer_SetMinDistance(*spatial_buffer, max_distance,
				k_deferred);
			IDirectSoundBuffer_Play(*spatial_buffer, 0, 0, 0);
		}
	}

	function_21e3b0(reinterpret_cast<c_sound_driver_effects *>(&g_47f088));
	for (long group = 0; group < 2; group++)
	{
		for (long i = 0; i < 4; i++)
		{
			IDirectSoundBuffer **buffer =
				&globals->submix_buffers[group * 4 + i];
			s_buffer_description description = {};
			description.size = sizeof(description);
			description.flags = k_effect_input;
			description.input_mixbin = 15 + group * 4 + i;
			if (IDirectSound_CreateSoundBuffer(globals->direct_sound,
				&description, buffer, NULL) >= 0)
			{
				IDirectSoundBuffer_SetVolume(*buffer, 0);
				s_mixbin_volume_pair pair;
				s_mixbins bins;
				pair.mixbin = 6 + i;
				pair.volume = 0;
				bins.count = 1;
				bins.pairs = &pair;
				IDirectSoundBuffer_SetMixBins(*buffer, &bins);
				IDirectSoundBuffer_Play(*buffer, 0, 0, 0);
			}
		}
	}

	for (long impulse = 0; impulse < 2; impulse++)
	{
		function_221490(&globals->impulse_buffers[impulse]);
		globals->impulse_ids[impulse] = NONE;
	}
	globals->volume_a = 1.0f;
	globals->volume_b = 1.0f;
	globals->volume_c = 1.0f;
	globals->volume_d = 1.0f;
	globals->unknown2acc = 8000;
	globals->unknown2ad0 = 0.0f;
}

#endif
