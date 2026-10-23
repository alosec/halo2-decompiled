// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_20B450.CPP: setting the five-bit timer a sound keeps after its
   permutation bits (read and counted down in src/unknown_20b3c0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"

/* a sound tag's pitch ranges, as the permutation choice reads them */
struct s_sound_timer_definition
{
	byte unknown00[8];
	short first_pitch_range_index;
	char pitch_range_count;
};

struct s_animation_state;
struct s_animation_ref;
long function_219430(s_animation_state *state, s_animation_ref *ref);
long log2_ceiling_plus_one(dword value);

extern byte *g_51e9ec;

#define TIMER_BIT_SET(vector, bit, value) ((value) ? (((dword *)(vector))[(bit) >> 5] |= (1 << ((bit) & 31))) : (((dword *)(vector))[(bit) >> 5] &= ~(1 << ((bit) & 31))))

#pragma inline_depth(0)
/* stores the time, in ticks up to 1800, in 31 steps */
// @retail 0x20b450
bool function_20b450(long definition_index, real ticks)
{
	bool result = false;
	if (ticks < 0.0f)
		ticks = 0.0f;
	else if (ticks > 1800.0f)
		ticks = 1800.0f;
	short timer = (short)(long)(ticks * (31.0f / 1800.0f));
	if (definition_index != NONE)
	{
		s_sound_timer_definition *definition = (s_sound_timer_definition *)g_4e3b44[definition_index & 0xffff].data;
		if (definition->pitch_range_count == 1)
		{
			s_permutation_set *set = &g_51ebd4->sets[definition->first_pitch_range_index];
			long bit_index = function_219430((s_animation_state *)set, (s_animation_ref *)definition);
			if (bit_index != NONE)
			{
				long count = set->count;
				long offset = bit_index + count + (short)log2_ceiling_plus_one(count);
				for (long i = 0; i < 5; i++)
				{
					long bit = offset + i;
					TIMER_BIT_SET(g_51e9ec, bit, timer & (1 << i));
				}
				result = true;
			}
		}
	}
	return result;
}
#pragma inline_depth(255)
