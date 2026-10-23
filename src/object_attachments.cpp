// @flags /O2 /arch:SSE /Gr
/* OBJECT_ATTACHMENTS.CPP: moving the children attached to one of an object's
   nodes onto a marker (script function 874: an object and three
   name identifiers), finding the child attached at a marker (script function
   75) and the flag that rebuilds an object's physics (script function 63) */

#include "object_masks_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "object_attachments.h"
#include <math.h>

short function_b8d30(long object_index, long marker_name, s_object_marker *markers, short count, bool flag);
void function_b9a90(long object_index);
void __stdcall function_b8ee0(long parent_index, long parent_marker_name, long index, long child_marker_name);

/* every child attached to the node of object_index's marker marker_name is
   detached and attached again, at parent_marker_name of object_index and its
   own child_marker_name; the object masks mark the children already moved */
// @retail 0x13c5a0
void object_children_reattach(long object_index, long marker_name, long parent_marker_name, long child_marker_name)
{
	s_object_marker marker;
	if (object_index != NONE && function_b8d30(object_index, marker_name, &marker, 1, false) == 1)
	{
		long child_index = ((s_object_attachment_header *)g_4e0300->data)[object_index & 0xffff].object->first_child_index;
		g_4de2fc++;
		g_4de2f8 = true;
		while (child_index != NONE)
		{
			s_object_attachment_view *child = ((s_object_attachment_header *)g_4e0300->data)[child_index & 0xffff].object;
			long next_index = child->next_sibling_index;
			if (g_4de300[child_index & 0xffff] != g_4de2fc)
			{
				g_4de300[child_index & 0xffff] = g_4de2fc;
				if (child->attached_node_index == marker.node_index)
				{
					function_b9a90(child_index);
					function_b8ee0(object_index, parent_marker_name, child_index, child_marker_name);
				}
			}
			child_index = next_index;
		}
		g_4de2f8 = false;
	}
}

#define OBJECT_ATTACHMENT_GET(index) (((s_object_attachment_header *)g_4e0300->data)[(index) & 0xffff].object)

void __stdcall function_1c3770(long object_index, dword flags);

/* sets or clears the object's physics flag, rebuilding its havok component
   when the flag changes (script function 63) */
// @retail 0x10ad40
void object_physics_flag_set(long object_index, bool enable)
{
	if (object_index != NONE)
	{
		s_object_attachment_view *object = OBJECT_ATTACHMENT_GET(object_index);
		bool was_enabled = TEST_FIELD_BIT(object->physics_flag_byte);

		if (enable)
			object->physics_flag = true;
		else
			object->physics_flag = false;
		if (was_enabled != enable)
			function_1c3770(object_index, 0);
	}
}

/* the first child attached to the node of the object's marker within one
   world unit of it, or NONE (script function 75) */
// @retail 0x10ae60
long object_find_child_at_marker(long object_index, long marker_name)
{
	if (object_index != NONE)
	{
		long child_index = OBJECT_ATTACHMENT_GET(object_index)->first_child_index;
		s_object_marker marker;

		function_b8d30(object_index, marker_name, &marker, 1, false);
		while (child_index != NONE)
		{
			s_object_attachment_view *child = OBJECT_ATTACHMENT_GET(child_index);

			if (child->attached_node_index == marker.node_index)
			{
				real dx = child->position.x - marker.node_matrix.position.x;
				real dy = child->position.y - marker.node_matrix.position.y;
				real dz = child->position.z - marker.node_matrix.position.z;

				real length_squared = dx * dx + dz * dz;
				length_squared += dy * dy;
				if (1.0f > sqrt(length_squared))
					return child_index;
			}
			child_index = child->next_sibling_index;
		}
	}
	return NONE;
}
