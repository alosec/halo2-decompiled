#include "data_array.h"
#include "globals.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered playback-held cache-reference release. */

struct s_sound_reference
{
	byte unknown00[4];
	byte reference_count;
	byte unknown05[0x0b];
};

struct s_sound_globals_reference_view
{
	byte unknown00[0x24];
	struct s_pitch_range
	{
		byte unknown00[8];
		short first_permutation;
		short count;
	} *pitch_ranges;
	byte unknown28[4];
	struct s_permutation
	{
		byte unknown00[0x0c];
		short first_chunk;
		byte unknown0e[2];
	} *permutations;
	byte unknown30[0x14];
	struct s_chunk
	{
		byte unknown00[8];
		long reference_index;
	} *chunks;
};

static_assert(sizeof(s_sound_reference) == 0x10,
	"sound reference size");
static_assert(offsetof(s_sound_reference, reference_count) == 0x04,
	"sound reference count offset");
static_assert(offsetof(s_sound_globals_reference_view, pitch_ranges) == 0x24,
	"sound pitch-range table offset");
static_assert(offsetof(s_sound_globals_reference_view, permutations) == 0x2c,
	"sound permutation table offset");
static_assert(offsetof(s_sound_globals_reference_view, chunks) == 0x44,
	"sound chunk table offset");

extern s_record_pool *g_502104;

// @retail 0x126960
void function_126960(s_sound_playback *sound)
{
	if (sound->holds_reference)
	{
		s_sound_globals_reference_view *tables =
			reinterpret_cast<s_sound_globals_reference_view *>(g_51ebd4);
		long const pitch_range =
			sound_definition_get(sound->definition_index)->pitch_range_base +
			sound->pitch_range_index;
		long const permutation =
			tables->pitch_ranges[pitch_range].first_permutation +
			sound->permutation_index;
		long const chunk = tables->permutations[permutation].first_chunk +
			sound->chunk_index;
		s_sound_reference *reference =
			reinterpret_cast<s_sound_reference *>(g_502104->data) +
			(tables->chunks[chunk].reference_index & 0xffff);
		reference->reference_count--;
		sound->holds_reference = false;
	}
}

#endif
