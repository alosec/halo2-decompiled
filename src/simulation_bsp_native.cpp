#if H2_NATIVE_PORT

// @flags /O2 /Gr

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include <stddef.h>

static_assert(sizeof(s_object_header) == 0x0c, "native object header layout");
static_assert(offsetof(s_object_view, field_d4) == 0xd4, "native simulation identifier layout");
static_assert(offsetof(s_object_view, field_d8) == 0xd8, "native simulation state layout");

// Native offline play has no replication world or entity table. BSP changes
// therefore cannot create the retail mode-5 simulation entity associated
// with a scenario slot. This is deliberately idle rather than fabricating a
// replication identifier or touching an unowned world database.
// @retail 0xa8e90
void function_a8e90(long slot)
{
	(void)slot;
}

// The corresponding object-side cleanup still clears the recovered local
// simulation fields. There is no native replication entity or type-handler
// notification to release.
// @retail 0xa7a60
void function_a7a60(long object_index)
{
	s_object_header *header =
		&((s_object_header *)g_4e0300->data)[object_index & 0xffff];
	s_object_view *object = header->object;
	if (object->field_d4 != NONE)
	{
		object->field_d4 = NONE;
		object->field_d8 = 0;
	}
}

#endif
