// @flags /O2 /Gr
/* UNKNOWN_1AFDE0.CPP: the first callbacks of slot type 0x3a (its handler,
   g_47dec0, is in unknown_1b0020.cpp) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"

/* the state of a slot of type 0x3a as these callbacks see it: the block after
   the slot's header (s_slot's unknownc) */
struct s_slot_3a_data
{
	short unknown00;
	byte unknown02;
	byte unknown03;
	s_reference reference;
	byte unknown08[0x10 - 8];
	long unknown10;
	bool unknown14;
	byte unknown15;
	short unknown16;
	byte unknown18[0x34 - 0x18];
};

/* an element of g_50241c (0xc4 bytes) */
struct s_50241c_element_1c
{
	byte unknown00[0x1c];
	long unknown1c;
	byte unknown20[0xc4 - 0x20];
};

bool function_25da40(s_prop_datum *datum);

// @retail 0x1afde0
short __stdcall function_1afde0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown344 != NONE)
	{
		s_prop_node_view *prop = prop_node_get(actor->unknown344);
		if (function_25da40((s_prop_datum *)prop) && prop->unknown27 > 0)
			result = 3;
	}
	return result;
}

// @retail 0x1afe50
bool __stdcall function_1afe50(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_3a_data *state = (s_slot_3a_data *)slot->unknownc;
	bool result = false;

	actor->unknown3f2 = false;
	if (actor->unknown344 != NONE || state->unknown10 != NONE)
	{
		result = true;
		state->reference = g_470fa0;
		if (!state->unknown14)
		{
			state->unknown10 = ((s_50241c_element_1c *)g_50241c->data)[prop_node_get(actor->unknown344)->unknown08 & 0xffff].unknown1c;
			state->unknown00 = 0;
			state->unknown14 = true;
		}
		state->unknown16 = 0;
		state->unknown02 = 0;

		long unit_index = actor_get(actor_index)->unknown018;
		if (unit_index != NONE)
			function_20ba60(0x42, unit_index, NONE, NONE, NONE, NULL);
	}
	return result;
}
