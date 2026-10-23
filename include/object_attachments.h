/* OBJECT_ATTACHMENTS.H: the part of an object that links it into its
   parent's list of attached children, as object_children_reattach
   (0x13c5a0) and object_find_child_at_marker (0x10ae60) read it */
#ifndef OBJECT_ATTACHMENTS_H
#define OBJECT_ATTACHMENTS_H

#include <stddef.h>
#include "unknown_11c920.h"

/* an object: the next child of its parent, its own first child, the node of
   the parent it is attached to, its position, and the flag that rebuilds its
   physics */
struct s_object_attachment_view
{
	byte unknown00[0xc];
	long next_sibling_index;
	long first_child_index;
	byte unknown14[4];
	signed char attached_node_index;
	byte unknown19[0x64 - 0x19];
	point3f position;
	byte unknown70[0xc0 - 0x70];
	union
	{
		word flags;
		struct
		{
			word : 5;
			word physics_flag : 1; /* changing it rebuilds the havok component */
		};
		struct /* the same bit, read as a byte */
		{
			byte : 5;
			byte physics_flag_byte : 1;
		};
	};
};
typedef char assert_object_attachment_view_next_sibling[(offsetof(s_object_attachment_view, next_sibling_index) == 0xc) ? 1 : -1];
typedef char assert_object_attachment_view_first_child[(offsetof(s_object_attachment_view, first_child_index) == 0x10) ? 1 : -1];
typedef char assert_object_attachment_view_parent_node[(offsetof(s_object_attachment_view, attached_node_index) == 0x18) ? 1 : -1];
typedef char assert_object_attachment_view_position[(offsetof(s_object_attachment_view, position) == 0x64) ? 1 : -1];
typedef char assert_object_attachment_view_flags[(offsetof(s_object_attachment_view, flags) == 0xc0) ? 1 : -1];

/* an element of the object header data (g_4e0300): the object pointer */
struct s_object_attachment_header
{
	byte unknown00[8];
	s_object_attachment_view *object;
};
typedef char assert_object_attachment_header_size[(sizeof(s_object_attachment_header) == 0xc) ? 1 : -1];

#endif
