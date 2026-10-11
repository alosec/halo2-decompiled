// Exact recovered map-reset helpers isolated from Xbox DirectSound headers.
// They preserve real driver effect publication and interface installation;
// none of those backend operations is replaced with synthetic success.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct IDirectSound;
class c_sound_driver_effects;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

struct s_voice_effects
{
	void *description;
	short indices[15];
	word changed;
	word previous_changed;
	byte unknown26[2];
	dword effects[4][2];
};

struct s_sound_driver_map_reset_view
{
	byte unknown0000[0x2a0c];
	dword effect_levels_a[2];
	dword effect_levels_b[2];
	byte effect_settings[0x30];
	long effect_mixbins[3];
	byte unknown2a58[0x58];
	IDirectSound *direct_sound;
	c_sound_driver_effects *effects;
};

struct s_sound_mix_map_view
{
	byte unknown00[0x18];
	dword levels_a[2];
	dword levels_b[2];
	byte settings[0x20];
	real fade_times[3];
};

struct s_sound_system_mix_view
{
	byte unknown00[0x1ec];
	real master_fade_times[3];
};

struct s_globals_sound_mix_view
{
	byte unknown00[0x14];
	long sound_mix_index;
};

extern s_voice_effects *g_510c90;
struct s_4e6380;
extern s_4e6380 *g_4e6380;

void function_21e3b0(c_sound_driver_effects *effects);
extern "C" long __stdcall IDirectSound_SetEffectData(
	IDirectSound *direct_sound,
	dword effect_index,
	dword offset,
	void const *data,
	dword data_size,
	dword apply);

static_assert(sizeof(s_voice_effects) == 0x48, "voice effects size");
static_assert(offsetof(s_voice_effects, changed) == 0x22,
	"voice effects changed offset");
static_assert(offsetof(s_voice_effects, previous_changed) == 0x24,
	"voice effects previous-changed offset");
static_assert(offsetof(s_voice_effects, effects) == 0x28,
	"voice effects data offset");
static_assert(offsetof(s_sound_driver_map_reset_view, effect_levels_a) ==
	0x2a0c, "driver effect-level A offset");
static_assert(offsetof(s_sound_driver_map_reset_view, effect_levels_b) ==
	0x2a14, "driver effect-level B offset");
static_assert(offsetof(s_sound_driver_map_reset_view, effect_settings) ==
	0x2a1c, "driver effect-settings offset");
static_assert(offsetof(s_sound_driver_map_reset_view, direct_sound) == 0x2ab0,
	"driver DirectSound offset");
static_assert(offsetof(s_sound_driver_map_reset_view, effects) == 0x2ab4,
	"driver effects-interface offset");
static_assert(sizeof(s_sound_mix_map_view) == 0x54,
	"sound mix tag view size");
static_assert(offsetof(s_sound_system_mix_view, master_fade_times) == 0x1ec,
	"sound master-fade times offset");

// @retail 0x191270
void function_191270(void)
{
	s_sound_driver_map_reset_view *driver =
		reinterpret_cast<s_sound_driver_map_reset_view *>(g_51ebe4);
	for (dword i = 0; i < 4; i++)
	{
		IDirectSound_SetEffectData(driver->direct_sound, i + 4, 0x20,
			g_510c90->effects[i], sizeof(g_510c90->effects[i]), 0);
	}
	if (g_510c90)
	{
		g_510c90->previous_changed = 0;
		g_510c90->changed = 0;
	}
}

// @retail 0x21f6d0
void __stdcall function_21f6d0(dword const *levels_a,
	dword const *levels_b, void const *settings)
{
	s_sound_driver_map_reset_view *driver =
		reinterpret_cast<s_sound_driver_map_reset_view *>(g_51ebe4);
	driver->effect_levels_a[0] = levels_a[0];
	driver->effect_levels_a[1] = levels_a[1];
	driver->effect_levels_b[0] = levels_b[0];
	driver->effect_levels_b[1] = levels_b[1];
	memcpy(driver->effect_settings, settings, sizeof(driver->effect_settings));
	function_21e3b0(driver->effects);
}

// @retail 0x125df0
void sound_mix_apply(void)
{
	s_tag_header_globals *globals = g_4e034c;
	s_globals_sound_mix_view *header =
		reinterpret_cast<s_globals_sound_mix_view *>(
			globals->header ? globals->header_alt : NULL);
	long sound_mix_index = header->sound_mix_index;
	if (sound_mix_index != NONE)
	{
		s_sound_mix_map_view *sound_mix =
			reinterpret_cast<s_sound_mix_map_view *>(
				g_4e3b44[sound_mix_index & 0xffff].bytes);
		function_21f6d0(sound_mix->levels_a, sound_mix->levels_b,
			sound_mix->settings);
		s_sound_system_mix_view *sound_system =
			reinterpret_cast<s_sound_system_mix_view *>(g_4e6380);
		sound_system->master_fade_times[0] = sound_mix->fade_times[0];
		sound_system->master_fade_times[1] = sound_mix->fade_times[1];
		sound_system->master_fade_times[2] = sound_mix->fade_times[2];
	}
}

#endif
