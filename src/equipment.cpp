// @flags /O2 /arch:SSE /Gr
/* EQUIPMENT.CPP: equipment placement and pickup sounds.
   See docs/equipment.md for the original-object mapping. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

struct s_equipment_object_view
{
	long definition_index;
	union
	{
		struct
		{
			dword : 15;
			dword cannot_be_garbage : 1;
			dword shadowless : 1;
			dword : 15;
		} object_flags;
		dword object_flag_bits;
	};
	byte unknown08[0x6c - 0x08];
	real field_x86bef3;
	byte unknown70[0x12c - 0x70];
	struct
	{
		word : 7;
		word does_not_accelerate : 1;
	} item_flags;
};

struct s_equipment_header_view
{
	byte unknown00[8];
	s_equipment_object_view *object;
};

struct s_scenario_equipment_view
{
	byte unknown00[0x34];
	struct
	{
		dword created_at_rest : 1;
		dword : 1;
		dword does_accelerate : 1;
		dword : 29;
	} flags;
};

struct s_equipment_definition_view
{
	byte unknown00[0x138];
	long pickup_sound_index;
};

#define EQUIPMENT_GET(index) (((s_equipment_header_view *)g_4e0300->data)[(index) & 0xffff].object)
#define EQUIPMENT_DEFINITION_GET(index) ((s_equipment_definition_view *)g_4e3b44[(index) & 0xffff].bytes)

void __stdcall function_b9b90(long object_index, bool disable);
long function_1896c0(real scale, long tag_index);

// @retail 0xf8090
void __stdcall function_f8090(long equipment_index, s_scenario_equipment_view *placement)
{
	s_equipment_object_view *equipment = EQUIPMENT_GET(equipment_index);

	function_b9b90(equipment_index, TEST_FIELD_BIT(placement->flags.created_at_rest));
	dword *flags = &equipment->object_flag_bits;
	*flags |= 0x8000;
	equipment->object_flag_bits |= 0x10000;
	bool does_not_accelerate = !placement->flags.does_accelerate;
	if (does_not_accelerate)
		equipment->item_flags.does_not_accelerate = true;
	else
		equipment->item_flags.does_not_accelerate = false;

	if (!TEST_FIELD_BIT(placement->flags.created_at_rest))
		equipment->field_x86bef3 += 0.05f;
}

// @retail 0xf8110
void function_f8110(long equipment_index)
{
	s_equipment_object_view *equipment = EQUIPMENT_GET(equipment_index);
	s_equipment_definition_view *definition = EQUIPMENT_DEFINITION_GET(equipment->definition_index);

	if (definition->pickup_sound_index != NONE)
		function_1896c0(1.0f, definition->pickup_sound_index);
}

// @retail 0xf8160
void function_f8160(long equipment_definition_index)
{
	s_equipment_definition_view *definition = EQUIPMENT_DEFINITION_GET(equipment_definition_index);

	if (definition->pickup_sound_index != NONE)
		function_1896c0(1.0f, definition->pickup_sound_index);
}

/* Prefix of the actual equipment type definition at 0x467d98, through
   its placement callback. The remaining callbacks and parent-type list
   are outside this view. Retail calls this slot with the stdcall ABI. */
struct s_equipment_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[8];
	void (__stdcall *place)(long, s_scenario_equipment_view *);
};

s_equipment_type_definition_view g_467d98 =
{
	"equipment", 'eqip', 0x184, 0x80, 0x88, 0x38,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_f8090
};
