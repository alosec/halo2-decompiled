// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A58D0_K2.CPP: the object a simulation entity stands for */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "unknown_xa19f52.h"

/* the entity an index stands for, or none when its salt is stale (the
   same as simulation_entity_try_get, which retail inlines here) */
static __forceinline s_simulation_entity *entity_try_get(s_simulation_entity_table *table, long entity_index)
{
	s_simulation_entity *result = 0;
	if (table->entities[entity_index & 0x3ff].identifier == entity_index)
		result = &table->entities[entity_index & 0x3ff];
	return result;
}

/* the object of an entity, or none when the index is none or stale, or
   the entity's definition rejects it; retail keeps the plain __fastcall
   (the index in ecx) that its many callers use */
// @retail 0xa58d0 standard
long function_a58d0(long entity_index)
{
	long object_index = NONE;
	if (entity_index != NONE)
	{
		s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
		s_simulation_entity *entity = entity_try_get(&world->database->table, entity_index);
		if (entity)
		{
			long type = entity->type;
			s_entity_definitions *definitions = (s_entity_definitions *)g_4cf784;
			c_object_type_definition *definition = definitions->definitions[type];
			if (definition->v7((s_entity *)entity) && entity->object_index != NONE)
				object_index = entity->object_index;
		}
	}
	return object_index;
}
