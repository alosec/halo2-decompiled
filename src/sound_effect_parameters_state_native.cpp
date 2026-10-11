// Source-local native ownership of the recovered mutable sound effect
// parameters. The state has an exact static initializer and remains dormant
// with the sound driver cluster; this does not provide an audio backend.

#include "unknown_11c920.h"
#include <stddef.h>

struct s_sound_effect_parameters
{
	bool dirty;
	byte unknown01[3];
	long room;
	long room_hf;
	long direct;
	long direct_hf;
	real unknown70;
	long unknown74;
	real unknown78;
	long unknown7c;
	real unknown80;
	long unknown84;
	real unknown88;
};

typedef char check_sound_effect_parameters_size[
	sizeof(s_sound_effect_parameters) == 0x30 ? 1 : -1];
typedef char check_sound_effect_parameters_room[
	offsetof(s_sound_effect_parameters, room) == 0x04 ? 1 : -1];
typedef char check_sound_effect_parameters_unknown80[
	offsetof(s_sound_effect_parameters, unknown80) == 0x24 ? 1 : -1];
typedef char check_sound_effect_parameters_unknown88[
	offsetof(s_sound_effect_parameters, unknown88) == 0x2c ? 1 : -1];

s_sound_effect_parameters g_47005c =
{
	true, {0, 0, 0},
	0, 0, 0, 0,
	0.0f, 0, 0.0f, 0, 0.25f, 8000, 0.0f
};
