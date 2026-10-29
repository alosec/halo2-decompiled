/* UNKNOWN_293070.H: a flock member object's membership record and the walk over a flock's
   members (src/unknown_293070.cpp; 0x2955b0 is the step's out-of-line copy) */

#ifndef UNKNOWN_293070_H
#define UNKNOWN_293070_H

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* an object's flock membership: its flock and the next member */
struct s_flock_member
{
	long flock_index;
	byte unknown04[4];
	long next_object_index;
};

/* the object fields a flock member has (+0x134 is 1 for a creature) */
struct s_flock_object
{
	byte unknown000[0x134];
	long type134;
	byte unknown138[2];
	short flock_member_offset;
};

struct s_flock_object_header
{
	byte unknown00[8];
	s_flock_object *object;
};

inline s_flock_member *flock_object_get_member(s_flock_object *object)
{
	s_flock_member *member;
	if (object->type134 == 1)
		member = (s_flock_member *)((byte *)object + object->flock_member_offset);
	else
		member = NULL;
	return member;
}

struct s_flock_member_iterator
{
	long next_object_index;
	long object_index;
};

/* the next member of a flock (null at the end), keeping its object index in the iterator */
PRIVATE __forceinline s_flock_object *function_2937a1(s_flock_member_iterator *arg_0)
{
	long local_0 = arg_0->next_object_index;
	s_flock_object *local_1 = NULL;
	arg_0->object_index = local_0;
	if (local_0 != NONE)
	{
		local_1 = ((s_flock_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
		s_flock_member *local_2 = flock_object_get_member(local_1);
		arg_0->next_object_index = local_2 ? local_2->next_object_index : NONE;
	}
	return local_1;
}

#endif
