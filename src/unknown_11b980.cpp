// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_11B980.CPP: placement, touch handling, and control switching.
   See docs/unknown_11b980.md for the original-object mapping. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

enum e_control_type
{
	_control_toggle_switch,
	_control_on_button,
	_control_off_button,
	_control_call_button
};

struct s_control_object_view
{
	long definition_index;
	byte unknown004[0x13c - 4];
	long group_index_13c;
	byte unknown140[0x1cc - 0x140];
	dword flags;
	short hud_override_index;
};

struct s_control_header_view
{
	byte unknown00[8];
	s_control_object_view *object;
};

struct s_scenario_control_view
{
	byte unknown00[0x3c];
	dword flags;
	short hud_override_string_list_index;
};

struct s_control_definition_view
{
	byte unknown000[0x11c];
	short type;
	short trigger;
	real call_value;
	long value_124;
	byte unknown128[4];
	long on_effect_index;
	byte unknown130[4];
	long off_effect_index;
	byte unknown138[4];
	long deny_effect_index;
};

struct s_control_group_view
{
	short identifier;
	word flags;
	real value;
	real value_08;
};

#define CONTROL_GET(index) (((s_control_header_view *)g_4e0300->data)[(index) & 0xffff].object)
#define CONTROL_DEFINITION_GET(index) ((s_control_definition_view *)g_4e3b44[(index) & 0xffff].bytes)
#define CONTROL_GROUP_GET(index) (&((s_control_group_view *)g_4e0328.groups->data)[(index) & 0xffff])

bool __stdcall function_1071e0(long group_index, real value);
void function_107980(long object_index, long tag_index);
PRIVATE void function_11ba50(long control_index);

// @retail 0x11b980
void __stdcall function_11b980(long control_index, s_scenario_control_view *placement)
{
	s_control_object_view *control = CONTROL_GET(control_index);

	if (placement->flags & 1)
		control->flags |= 1;
	if (placement->flags & 0x10)
		control->flags |= 2;
	control->hud_override_index = placement->hud_override_string_list_index - 1;
}

// @retail 0x11b9d0
void function_11b9d0(long control_index, long unit_index)
{
	s_control_object_view *control = CONTROL_GET(control_index);
	s_control_definition_view *definition = CONTROL_DEFINITION_GET(control->definition_index);

	if (definition->trigger == 0)
		function_11ba50(control_index);
}

// @retail 0x11ba10
long function_11ba10(long control_index)
{
	s_control_object_view *control = CONTROL_GET(control_index);
	s_control_definition_view *definition = CONTROL_DEFINITION_GET(control->definition_index);
	return definition->value_124;
}

// @retail 0x11ba50
PRIVATE void function_11ba50(long control_index)
{
	s_control_object_view *control = CONTROL_GET(control_index);
	s_control_definition_view *definition = CONTROL_DEFINITION_GET(control->definition_index);

	if (control->group_index_13c != NONE)
	{
		s_control_group_view *group = CONTROL_GROUP_GET(control->group_index_13c);
		real desired_value;

		switch (definition->type)
		{
		case _control_toggle_switch:
			desired_value = group->value > 0.5f ? 0.0f : 1.0f;
			break;
		case _control_on_button:
			desired_value = 1.0f;
			break;
		case _control_off_button:
			desired_value = 0.0f;
			break;
		case _control_call_button:
			desired_value = definition->call_value;
			break;
		default:
			__assume(0);
		}

		if (function_1071e0(control->group_index_13c, desired_value))
			function_107980(control_index, desired_value > 0.5f ? definition->on_effect_index : definition->off_effect_index);
		else
			function_107980(control_index, definition->deny_effect_index);
	}
}

/* Actual control type-definition prefix at 0x4683d8, through its placement
   callback. Later fields and the parent-type list are outside this view. */
struct s_control_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[8];
	void (__stdcall *place)(long, s_scenario_control_view *);
};

s_control_type_definition_view g_4683d8 =
{
	"control", 'ctrl', 0x1d4, 0xb8, 0xc0, 0x44,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_11b980
};
