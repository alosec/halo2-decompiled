// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_218E50.CPP: a sample of a sound permutation's promotion data.
   Decompiled by lane F for 0x18c720. */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_promotions.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the sound's permutation set with the given index */
static inline s_sound_permutation_set *sound_permutation_set_get(s_sound_promotion_tag const *sound, short set_index)
{
	return &((s_sound_globals_promotion_view *)g_51ebd4)->sets[sound->permutation_base + set_index];
}

// score 5 (was 40): the sample argument is read as a word (`mov bp, word ptr`) where retail reads the
// whole dword: retail's parameter is a long that 0x18c720 passes unnarrowed. A long parameter (used as
// `(short)sample_index`) matches this function, with the call in 0x18c720 passing `local_2` itself.
// Round 10: both clamps through PIN 5; the permutation clamp alone 52, the sample clamp alone 40; the
// clamp's arms swapped 62; valid index declared after the globals pointer, the ternary reversed, the
// equality reversed 40 each.
// Earlier: the argument is read into BP where retail reads it into EBP, so the values are
// kept in the 16 bit registers instead of the 32 bit ones.
// Direct signed-count validation with early or positive ownership scored
// 95/90 (164/169 bytes); short/long sample results and global reads varied.
// @retail 0x218e50
real function_218e50(long tag_index, short set_index, short permutation_index, short sample_index)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;
	s_sound_permutation_set *set = sound_permutation_set_get(sound, set_index);
	s_sound_globals_promotion_view *globals = (s_sound_globals_promotion_view *)g_51ebd4;

	if (PIN(permutation_index, 0, set->permutation_count - 1) == permutation_index)
	{
		s_sound_promotion_data *data = globals->promotions[sound->promotion_index].data;
		s_sound_promotion_entry *entry = &data->entries[globals->permutations[set->first_permutation + permutation_index].entry_index];
		long count = entry->sample_count;

		if (count > 0)
		{
			short sample = PIN(sample_index, 0, count - 1);
			return data->samples[sample + entry->sample_offset] * (1.0f / 255.0f);
		}
	}
	return 0.0f;
}
