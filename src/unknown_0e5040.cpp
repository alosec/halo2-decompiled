// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E5040.CPP: a biped's footstep frame events (the animation event
   callback 0xbf600 hands it the frame events of a biped's animation) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cafc0.h"

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif

/* the biped fields read here (see s_biped in bipeds.cpp) */
struct s_footstep_biped
{
	byte unknown000[0x88];
	vector3f linear_velocity;
	byte unknown094[0x12a - 0x94];
	short animation_state_offset;
	byte unknown12c[0x348 - 0x12c];
	word flags_348;
	byte unknown34a[0x39f - 0x34a];
	byte footstep_scale;
};

struct s_footstep_biped_header
{
	byte unknown00[8];
	s_footstep_biped *biped;
};

#define FOOTSTEP_BIPED_GET(index) (((s_footstep_biped_header *)g_4e0300->data)[(index) & 0xffff].biped)

/* s_animation_state::blend_fraction_get's body (0x1cdf00), which retail
   inlines here */
PRIVATE __forceinline real footstep_blend_fraction(real blend)
{
	real fraction = PIN(blend + 0.0001f, 0.0f, 0.9999f) * 1.0002f;

	return PIN(fraction, 0.0f, 1.0f);
}

void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long marker);

/* plays the footstep of a frame event: types 2 and 3 are the feet (markers
   0 and 1), 9 and 10 other footsteps at marker 2. A slow biped (moving at
   most 1.5 a second, or mostly blended out of its animation) plays kind 0.

   local_header is never read. With it the build reads g_4e0300's data
   pointer and the biped again after the animation state test (0xe5063), as
   retail does; without it the second lookup reuses the first. */
// @retail 0xe5040
void __stdcall function_e5040(long arg_159e6d_4, s_animation_frame_event const *event)
{
	s_footstep_biped *local_biped = FOOTSTEP_BIPED_GET(arg_159e6d_4);
	s_footstep_biped_header *local_header = &((s_footstep_biped_header *)g_4e0300->data)[arg_159e6d_4 & 0xffff];

	if (local_biped->animation_state_offset != NONE)
	{
		s_footstep_biped *biped = FOOTSTEP_BIPED_GET(arg_159e6d_4);
		vector3f const *velocity = &biped->linear_velocity;
		s_animation_state *state = (s_animation_state *)((byte *)biped + biped->animation_state_offset);
		bool slow = velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k <= 2.25f;
		real scale = 1.0f;
		long kind;
		long value;

		if (state->channels[1].graph_tag_index != NONE && state->channels[1].animation_id.index != NONE)
		{
			scale = 1.0f - footstep_blend_fraction(state->unknown80);
			scale = PIN(scale, 0.0f, 1.0f);
			if (scale < 0.5f)
			{
				slow = true;
			}
		}
		value = (long)(scale * 127.0f);
		biped->footstep_scale = (byte)PIN(value, 0, 0x7f);
		if (!slow)
		{
			kind = TEST_FIELD_BIT((biped->flags_348 >> 4) & 1) ? 6 : 1;
		}
		else
		{
			kind = 0;
		}
		switch (event->type)
		{
		case 2:
			function_e4d90(arg_159e6d_4, kind, scale, 0);
			break;
		case 3:
			function_e4d90(arg_159e6d_4, kind, scale, 1);
			break;
		case 9:
			function_e4d90(arg_159e6d_4, 3, scale, 2);
			break;
		case 10:
			function_e4d90(arg_159e6d_4, 7, scale, 2);
			break;
		}
	}
}
