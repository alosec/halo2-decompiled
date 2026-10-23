// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_257160.CPP: slot handler 0x79 (handler at 0x47fa48) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"
#include "props.h"
#include "object_markers.h"

/* the slot state of handler 0x79 */
struct s_slot_79_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[0x40 - 0xe];
};

/* what function_1e4b10 returns for the actor's character */
struct s_character_79_view
{
	byte unknown0[4];
	real threshold;
	real duration;
};

/* the actor's value at +0x318 */
struct s_actor_79_view
{
	byte unknown000[0x318];
	real unknown318;
};

short g_470c48 = -1;
short g_470c4c = -2;

long function_1e4b10(long index);
bool function_10f630(long object_index, long *first, long *second);
bool function_e68c0(long type, long unit_index);

short __stdcall function_257160(long actor_index);
short __stdcall function_257290(long actor_index, s_slot *slot, bool active);
bool __stdcall function_2571e0(long actor_index, s_slot *slot);

s_slot_handler_2 g_47fa48 =
{
	{
		0x79, 2, NONE, -2, 0,
		function_257160, function_257290, function_2571e0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, slot_proc_nothing
};

short g_470c50 = -1;
short __stdcall function_257320(long arg_0, s_slot *arg_1);
s_slot_handler_0 g_47fa94 = {0x7b, 0, 0x7ff, -2, 0, function_257320};
bool function_2675b0(long arg_0);
void *function_1e5280(long actor_index, long key);
short actor_behavior_find_level(long owner_index, short type);
bool function_1ff7d0(long actor_index, point3f const *point, real enemy_radius, real friendly_radius, short *count_out);

// @retail 0x257320
short __stdcall function_257320(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->prop_index != NONE && *(short *)((byte *)local_0 + 0x6fe) != 2)
	{
		byte *local_1 = (byte *)object_get(local_0->unknown018);
		s_prop_datum *local_2 = prop_ref_get(local_0->prop_index);
		s_type_5cfb45 *local_3 = function_25d690(local_2);
		byte *local_4 = NULL;
		byte *local_5 = NULL;
		byte *local_6 = NULL;
		short local_7 = *(char *)(local_1 + 0x212);
		bool local_8 = local_3->unknown3c != NONE || function_2675b0(local_0->prop_index);
		point3f const *local_12 = &local_3->position;
		real local_9 = local_12->x - local_0->position.x;
		real local_10 = local_12->y - local_0->position.y;
		real local_11 = (real)sqrt(local_10 * local_10 + local_9 * local_9);
		s_object_marker local_13;
		long local_14 = *(long *)((byte *)object_get(local_0->unknown018) + 0x218);
		if (local_14 != NONE && function_b8d30(local_0->unknown018, 0xb000683, &local_13, 1, false) > 0)
			local_4 = (byte *)function_1e5280(arg_0, object_get(local_14)->tag_index);
		long local_17 = *(long *)((byte *)object_get(local_0->unknown018) + 0x21c);
		if (local_17 != NONE && function_b8d30(local_0->unknown018, 0xf000684, &local_13, 1, false) > 0)
			local_5 = (byte *)function_1e5280(arg_0, object_get(local_17)->tag_index);
		local_14 = *(long *)((byte *)object_get(local_0->unknown018) + 0x21c);
		if (local_14 != NONE && function_b8d30(local_0->unknown018, 0xe000685, &local_13, 1, false) > 0)
			local_6 = (byte *)function_1e5280(arg_0, object_get(local_14)->tag_index);
		long local_15;
		if (local_4 && ((local_8 && (!local_6 || *(real *)(local_6 + 0x10) > local_11 ||
			(local_7 == 0 && *(real *)(local_6 + 0x10) * 1.5f > local_11))) ||
			(!local_5 && (!local_6 || *(real *)(local_6 + 0x10) > local_2->unknown28))))
		{
			local_15 = 0;
		}
		else if (local_6 && (local_11 >= *(real *)(local_6 + 0x10) &&
			(actor_behavior_find_level(arg_0, 0x1e) != NONE || local_8) || !local_5))
		{
			local_15 = 1;
		}
		else if (local_5)
		{
			local_15 = 2;
			if (local_6)
			{
				short local_16 = 0;
				if (function_1ff7d0(arg_0, local_12, 2.5f, 3.f, &local_16) && local_16 > 3)
					local_15 = 1;
			}
		}
		else
		{
			goto done;
		}
		local_1[0x216] = (byte)local_15;
		local_1[0x217] = 0xff;
	}
done:
	return g_470c50;
}

// @retail 0x257160
short __stdcall function_257160(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;

	if (!function_110ab0(unit_index) && !actor->unknown264)
	{
		s_character_79_view *character = (s_character_79_view *)function_1e4b10(actor->unknown054);

		if (character)
		{
			real threshold = 0.35f;
			if (character->threshold > 0.f)
			{
				threshold = character->threshold;
			}

			if (((s_actor_79_view *)actor)->unknown318 > threshold)
			{
				result = 3;
			}
		}
	}
	return result;
}

// @retail 0x2571e0
bool __stdcall function_2571e0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_79_state *state = (s_slot_79_state *)slot;
	s_character_79_view *character = (s_character_79_view *)function_1e4b10(actor->unknown054);

	if (character)
	{
		real ticks;
		long rounded;

		((s_actor_79_view *)actor)->unknown318 = 0.f;
		if (character->duration > 0.f)
		{
			ticks = g_510c54->field_2_3 * character->duration;
			__asm
			{
				fld ticks
				fistp rounded
			}
			state->timer = (short)rounded;
			return true;
		}

		ticks = g_510c54->field_2_3 * 5.f;
		__asm
		{
			fld ticks
			fistp rounded
		}
		state->timer = (short)rounded;
	}
	return true;
}

// @retail 0x257290
short __stdcall function_257290(long actor_index, s_slot *slot, bool active)
{
	short result = g_470c4c;
	s_actor_view *actor = actor_get(actor_index);
	s_slot_79_state *state = (s_slot_79_state *)slot;
	long first;
	long second;

	if (function_10f630(actor->unknown018, &first, &second))
	{
		if (state->timer > 0)
		{
			state->timer--;
		}

		if (second != 0xc0006b3)
		{
			if (state->timer <= 0)
			{
				return g_470c48;
			}
			function_e68c0(0x34, actor->unknown018);
		}
	}
	return result;
}
