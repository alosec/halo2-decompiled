// @flags /O2 /Gr
/* OBJECT_DISPATCH.CPP: the per-object-type event dispatch, second part (the
   first is in object_dispatch_sse.cpp) */

#include "object_dispatch.h"

/* tells each of the object's types that its parent was destroyed; true when
   one of them handled it (damage.cpp then skips the object's children). The
   name stays: damage.cpp (a contributor's) calls it */
// @retail 0x108fd0
bool function_108fd0(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	bool handled = false;
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->on_parent_destroyed)
		{
			if (type->on_parent_destroyed(object_index))
			{
				handled = true;
			}
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
	return handled;
}

/* lets each of the object's types adjust its sampled node orientations */
// @retail 0x109050
void object_dispatch_adjust_node_orientations(long object_index, long node_mask, long node_count, long orientations)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->adjust_node_orientations)
		{
			type->adjust_node_orientations(object_index, node_mask, node_count, orientations);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* lets each of the object's types adjust its root node matrix */
// @retail 0x1090d0
void object_dispatch_adjust_root_matrix(long object_index, long matrices)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->adjust_root_matrix)
		{
			type->adjust_root_matrix(object_index, matrices);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* lets each of the object's types adjust its node matrices */
// @retail 0x109140
void object_dispatch_adjust_node_matrices(long object_index, long node_count, long nodes)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->adjust_node_matrices)
		{
			type->adjust_node_matrices(object_index, node_count, nodes);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that its physics was detached */
// @retail 0x1091b0
void object_dispatch_physics_detached(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->on_physics_detached)
		{
			type->on_physics_detached(object_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}

/* tells each of the object's types that a sound started playing on it */
// @retail 0x109220
void object_dispatch_sound_started(long object_index, long sound_tag_index, long sound_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION_FOR(object_index);
	short i = 0;
	s_object_type_definition **entry = &definition->type_hierarchy[0];
	while (definition->type_hierarchy[i])
	{
		s_object_type_definition *type = *entry;
		if (type->on_sound_started)
		{
			type->on_sound_started(object_index, sound_tag_index, sound_index);
		}
		i++;
		entry = &definition->type_hierarchy[i];
	}
}


/* tells each of the object's types that it was moved */
// @retail 0x109290
void object_dispatch_moved(long object_index, long position, long forward, long up)
{
	s_object_type_definition **hierarchy = OBJECT_TYPE_DEFINITION_FOR(object_index)->type_hierarchy;

	for (long i = 0; hierarchy[i]; i++)
	{
		s_object_type_definition *type = hierarchy[i];

		if (type->on_moved)
		{
			type->on_moved(object_index, position, forward, up);
		}
	}
}
