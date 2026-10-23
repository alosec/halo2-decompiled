/* UNKNOWN_2551C0.H: what the slot handlers of 0x2551c0..0x257cb0 (lane I)
   share: their view of the objects and the data array they read */

#ifndef UNKNOWN_2551C0_H
#define UNKNOWN_2551C0_H

#include "unknown_11c920.h"
#include "slot_handler.h"

/* g_5044c8: the ai's per-actor perception data (0x34 byte elements, from
   actor +0x1c) */
extern s_record_pool *g_5044c8;

struct s_perception_datum
{
	byte unknown00[0x18];
	long object_index;
	short unknown1c;
	byte unknown1e[0x34 - 0x1e];
};

inline s_perception_datum *perception_get(long index)
{
	return (s_perception_datum *)(g_5044c8->data + (index & 0xffff) * sizeof(s_perception_datum));
}

/* the object fields these handlers read (the objects of g_4e0300) */
struct s_handler_object_view
{
	byte unknown000[0xc];
	long next_object_index;
	long first_child_index;
	byte unknown014[0x19 - 0x14];
	byte flags19;
	byte unknown01a[0xaa - 0x1a];
	byte type;
	byte unknownab[0x134 - 0xab];
	dword flags134;
	byte unknown138[0x13a - 0x138];
	short ai_offset;
};

inline s_handler_object_view *handler_object_get(long object_index)
{
	return (s_handler_object_view *)object_get(object_index);
}

/* the actor's perception index (+0x1c), which s_actor_view leaves unnamed */
struct s_handler_actor_view
{
	byte unknown000[0x1c];
	long perception_index;
	byte unknown020[0x888 - 0x20];
};

inline long actor_perception_index(long actor_index)
{
	return ((s_handler_actor_view *)actor_get(actor_index))->perception_index;
}

/* the ai's iterator over the objects near a perception (0x290c50, 0x290c80) */
struct s_ai_object_iterator
{
	long next_index;
	long index;
};

s_handler_object_view *swarm_creature_iterator_next(s_ai_object_iterator *iterator);

#endif
