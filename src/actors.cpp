// @flags /O2 /Gr
/* ACTORS.CPP: the actor functions built without SSE code (the rest are in
   src/actors_sse.cpp): an actor flag its unit mirrors, and the actor
   iterator (12 callers, among them the props code's 0x25db60) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "squads.h"
#include "unknown_1e46c0.h"

/* sets the actor's flag at +0x225; a unit actor (not a swarm) mirrors it in
   its unit's flags (+0x134 bit 0x20) and clears its flag at +0x226 */
// @retail 0x1e4650
void function_1e4650(long actor_index, bool value)
{
	s_actor_datum *actor = actor_datum_get(actor_index);

	if (value != actor->flag225)
	{
		actor->flag225 = value;
		if (!actor->swarm)
		{
			s_ai_object *unit = ai_object_get(actor->unit_index);
			dword *flags = &unit->flags134;
			if (value)
				*flags |= 0x20;
			else
				*flags &= ~0x20;
			actor->flag226 = false;
		}
	}
}

/* the next datum of the iterator's data array, kept in the iterator */
static inline bool actor_data_iterator_next(s_actor_iterator *iterator)
{
	byte *datum = data_iterator_next_inlined(&iterator->iterator);

	iterator->actor = datum;
	return datum != 0;
}

/* the next actor (only active ones if the iterator asks for them) */
// @retail 0x1e46c0
void *function_1e46c0(s_actor_iterator *iterator)
{
	void *result = NULL;

	if (g_4f55d0->active)
	{
		while (actor_data_iterator_next(iterator))
		{
			s_actor_datum *actor = (s_actor_datum *)iterator->actor;

			if (!iterator->active_only || actor->active)
			{
				result = actor;
				break;
			}
		}
		iterator->actor_index = iterator->iterator.datum_index;
	}
	return result;
}
