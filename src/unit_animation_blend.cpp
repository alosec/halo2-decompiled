// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNIT_ANIMATION_BLEND.CPP: setting a unit's channel blend (unit_animation_access.h,
   s_unit_animation_channel_blend) by a channel's name. The blend itself is in
   unknown_10dc70.cpp.

   This file does not include unit_animation_access.h: with globals.h in one more
   source linked before unknown_058dd0.cpp, the full build allocates
   0x73360's registers differently. */

#include "unknown_11c920.h"

void function_10e9f0(long object_index, short channel, real value, real time);

/* the names of the blend's 13 channels (retail 0x4408bc) */
long const g_unit_animation_blend_channel_names[13] = { 0x06000084, 0x070001a4, 0x050001a5, 0x0b0001a6, 0x080001a7, 0x0c0001a8, 0x070001a9, 0x060001aa, 0x080001ab, 0x070001ac, 0x050001ad, 0x070001ae, 0x040001af };

/* blends toward the named channel (no channel for 0x7000001 or an unknown
   name) at a weight over a time */
// @retail 0x10e9a0
void unit_animation_channel_blend_set_by_name(long unit_index, long name, real weight, real seconds)
{
	if (name == 0x7000001)
	{
		function_10e9f0(unit_index, NONE, weight, seconds);
		return;
	}
	for (long channel = 0; channel < 13; channel++)
	{
		if (name == g_unit_animation_blend_channel_names[channel])
		{
			function_10e9f0(unit_index, (short)channel, weight, seconds);
			return;
		}
	}
	function_10e9f0(unit_index, NONE, weight, seconds);
}
