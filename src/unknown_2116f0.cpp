// @flags /O2 /Gr
/* UNKNOWN_2116F0.CPP: listing the seats of a unit that pass a filter (outside
   functions lane A's AI script functions need) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "units.h"

/* the scenario's seat filters (g_4e0350 +0x228): an object definition and a
   bit vector of its seats */
struct s_seat_filter
{
	long definition_index;
	dword seats[1];
};

struct s_scenario_seat_filters_view
{
	byte unknown000[0x22c];
	s_seat_filter *seat_filters;
};

/* a seat definition's type (0 for none) */
struct s_seat_definition_type_view
{
	byte unknown000[0x3c];
	short type;
};

// @retail 0x200ee0
short function_200ee0(long object_index, short mode, long *selected_object, short *occupied_count)
{
	(void)&mode;
	(void)&selected_object;
	(void)&occupied_count;
	s_object_seat seats[64];
	short selected_seat = NONE;
	short count = 0;
	function_c8a40(object_index, seats, &count, 64);
	for (short i = 0; i < count; i++)
	{
		s_object_seat *seat = &seats[i];
		long unit_index = seat->object_index;
		word seat_index = seat->seat_index;
		if (function_c8f60(unit_index, (short)seat_index) != NONE)
		{
			if (occupied_count)
				(*occupied_count)++;
		}
		else if (selected_seat == NONE)
		{
			s_seat_definition_type_view *seat_definition = (s_seat_definition_type_view *)seat->definition;
			if (!seat_definition->type)
				continue;
			switch (mode)
			{
			case 0:
				if (seat_definition->type == 3) continue;
				if (seat_definition->type == 4) continue;
				break;
			case 1:
				if (seat_definition->type != 1) continue;
				break;
			case 2:
				if (seat_definition->type != 2) continue;
				break;
			case 3:
				if (seat_definition->type != 5) continue;
				break;
			case 4:
				if (seat_definition->type != 3) continue;
				break;
			case 5:
				if (seat_definition->type != 4) continue;
				break;
			case 6:
			case 7:
				/* only typeless seats, which the test above already skipped */
				if (seat_definition->type != 0) continue;
				break;
			default:
				continue;
			}
			selected_seat = seat_index;
			if (selected_object)
				*selected_object = unit_index;
			if (!occupied_count)
				break;
		}
	}
	return selected_seat;
}

// @retail 0x201010
long function_201010(long object_index, short type_index, short mode, short *seat_index)
{
	(void)&type_index;
	(void)&mode;
	(void)&seat_index;
	long result = NONE;
	long selected = NONE;
	short const *type_reference = &type_index;
	short minimum = 0x7fff;
	short best_seat;
	if (mode != 3 && mode != 6 && object_index != NONE)
	{
		byte *definition = *(byte **)((byte *)g_4e0350 + 0x7c) + *type_reference * 0x28;
		do
		{
			byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			if (*(long *)(definition + 4) == *(long *)object)
			{
				long occupied = 0;
				long selected_object = NONE;
				short seat = function_200ee0(object_index, mode, &selected_object, (short *)&occupied);
				if (seat != NONE && (short)occupied < minimum)
				{
					selected = selected_object;
					minimum = (short)occupied;
					best_seat = seat;
					if (!(short)occupied)
						break;
				}
			}
			object_index = *(long *)(object + 0x3a4);
		} while (object_index != NONE);
		if (selected != NONE)
		{
			*seat_index = best_seat;
			result = selected;
		}
	}
	return result;
}

// @retail 0x211830
bool function_211830(long filter_range, long object_index, long seat_index)
{
	bool result = false;
	long const *seat_reference = &seat_index;
	long definition_index = object_get(object_index)->tag_index;

	if (filter_range == NONE)
	{
		result = true;
	}
	else
	{
		long first = filter_range & 0xffff;
		long count = (dword)filter_range >> 16;
		for (long i = 0; i < count; i++)
		{
			s_seat_filter *filter = &((s_scenario_seat_filters_view *)g_4e0350)->seat_filters[first + i];

			if (filter->definition_index == definition_index)
			{
				result = (filter->seats[*seat_reference >> 5] & (1 << (*seat_reference & 0x1f))) != 0;
				break;
			}
		}
	}
	return result;
}

// @retail 0x2116f0
long function_2116f0(long unit_index, long filter_range, long seat_type, long occupancy, s_object_seat *results, long maximum_count)
{
	long result_count = 0;
	s_object_seat seats[64];
	short seat_count = 0;

	function_c8a40(unit_index, seats, &seat_count, 64);
	for (long i = 0; i < seat_count && result_count < maximum_count; i++)
	{
		s_object_seat *seat = &seats[i];
		bool valid;

		if (!function_211830(filter_range, seat->object_index, seat->seat_index))
			continue;

		switch (seat_type)
		{
		case 0:
			valid = !TEST_FIELD_BIT(seat->definition->flags.bit2);
			break;
		case 1:
			valid = TEST_FIELD_BIT(seat->definition->flags.bit3);
			break;
		case 2:
			valid = !TEST_FIELD_BIT(seat->definition->flags.bit2) && !TEST_FIELD_BIT(seat->definition->flags.bit3);
			break;
		case 3:
			valid = TEST_FIELD_BIT(seat->definition->flags.bit2);
			break;
		default:
			valid = true;
			break;
		}
		if (!valid)
			continue;

		switch (occupancy)
		{
		case 1:
			valid = function_c8f60(seat->object_index, seat->seat_index) != NONE;
			break;
		case 2:
			valid = function_c8f60(seat->object_index, seat->seat_index) == NONE;
			break;
		default:
			valid = true;
			break;
		}
		if (!valid)
			continue;

		results[result_count] = *seat;
		result_count++;
	}

	return result_count;
}


bool swarm_creature_get_pending_velocity(long object_index, point3f *position);

// @retail 0x211680
bool function_211680(long object_index, point3f *position)
{
    point3f *const *position_reference = &position;
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    bool result = false;
    if (!*(long *)(object + 0x134))
        result = swarm_creature_get_pending_velocity(object_index, *position_reference);
    return result;
}

void __stdcall swarm_creature_convert_parent_unit(long object_index);

/* the animation state's note that a creature's parent unit became an actor:
   hands the unit to the creature's swarm code (its two extra arguments are
   the animation state's fields, which the conversion ignores) */
// @retail 0x2116c0
void function_2116c0(long object_index, long arg_1, long arg_2)
{
    /* taking the address of the two unused arguments keeps retail's
       code; without them the callee cleans no stack */
    long const *arg_1_reference = &arg_1;
    long const *arg_2_reference = &arg_2;
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    if (!*(long *)(object + 0x134))
        swarm_creature_convert_parent_unit(object_index);
}
