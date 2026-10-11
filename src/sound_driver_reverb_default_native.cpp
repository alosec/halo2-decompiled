// ARM-safe ownership of the recovered no-environment listener reverb.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

struct s_sound_driver_reverb
{
	byte unknown00[8];
	real room;
	real room_hf;
	real room_rolloff;
	real decay_time;
	real decay_hf_ratio;
	real reflections;
	real reflections_delay;
	real reverb;
	real reverb_delay;
	real diffusion;
	real density;
	real hf_reference;
	byte unknown38[0x10];
};

s_sound_driver_reverb g_44a2a0 =
{
	{ 0, 0, 0, 0, 0, 0x80, 0, 0 },
	-100.0f, -100.0f, 0.0f, 1.0f, 1.0f, -100.0f,
	0.0f, -100.0f, 0.0f, 1.0f, 1.0f, 5000.0f,
	{}
};

static_assert(sizeof(s_sound_driver_reverb) == 0x48,
	"driver default reverb size");
static_assert(offsetof(s_sound_driver_reverb, room) == 0x08 &&
	offsetof(s_sound_driver_reverb, hf_reference) == 0x34,
	"driver default reverb fields");

#endif
