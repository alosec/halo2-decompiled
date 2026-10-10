// Exact Microsoft-ABI reconstruction of the compact sound-driver effects
// installation policy.  The DirectSound effects implementation and its
// mutable buffers remain genuine external lifetime boundaries.

#include "unknown_11c920.h"
#include <stddef.h>

class c_sound_driver_effects
{
public:
	virtual void initialize(void *settings, void *buffers, bool rear) = 0;
	virtual void dispose() = 0;
	virtual void set_i3dl2(void *parameters, long unused) = 0;
};

struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

struct s_sound_effect_parameters
{
	bool dirty;
	byte padding01[3];
	long room;
};
extern s_sound_effect_parameters g_47005c;

struct s_sound_driver_effects_view
{
	byte padding0000[0x2a0c];
	dword effect_levels_a[2];
	byte padding2a14[0x2a4c - 0x2a14];
	long effect_mixbins[3];
	byte padding2a58[0x2ab4 - 0x2a58];
	c_sound_driver_effects *effects;
};

// @retail 0x21e3b0
void function_21e3b0(c_sound_driver_effects *effects)
{
	s_sound_driver_effects_view *globals =
		(s_sound_driver_effects_view *)g_51ebe4;

	if (globals->effects)
	{
		globals->effects->dispose();
		globals = (s_sound_driver_effects_view *)g_51ebe4;
	}
	globals->effects = effects;
	effects->initialize(
		globals->effect_levels_a,
		globals->effect_mixbins,
		*((bool *)globals + 1));
	((s_sound_driver_effects_view *)g_51ebe4)->effects->set_i3dl2(
		&g_47005c.room,
		*((bool *)g_51ebe4 + 1));
}

static_assert(offsetof(s_sound_driver_effects_view, effect_levels_a) == 0x2a0c,
	"sound driver effect levels offset changed");
static_assert(offsetof(s_sound_driver_effects_view, effect_mixbins) == 0x2a4c,
	"sound driver effect mixbins offset changed");
static_assert(offsetof(s_sound_driver_effects_view, effects) == 0x2ab4,
	"sound driver effects pointer offset changed");
static_assert(offsetof(s_sound_effect_parameters, room) == 4,
	"sound effect room parameter offset changed");
