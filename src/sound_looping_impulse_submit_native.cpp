// Exact looping impulse-buffer submission isolated from the broader Xbox
// DirectSound driver.  The live buffer operations remain genuine imports;
// this does not fabricate a buffer, lock, waveform, or playback progress.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSoundBuffer;
struct s_tag_data;
struct s_looping_impulse_parameters;

namespace
{
struct s_looping_impulse_view
{
	short index;
	short mixbin;
	byte unknown04[4];
	s_tag_data const *function;
	long identifier;
};

struct s_driver_impulse_view
{
	byte unknown0000[0x2a90];
	IDirectSoundBuffer *impulse_buffers[2];
	long impulse_ids[2];
	byte unknown2aa0[0x38];
};

static_assert(sizeof(s_looping_impulse_view) == 0x10,
	"looping impulse parameter size");
static_assert(offsetof(s_looping_impulse_view, function) == 0x8,
	"looping impulse function offset");
static_assert(offsetof(s_looping_impulse_view, identifier) == 0xc,
	"looping impulse identifier offset");
static_assert(sizeof(s_driver_impulse_view) == 0x2ad8,
	"sound driver globals size");
static_assert(offsetof(s_driver_impulse_view, impulse_buffers) == 0x2a90,
	"driver impulse buffer offset");
static_assert(offsetof(s_driver_impulse_view, impulse_ids) == 0x2a98,
	"driver impulse identifier offset");

long real_to_nearest_even(real value)
{
	/* Retail fistp uses nearest-even under the default x87 control word. */
	double converted = value;
	long truncated = static_cast<long>(converted);
	double fraction = converted - static_cast<double>(truncated);
	if (fraction > 0.5 || (fraction == 0.5 && (truncated & 1)))
		++truncated;
	else if (fraction < -0.5 || (fraction == -0.5 && (truncated & 1)))
		--truncated;
	return truncated;
}
}

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

real function_13bb90(s_tag_data const *function, real input, real range);

extern "C" long __stdcall IDirectSoundBuffer_Pause(
	IDirectSoundBuffer *buffer, dword pause);
extern "C" long __stdcall IDirectSoundBuffer_Lock(
	IDirectSoundBuffer *buffer, dword offset, dword bytes,
	void **data, dword *size, void **data2, dword *size2, dword flags);
extern "C" long __stdcall IDirectSoundBuffer_SetCurrentPosition(
	IDirectSoundBuffer *buffer, dword position);
extern "C" long __stdcall IDirectSoundBuffer_SetVolume(
	IDirectSoundBuffer *buffer, long volume);

// @retail 0x21f720
void __stdcall function_21f720(
	s_looping_impulse_parameters const *parameters_)
{
	s_looping_impulse_view const *parameters =
		reinterpret_cast<s_looping_impulse_view const *>(parameters_);
	if (parameters->mixbin >= 0 && parameters->mixbin <= 31 &&
		parameters->index >= 0 && parameters->index <= 1)
	{
		s_driver_impulse_view *globals =
			reinterpret_cast<s_driver_impulse_view *>(g_51ebe4);
		long index = parameters->index;
		IDirectSoundBuffer *buffer = globals->impulse_buffers[index];
		if (parameters->identifier != globals->impulse_ids[index])
		{
			byte *data;
			dword size;
			/* Retail/Xbox DSBPAUSE_PAUSE and DSBPAUSE_RESUME are 1 and 0. */
			IDirectSoundBuffer_Pause(buffer, 1);
			IDirectSoundBuffer_Lock(buffer, 0, 1000,
				reinterpret_cast<void **>(&data), &size, NULL, NULL, 0);
			for (dword i = 0; i < size; i++)
			{
				real value = function_13bb90(parameters->function,
					static_cast<real>(i) * 0.001f, 0.0f);
				long sample = real_to_nearest_even(value * 255.0f);
				/* Retail narrows before its ineffective >0xff comparison. */
				data[i] = static_cast<byte>(sample);
			}
			IDirectSoundBuffer_Pause(buffer, 0);
			globals->impulse_ids[index] = parameters->identifier;
		}
		IDirectSoundBuffer_SetCurrentPosition(buffer, 0);
		IDirectSoundBuffer_SetVolume(buffer, 0);
	}
}

#endif
