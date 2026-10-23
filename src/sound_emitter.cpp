// @flags /O2 /Gr
/* SOUND_EMITTER.CPP: the sound scenery object type's creation and
   scenario placement callbacks, and its type definition */

#include "sound_emitter.h"

/* object flag bit 16, also set on every projectile (projectiles.cpp) */
enum
{
	_sound_emitter_object_flag_16 = 0x10000
};

/* creates a sound scenery object: sets object flag 16 */
// @retail 0x207b90
bool __stdcall sound_emitter_create(long object_index, long creation_argument1, long creation_argument2)
{
	s_sound_emitter_datum *scenery = sound_emitter_get(object_index);
	dword *flags = &scenery->object_flags;
	*flags |= _sound_emitter_object_flag_16;
	return true;
}

/* places a sound scenery object: copies its data from the placement */
// @retail 0x207bc0
void __stdcall sound_emitter_place(long object_index, s_sound_emitter_placement *placement)
{
	s_sound_emitter_datum *scenery = sound_emitter_get(object_index);
	scenery->sound_emitter_datum_of[0] = placement->sound_emitter_datum_of[0];
	scenery->sound_emitter_datum_of[1] = placement->sound_emitter_datum_of[1];
	scenery->sound_emitter_datum_of[2] = placement->sound_emitter_datum_of[2];
	scenery->sound_emitter_datum_of[3] = placement->sound_emitter_datum_of[3];
	scenery->sound_emitter_datum_of[4] = placement->sound_emitter_datum_of[4];
	scenery->sound_emitter_datum_of[5] = placement->sound_emitter_datum_of[5];
	scenery->sound_emitter_datum_of[6] = placement->sound_emitter_datum_of[6];
}

/* the type definition's prefix through the creation and placement
   callbacks (retail 0x4680b8); retail's creation callback takes three
   stack arguments */
struct s_sound_emitter_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[7];
	bool (__stdcall *create)(long, long, long);
	void (__stdcall *place)(long, s_sound_emitter_placement *);
};

s_sound_emitter_type_definition g_sound_emitter_type_definition =
{
	"sound_scenery", 'ssce', 0x148, 0xd8, 0xe0, 0x50,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	sound_emitter_create,
	sound_emitter_place
};
