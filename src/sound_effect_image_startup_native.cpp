// Exact dormant DSP effect-image startup isolated from voice-engine and Xbox
// reload code.  Section loading, image download, and effect-data reads remain
// genuine platform/DirectSound boundaries.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;

struct s_effect_image_location
{
	dword i3dl2_reverb_index;
	dword crosstalk_index;
};

struct s_voice_effects
{
	void *description;
	short indices[15];
	word changed;
	word previous_changed;
	byte unknown26[2];
	dword effects[4][2];
};

extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;
extern s_voice_effects *g_510c90;

void function_163ba0(dword *checksum, void *data, long data_size);

extern "C" void *__stdcall XLoadSection(char const *name);
extern "C" void __stdcall XFreeSection(char const *name);
extern "C" long __stdcall XAudioDownloadEffectsImage(
	char const *name,
	s_effect_image_location *location,
	dword flags,
	void **description);
extern "C" long __stdcall IDirectSound_GetEffectData(
	IDirectSound *direct_sound,
	dword effect_index,
	dword offset,
	void *data,
	dword data_size);

static_assert(sizeof(s_effect_image_location) == 0x08,
	"effect image location size");
static_assert(sizeof(s_voice_effects) == 0x48, "voice effects size");
static_assert(offsetof(s_voice_effects, indices) == 0x04,
	"voice effects indices offset");
static_assert(offsetof(s_voice_effects, changed) == 0x22,
	"voice effects changed offset");
static_assert(offsetof(s_voice_effects, previous_changed) == 0x24,
	"voice effects previous-changed offset");
static_assert(offsetof(s_voice_effects, effects) == 0x28,
	"voice effects data offset");

// @retail 0x191300
bool function_191300(IDirectSound *direct_sound)
{
	char effect_image_name[9];
	effect_image_name[0] = 'D';
	effect_image_name[1] = 'S';
	effect_image_name[2] = 'P';
	effect_image_name[3] = 'I';
	effect_image_name[4] = 'm';
	effect_image_name[5] = 'a';
	effect_image_name[6] = 'g';
	effect_image_name[7] = 'e';
	effect_image_name[8] = 0;
	static const dword k_download_xbe_section = 1;
	bool result = false;

	if (XLoadSection(effect_image_name))
	{
		byte *top = reinterpret_cast<byte *>(g_510800_pool_base) +
			g_510804_pool_size;
		s_voice_effects *effects = reinterpret_cast<s_voice_effects *>(
			(reinterpret_cast<dword>(top) + 3) & ~3);
		long aligned_size = reinterpret_cast<byte *>(effects) - top +
			sizeof(s_voice_effects);
		g_510804_pool_size += aligned_size;
		function_163ba0(&g_510808_pool_checksum, &aligned_size,
			sizeof(aligned_size));

		memset(effects->indices, 0xff, sizeof(effects->indices));
		effects->changed = 0;
		effects->previous_changed = 0;
		g_510c90 = effects;

		s_effect_image_location location = { 9, 10 };
		if (XAudioDownloadEffectsImage(effect_image_name, &location,
			k_download_xbe_section, &effects->description) >= 0)
		{
			result = true;
		}
		XFreeSection(effect_image_name);

		if (result)
		{
			for (dword i = 0; i < 4; i++)
			{
				IDirectSound_GetEffectData(direct_sound, i + 4, 0x20,
					g_510c90->effects[i], sizeof(g_510c90->effects[i]));
			}
		}
	}
	return result;
}

#endif
