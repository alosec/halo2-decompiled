// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1088A0.CPP: the object type definitions: the type of a tag, the
   list of types their initialize runs through, and per-object-type event
   dispatch (continued in unknown_108a90.cpp and unknown_108fd0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0b68c0.h"

struct s_object_handlers_1088a0
{
	byte unknown00[0x20];
	void (__stdcall *handler20)(long);
	void (__stdcall *handler24)(long);
	void (__stdcall *handler28)(void *);
};

struct s_object_type_definition_1088a0
{
	byte unknown00[4];
	dword group_tag;
	byte unknown08[8];
	void (*initialize)(void);
	byte unknown14[0x84 - 0x14];
	s_object_type_definition_1088a0 *handlers[16];
	s_object_type_definition_1088a0 *next;
};

struct s_object_1088a0
{
	byte unknown00[0xaa];
	signed char type;
};

struct s_object_header_1088a0
{
	byte unknown00[8];
	s_object_1088a0 *object;
};

struct s_tag_group_1088a0
{
	dword group_tag;
};

/* the first object type definition of the list the initialize runs through
   (defined in unknown_0b68c0.cpp) */
#define FIRST_OBJECT_TYPE_DEFINITION (*(s_object_type_definition_1088a0 **)&g_4e0330)

#define OBJECT_TYPE_DEFINITIONS ((s_object_type_definition_1088a0 **)g_468630)
#define OBJECT_TYPE_DEFINITION_1088a0(index) (OBJECT_TYPE_DEFINITIONS[((s_object_header_1088a0 *)g_4e0300->data)[(index) & 0xFFFF].object->type])

// @retail 0x1088a0
long function_1088a0(long tag_index)
{
	long const *tag_index_ptr = &tag_index;
	dword group_tag = ((s_tag_group_1088a0 *)&g_4e3b44[(short)*tag_index_ptr])->group_tag;

	long result = NONE;

	for (long type = 0; type < 13; type++)
	{
		if (OBJECT_TYPE_DEFINITIONS[type]->group_tag == group_tag)
		{
			result = type;
			break;
		}
	}
	return result;
}

// @retail 0x1088e0
void function_1088e0(void)
{
	s_object_type_definition_1088a0 **next = &FIRST_OBJECT_TYPE_DEFINITION;

	for (long type = 0; type < 13; type++)
	{
		s_object_type_definition_1088a0 *definition = OBJECT_TYPE_DEFINITIONS[type];

		*next = definition;
		next = &definition->next;
		for (short i = 0; i < sizeof(definition->handlers) / sizeof(definition->handlers[0]); i++)
		{
			s_object_type_definition_1088a0 *parent = definition->handlers[i];
			if (!parent)
				break;
			if (!parent->next)
			{
				*next = parent;
				next = &parent->next;
			}
		}
	}
	*next = NULL;
	for (s_object_type_definition_1088a0 *definition = FIRST_OBJECT_TYPE_DEFINITION; definition; definition = definition->next)
	{
		if (definition->initialize)
			definition->initialize();
	}
}

// @retail 0x108960
void function_108960(long object_index)
{
	s_object_type_definition_1088a0 *definition = OBJECT_TYPE_DEFINITION_1088a0(object_index);
	short i = 0;
	s_object_type_definition_1088a0 **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers_1088a0 *handlers = (s_object_handlers_1088a0 *)*slot;
		if (handlers->handler20)
		{
			handlers->handler20(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x1089d0
void function_1089d0(long object_index)
{
	s_object_type_definition_1088a0 *definition = OBJECT_TYPE_DEFINITION_1088a0(object_index);
	short i = 0;
	s_object_type_definition_1088a0 **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers_1088a0 *handlers = (s_object_handlers_1088a0 *)*slot;
		if (handlers->handler24)
		{
			handlers->handler24(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108a40
void function_108a40(long *placement)
{
	s_object_type_definition_1088a0 *definition = OBJECT_TYPE_DEFINITIONS[function_1088a0(*placement)];
	short i = 0;
	s_object_type_definition_1088a0 **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers_1088a0 *handlers = (s_object_handlers_1088a0 *)*slot;
		if (handlers->handler28)
		{
			handlers->handler28(placement);
		}
		i++;
		slot = &definition->handlers[i];
	}
}
