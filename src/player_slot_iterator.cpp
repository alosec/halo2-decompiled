// @flags /O1 /Ob2 /arch:SSE /Gr
/* PLAYER_SLOT_ITERATOR.CPP: stepping through the four local player slots
   (player_slots.h) */

#include "unknown_11c920.h"
#include "unknown_19b516.h"

/* the slot after this one: 0 after NONE, NONE after the last */
// @retail 0x190262
long player_slot_iterate_next(long index)
{
	long next;

	switch (index)
	{
	case NONE:
		next = 0;
		break;
	case 0:
		next = 1;
		break;
	case 1:
		next = 2;
		break;
	case 2:
		next = 3;
		break;
	default:
		next = NONE;
		break;
	}
	return next;
}
