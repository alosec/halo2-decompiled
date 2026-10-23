/* AI_ACTOR.H: the actor's unit object as lane M's behavior code
   (0x1a8000..0x1affff) and the actor helpers it calls (0x1e1f20..0x1e9700)
   see it, and those helpers. The actor itself is s_actor_view
   (slot_handler.h). */

#ifndef AI_ACTOR_H
#define AI_ACTOR_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "slot_handler.h"

struct s_ai_object
{
	long definition_index;
	byte unknown004[0x19 - 0x4];
	byte flags19;
	byte unknown01a[0x30 - 0x1a];
	point3f position;
	byte unknown03c[0x88 - 0x3c];
	vector3f velocity;
	byte unknown094[0x134 - 0x94];
	dword flags134;
	byte unknown138[0x212 - 0x138];
	char current_weapon;
	byte unknown213[0x218 - 0x213];
	long weapons[4];
	byte unknown228[0x23c - 0x228];
	char unknown23c;
	byte unknown23d[0x34c - 0x23d];
	byte unknown34c;
	byte unknown34d[0x350 - 0x34d];
};

struct s_ai_object_header
{
	byte unknown00[8];
	s_ai_object *object;
};

inline s_ai_object *ai_object_get(long object_index)
{
	return ((s_ai_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* the enabled bits of the slot types (g_557c40, globals.h) as lane M's
   callbacks test them: as bitfields, which gives retail's shr/test */
struct s_slot_type_bits
{
	dword word0;
	dword : 12;
	dword type2c : 1;
	dword : 19;
	dword : 19;
	dword type53 : 1;
	dword : 12;
};

#define SLOT_TYPE_BITS ((s_slot_type_bits *)g_557c40)

/* the actor helpers (unknown_1e1f20.cpp, unknown_1e4290.cpp,
   character_tag_accessors.cpp, unknown_1e9700.cpp) */
long function_1e1f20(long actor_index);
bool function_1e2030(long actor_index);
void function_1e4290(long actor_index, bool value);
void function_1e4650(long actor_index, bool value);
void *function_1e5280(long actor_index, long key);
void *function_1e5240(long actor_index);
void *function_1e5380(long actor_index);
real function_1e9700(short row);

#endif