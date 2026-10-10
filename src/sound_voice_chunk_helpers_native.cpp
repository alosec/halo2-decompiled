#include "data_array.h"
#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact compact helpers beneath queued voice chunk transitions. */

struct s_sound_reference_holder
{
	byte unknown00[8];
	long reference_index;
};

struct s_sound_reference
{
	byte unknown00[4];
	byte reference_count;
	byte unknown05[0x0b];
};

struct s_permutation_group
{
	byte unknown00[0x0e];
	short count;
};

static_assert(sizeof(s_sound_reference_holder) == 0x0c,
	"sound reference holder size");
static_assert(offsetof(s_sound_reference_holder, reference_index) == 0x08,
	"sound reference index offset");
static_assert(sizeof(s_sound_reference) == 0x10,
	"sound reference size");
static_assert(offsetof(s_sound_reference, reference_count) == 0x04,
	"sound reference count offset");
static_assert(sizeof(s_permutation_group) == 0x10,
	"permutation group size");
static_assert(offsetof(s_permutation_group, count) == 0x0e,
	"permutation group count offset");

extern s_record_pool *g_502104;

// @retail 0x125ef0
void sound_reference_release(s_sound_reference_holder *holder)
{
	s_sound_reference *reference =
		reinterpret_cast<s_sound_reference *>(g_502104->data) +
		(holder->reference_index & 0xffff);
	reference->reference_count--;
}

// @retail 0x219290
long function_219290(short index, s_permutation_group *group)
{
	if (group && index < group->count - 1)
		return index + 1;
	return NONE;
}

#endif
