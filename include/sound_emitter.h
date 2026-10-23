/* SOUND_EMITTER.H: the sound scenery object type ("sound_scenery", 'ssce'):
   its object datum and its scenario placement */

#ifndef SOUND_EMITTER_H
#define SOUND_EMITTER_H

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* a sound scenery object (the type's 0x148 byte datum); placement copies
   the last seven words from the scenario */
struct s_sound_emitter_datum
{
	long definition_index;
	dword object_flags;
	byte unknown008[0x12c - 8];
	dword sound_emitter_datum_of[7];
};
typedef char assert_sound_emitter_datum_size[(sizeof(s_sound_emitter_datum) == 0x148) ? 1 : -1];

/* an entry of the object header data (g_4e0300, 12 bytes) */
struct s_sound_emitter_object_header
{
	byte unknown00[8];
	s_sound_emitter_datum *datum;
};
typedef char assert_sound_emitter_object_header_size[(sizeof(s_sound_emitter_object_header) == 12) ? 1 : -1];

/* a sound scenery placement of the scenario: the words from +0x34 go to
   the object */
struct s_sound_emitter_placement
{
	byte unknown00[0x34];
	dword sound_emitter_datum_of[7];
};
typedef char assert_sound_emitter_placement_size[(sizeof(s_sound_emitter_placement) == 0x50) ? 1 : -1];

inline s_sound_emitter_datum *sound_emitter_get(long object_index)
{
	return ((s_sound_emitter_object_header *)g_4e0300->data)[object_index & 0xffff].datum;
}

#endif
