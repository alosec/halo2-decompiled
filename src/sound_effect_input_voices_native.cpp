// Exact dormant DSP-image effect-input voice routing isolated from broader
// Xbox voice and effects-image lifecycle code.  Effect state is read through
// the genuine DirectSound boundary and any voice is created by the real driver.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_effect_image_description_view
{
	dword effect_count;
};

struct s_voice_effects
{
	s_effect_image_description_view *description;
	byte unknown04[0x42];
};

struct s_effect_state
{
	dword scratch_offset;
	dword scratch_length;
	dword y_memory_offset;
	dword y_memory_length;
	dword flags;
	dword input_mixbins[10];
};

struct s_effect_mixbin
{
	long effect;
	long parameter;
};

extern s_voice_effects *g_510c90;

extern "C" long __stdcall IDirectSound_GetEffectData(
	IDirectSound *direct_sound,
	dword effect_index,
	dword offset,
	void *data,
	dword data_size);

short sound_driver_voice_new(dword flags, dword input_mixbin);

static const s_effect_mixbin k_effect_mixbins[5] =
{
	{ 1, 1 },
	{ 0xb, 1 },
	{ 0xc, 1 },
	{ 0xd, 1 },
	{ 0xe, 1 },
};

static inline long mixbin_from_address(dword address, dword first, dword last)
{
	long result = NONE;
	dword pinned;

	if (address < first)
		pinned = first;
	else if (address > last)
		pinned = last;
	else
		pinned = address;
	if (pinned == address)
		result = (address - first) >> 5;

	return result;
}

static_assert(sizeof(s_effect_image_description_view) == 0x04,
	"effect image description prefix");
static_assert(offsetof(s_voice_effects, description) == 0x00,
	"voice effects description offset");
static_assert(sizeof(s_effect_state) == 0x3c, "effect state size");
static_assert(offsetof(s_effect_state, input_mixbins) == 0x14,
	"effect state mixbin offset");

// @retail 0x191420
void function_191420(IDirectSound *direct_sound)
{
	if (g_510c90->description->effect_count == 15)
	{
		for (long i = 0; i < 5; i++)
		{
			s_effect_state state;
			IDirectSound_GetEffectData(direct_sound, k_effect_mixbins[i].effect,
				0, &state, sizeof(state));
			dword address = state.input_mixbins[k_effect_mixbins[i].parameter];
			long mixbin = mixbin_from_address(address, 0xc00, 0xfe0);
			if (mixbin == NONE)
				mixbin = mixbin_from_address(address, 0x1400, 0x17e0);
			if (((1 << k_effect_mixbins[i].effect) & 2) && mixbin != NONE)
				sound_driver_voice_new(0, mixbin);
		}
	}
}

#endif
