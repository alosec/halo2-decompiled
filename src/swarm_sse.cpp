// @flags /O2 /Gr /arch:SSE
/* SWARM_SSE.CPP: the swarm code built with SSE (include/swarm.h): the
   actor's update of its swarm, and a creature handing its parent unit to a
   new actor. */

#include "unknown_2551c0.h"
#include "globals.h"
#include "props.h"
#include "swarm.h"

void __stdcall function_28e770(long actor_index, long object_index);
void function_28f3b0(long actor_index, long object_index);

__forceinline s_object_ai_data *swarm_creature_data_get(s_handler_object_view *object)
{
	s_object_ai_data *creature;
	if (!object->flags134)
		creature = (s_object_ai_data *)((byte *)object + object->ai_offset);
	else
		creature = NULL;
	return creature;
}

/* swarm_creature_iterator_next, inlined */
PRIVATE __forceinline s_handler_object_view *swarm_creature_iterator_advance(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;
	if (iterator->next_index != NONE)
	{
		object = handler_object_get(iterator->next_index);
		iterator->index = iterator->next_index;
		s_object_ai_data *creature = swarm_creature_data_get(object);
		iterator->next_index = creature ? creature->next_creature_index : NONE;
	}
	return object;
}

/* an actor's update of its swarm: the event count lapses two seconds after
   the last event, then each creature is updated in three passes */
// @retail 0x28f700
void __stdcall swarm_update(long actor_index)
{
	s_swarm_leader *actor = swarm_leader_get(actor_index);
	s_swarm *swarm = swarm_get(actor->swarm_handle);
	if ((real)(g_510c54->game_time - swarm->last_event_time) * g_510c54->rate > 2.0f)
		swarm->event_count = 0;
	s_ai_object_iterator iterator;
	iterator.next_index = swarm_get(actor->swarm_handle)->first_creature_index;
	while (swarm_creature_iterator_advance(&iterator) != NULL)
	{
		long object_index = iterator.index;
		function_28e770(actor_index, object_index);
	}
	iterator.next_index = swarm_get(actor->swarm_handle)->first_creature_index;
	while (swarm_creature_iterator_advance(&iterator) != NULL)
	{
		long object_index = iterator.index;
		function_28f3b0(actor_index, object_index);
	}
	iterator.next_index = swarm_get(actor->swarm_handle)->first_creature_index;
	while (swarm_creature_iterator_advance(&iterator) != NULL)
	{
		long object_index = iterator.index;
		swarm_creature_apply_orientation(object_index);
	}
}

long function_1e0dc0(short team, long unit_index, short request_value, short squad_index);
void function_269250(long clump_index, long actor_index);
bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_ref_argument,
	long reference_actor_index, long reference_index);

/* a creature riding a unit (a parent object of type 0 that allows it): an
   actor is made for the unit, joins the clump of the creature's actor and
   learns its prop, and the creature leaves its swarm and the world */
// @retail 0x28fe50
void __stdcall swarm_creature_convert_parent_unit(long object_index)
{
	s_handler_object_view *object = handler_object_get(object_index);
	s_object_ai_data *creature = swarm_creature_data_get(object);
	if (creature)
	{
		long parent_index = ((s_swarm_object *)object)->parent_index;
		if (parent_index != NONE)
		{
			s_swarm_object_header *parent_header = swarm_object_header_get(parent_index);
			if (parent_header->type == 0)
			{
				short squad_index = NONE;
				s_swarm_object *parent = parent_header->object;
				s_swarm_leader *actor = NULL;
				if (creature->actor_index != NONE)
				{
					actor = swarm_leader_get(creature->actor_index);
					squad_index = (short)actor->squad_index;
				}
				if (TEST_FIELD_BIT(parent->convertible))
				{
					long created_actor_index = function_1e0dc0(((s_swarm_object *)object)->team, parent_index, creature->target_state, (short)squad_index);
					if (actor)
					{
						if (actor->clump_index != NONE && created_actor_index != NONE)
						{
							function_269250(actor->clump_index, created_actor_index);
							if (actor->prop_index != NONE)
							{
								long prop_object_index = prop_ref_get(actor->prop_index)->object_index;
								function_25d020(created_actor_index, prop_object_index, NONE, NONE, creature->actor_index, actor->prop_index);
							}
						}
						if (actor->flag00c && created_actor_index != NONE)
							swarm_leader_get(created_actor_index)->flag00c = true;
					}
					if (creature->swarm_handle != NONE)
						swarm_remove_creature(creature->swarm_handle, object_index, true, true);
				}
			}
		}
	}
}
