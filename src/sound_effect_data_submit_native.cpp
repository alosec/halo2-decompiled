// Exact packed effect-data submission isolated from the broad recovered Xbox
// DirectSound unit.  High-level and DSP-state writes remain genuine XAudio
// and DirectSound operations against the downloaded effect image.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_sound_effect_data
{
	dword effect_mask;
	byte flags;
	byte unknown05[3];
	word offset;
	word size;
	byte data[4];
};

struct s_effect_map
{
	byte unknown00[0x10];
	byte *state_segment;
	dword state_size;
	byte unknown18[8];
};

struct s_effect_image_description
{
	s_effect_map maps[32];
};

struct s_voice_effects
{
	s_effect_image_description *description;
	short indices[15];
	word changed;
	word previous_changed;
	byte unknown26[2];
	dword effects[4][2];
};

struct s_sound_driver_effect_submit_view
{
	byte unknown0000[0x2ab0];
	IDirectSound *direct_sound;
};

static inline s_sound_effect_data const *effect_data_at(
	dword const *buffer, long index)
{
	return reinterpret_cast<s_sound_effect_data const *>(&buffer[index + 1]);
}

static_assert(sizeof(s_sound_effect_data) == 0x10,
	"packed sound effect data header size");
static_assert(offsetof(s_sound_effect_data, flags) == 0x04,
	"packed sound effect flags offset");
static_assert(offsetof(s_sound_effect_data, offset) == 0x08,
	"packed sound effect state offset");
static_assert(offsetof(s_sound_effect_data, size) == 0x0a,
	"packed sound effect size offset");
static_assert(offsetof(s_sound_effect_data, data) == 0x0c,
	"packed sound effect payload offset");
static_assert(sizeof(s_effect_map) == 0x20, "effect map size");
static_assert(offsetof(s_effect_map, state_segment) == 0x10,
	"effect map state segment offset");
static_assert(offsetof(s_effect_map, state_size) == 0x14,
	"effect map state size offset");
static_assert(sizeof(s_voice_effects) == 0x48, "voice effects size");
static_assert(offsetof(s_voice_effects, changed) == 0x22,
	"voice effects changed offset");
static_assert(offsetof(s_sound_driver_effect_submit_view, direct_sound) == 0x2ab0,
	"sound driver DirectSound offset");

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;
extern s_voice_effects *g_510c90;

extern "C" long __stdcall XAudioSetEffectData(
	long effect_index, void const *description, void *unknown);
extern "C" long __stdcall IDirectSound_SetEffectData(
	IDirectSound *direct_sound,
	dword effect_index,
	dword offset,
	void const *data,
	dword data_size,
	dword apply);

// @retail 0x21f960
void function_21f960(dword size, dword const *buffer)
{
	if (!buffer || !size)
		return;

	long word_count = static_cast<long>((size - 4) >> 2);
	for (long i = 0; i < word_count;)
	{
		s_sound_effect_data const *effect = effect_data_at(buffer, i);
		if (!(effect->flags & 2))
		{
			for (long effect_index = 0; effect_index < 32; effect_index++)
			{
				dword bit = 1u << effect_index;
				if (effect->effect_mask < bit)
					break;
				if (!(effect->effect_mask & bit))
					continue;

				if (effect->flags & 1)
				{
					XAudioSetEffectData(effect_index, effect->data, NULL);
				}
				else
				{
					s_effect_map const *map =
						&g_510c90->description->maps[effect_index];
					dword offset = effect->offset;
					dword data_size = effect->size;
					if (offset + data_size <= map->state_size * 4 &&
						memcmp(effect->data, map->state_segment + offset,
							data_size) != 0)
					{
						s_sound_driver_effect_submit_view *driver =
							reinterpret_cast<s_sound_driver_effect_submit_view *>(
								g_51ebe4);
						IDirectSound_SetEffectData(driver->direct_sound,
							effect_index, offset, effect->data, data_size, 0);
					}
				}
			}
			g_510c90->changed |= static_cast<word>(effect->effect_mask);
		}
		i += (effect->size + 0xf) >> 2;
	}
}

#endif
