// Exact dormant sound-driver effect refresh and processor-panic recovery
// isolated from Xbox voice-chat headers.  DirectSound effect submission, DSP
// image loading, and voice-engine restart remain genuine runtime boundaries.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

struct s_effect_image_location
{
	dword i3dl2_reverb_index;
	dword crosstalk_index;
};

struct s_driver_effect_refresh_view
{
	byte unknown0000[0x2ab0];
	IDirectSound *direct_sound;
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

struct s_voice_globals
{
	byte unknown00[0xee];
	word unknownEE;
	byte unknownF0[0x50];
	bool initialized;
	byte unknown141[3];
};

class c_voice_xhv
{
public:
	dword vtable;
	bool initialized;
};

extern s_voice_effects *g_510c90;
extern s_voice_globals g_4c9878;
extern c_voice_xhv g_476fc8;

void voice_xhv_dispose(c_voice_xhv *xhv);
void voice_start_engine(void);

extern "C" void *__stdcall XLoadSection(char const *name);
extern "C" void __stdcall XFreeSection(char const *name);
extern "C" long __stdcall XAudioDownloadEffectsImage(
	char const *name,
	s_effect_image_location *location,
	dword flags,
	void **description);
extern "C" long __stdcall IDirectSound_SetEffectData(
	IDirectSound *direct_sound,
	dword effect_index,
	dword offset,
	void const *data,
	dword data_size,
	dword apply);

static_assert(sizeof(s_effect_image_location) == 0x08,
	"effect image location size");
static_assert(offsetof(s_driver_effect_refresh_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");
static_assert(sizeof(s_voice_effects) == 0x48, "voice effects size");
static_assert(offsetof(s_voice_effects, changed) == 0x22,
	"voice effects changed offset");
static_assert(offsetof(s_voice_effects, previous_changed) == 0x24,
	"voice effects previous-changed offset");
static_assert(offsetof(s_voice_effects, effects) == 0x28,
	"voice effects data offset");
static_assert(sizeof(s_voice_globals) == 0x144, "voice globals size");
static_assert(offsetof(s_voice_globals, unknownEE) == 0xee,
	"voice globals processor state offset");
static_assert(offsetof(s_voice_globals, initialized) == 0x140,
	"voice globals initialized offset");
static_assert(offsetof(c_voice_xhv, initialized) == 0x04,
	"voice XHV initialized offset");

static const dword k_dsfx_immediate = 0;
static const dword k_download_xbe_section = 1;

// @retail 0x191550
void function_191550(void)
{
	dword changed = g_510c90->previous_changed & ~g_510c90->changed;
	s_driver_effect_refresh_view *driver =
		reinterpret_cast<s_driver_effect_refresh_view *>(g_51ebe4);
	IDirectSound *direct_sound = driver->direct_sound;

	for (dword i = 0; i < 4; i++)
	{
		if (changed & (0x10 << i))
		{
			IDirectSound_SetEffectData(direct_sound, i + 4, 0x20,
				g_510c90->effects[i], sizeof(g_510c90->effects[i]),
				k_dsfx_immediate);
		}
	}
	g_510c90->previous_changed = g_510c90->changed;
	g_510c90->changed = 0;
}

// @retail 0x1915f0
void function_1915f0(void)
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

	if (XLoadSection(effect_image_name))
	{
		s_effect_image_location location = { 9, 10 };
		if (g_4c9878.initialized && g_476fc8.initialized)
		{
			voice_xhv_dispose(&g_476fc8);
			g_4c9878.unknownEE = 0;
		}
		XAudioDownloadEffectsImage(effect_image_name, &location,
			k_download_xbe_section, &g_510c90->description);
		voice_start_engine();
		XFreeSection(effect_image_name);
	}
}

#endif
