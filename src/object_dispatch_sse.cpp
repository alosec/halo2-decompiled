// @flags /O2 /arch:SSE /Gr
/* OBJECT_DISPATCH_SSE.CPP: the per-object-type event dispatch, first part (the
   rest is in object_dispatch.cpp). Each function calls one event function of
   every definition in the object's type hierarchy (object_dispatch.h) */

#include "object_dispatch.h"

/* creates the object in each of its types; false when one of them fails */
// @retail 0x108a90
bool object_dispatch_create(long object_index, long placement, long failed)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	bool result = true;
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->create)
		{
			result = result && type->create(object_index, placement, failed);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
	return result;
}

/* places the object from its scenario datum in each of its types */
// @retail 0x108b10
void object_dispatch_place(long object_index, long scenario_datum)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->place)
		{
			type->place(object_index, scenario_datum);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that the new object is set up */
// @retail 0x108b80
void object_dispatch_finish_creation(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->finish_creation)
		{
			type->finish_creation(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* deletes the object in each of its types */
// @retail 0x108bf0
void object_dispatch_remove(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->remove_object)
		{
			type->remove_object(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* updates the object in each of its types; true while any keeps it active */
// @retail 0x108c60
bool object_dispatch_update(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	bool result = false;
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->update)
		{
			result |= type->update(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
	return result;
}

/* the second update pass in each of the object's types */
// @retail 0x108cd0
void object_dispatch_late_update(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	long i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->late_update)
		{
			type->late_update(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}


/* asks the object's types, until one answers, whether the object is in the
   clusters; true when one answered (with *visible) */
// @retail 0x108d30
bool object_dispatch_test_cluster_visibility(long object_index, long clusters, long visible)
{
	struct { s_object_type_definition *definition; bool answered; s_object_type_definition **entry; } iterator;
	iterator.definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	iterator.answered = false;
	iterator.entry = iterator.definition->type_hierarchy;
	while (!iterator.answered && *iterator.entry)
	{
		s_object_type_definition *type = *iterator.entry;
		if (type->test_cluster_visibility)
		{
			iterator.answered = type->test_cluster_visibility(object_index, clusters, visible);
		}
		iterator.entry++;
	}
	return iterator.answered;
}

/* the object's named function value from the first type that has it; zero
   and disabled when none does */
// @retail 0x108d90
bool object_dispatch_query_function_value(long object_index, long name, real *value, bool *enabled)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	bool found = false;
	*value = 0.0f;
	*enabled = false;
	s_object_type_definition **entry = definition->type_hierarchy;
	while (!found && *entry)
	{
		s_object_type_definition *type = *entry;
		if (type->query_function_value)
		{
			found = type->query_function_value(object_index, name, value, enabled);
		}
		entry++;
	}
	if (!found)
	{
		*value = 0.0f;
		*enabled = found;
	}
	return found;
}

/* tells each of the object's types that it has a simulation entity */
// @retail 0x108e10
void object_dispatch_attach_simulation_entity(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->attach_simulation_entity)
		{
			type->attach_simulation_entity(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that it loses its simulation entity */
// @retail 0x108e80
void object_dispatch_detach_simulation_entity(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->detach_simulation_entity)
		{
			type->detach_simulation_entity(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that another object is being deleted */
// @retail 0x108ef0
void object_dispatch_object_removed(long object_index, long deleted_object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->on_object_removed)
		{
			type->on_object_removed(object_index, deleted_object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that a player is being deleted */
// @retail 0x108f60
void object_dispatch_player_removed(long object_index, long player_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->on_player_removed)
		{
			type->on_player_removed(object_index, player_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}
