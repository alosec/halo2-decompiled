// @flags /O2 /Gr
/* UNKNOWN_256B00.CPP: slot handler 0x77 (handler at 0x47f9a8) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"

/* the slot state of handler 0x77 */
struct s_slot_77_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[0x40 - 0xe];
};

short __stdcall function_256b00(long actor_index);
short __stdcall function_256bd0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_256b60(long actor_index, s_slot *slot);
void __stdcall function_257010(long actor_index, s_slot *slot);

s_slot_handler_2 g_47f9a8 =
{
	{
		0x77, 2, NONE, -2, 0,
		function_256b00, function_256bd0, function_256b60, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_257010
};

// @retail 0x256b00
short __stdcall function_256b00(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown018 != NONE)
	{
		s_object_header_view *header = object_header_get(actor->unknown018);
		s_handler_object_view *object = handler_object_get(actor->unknown018);

		if (!header->type && (object->flags19 & 1) && (object->flags19 & 2))
		{
			result = 3;
		}
	}
	return result;
}

// @retail 0x256b60
bool __stdcall function_256b60(long actor_index, s_slot *slot)
{
	s_slot_77_state *state = (s_slot_77_state *)slot;
	real seconds = (slot_random() + 1.f) * 2.f;
	real ticks = seconds * g_510c54->field_2_3;
	long rounded;

	__asm
	{
		fld ticks
		fistp rounded
	}
	state->timer = (short)rounded;
	return true;
}

// @retail 0x257010
void __stdcall function_257010(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown456 = true;
	actor->unknown458 = actor->unknown290;
	actor->unknown450 = 0x6000085;
	*(short *)actor->unknown454 = 0;
}
