// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_1fb7e0.h"

/* slot group 0x38 and the slot test 0x28 */

struct s_slot_38
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	short unknown14;
	byte unknown16[0x40 - 0x16];
};

short __stdcall function_1b75a0(long actor_index);
bool __stdcall function_1b5700(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
short actor_behavior_find_level(long owner_index, short type);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

// @retail 0x1b56b0
short __stdcall function_1b56b0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown344 == NONE)
	{
		s_slot_38 *state = (s_slot_38 *)slot;

		if (state->unknown10 != NONE)
			actor->unknown344 = state->unknown10;
		else
			result = g_46fbe4;
	}
	return result;
}

// @retail 0x1b5700
bool __stdcall function_1b5700(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_38 *state = (s_slot_38 *)slot;
	short type;

	if (state->unknown0c)
	{
		actor->unknown344 = state->unknown10;
		type = state->unknown14;
	}
	else
	{
		actor->unknown344 = actor->prop_index;
		state->unknown10 = actor->prop_index;
		type = 0x3d;
	}
	if (type == NONE)
		type = 0x3d;
	if (actor->unknown344 != NONE)
		function_1fb7e0(type, actor_index, NULL, prop_node_get(actor->unknown344)->object_index, NONE);
	return true;
}

// @retail 0x1b5790
void __stdcall function_1b5790(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown344 = NONE;
}

// @retail 0x1b57c0
short __stdcall function_1b57c0(long actor_index, short level, bool active)
{
	actor_get(actor_index)->unknown084 = 5;
	return function_1a79e0(actor_index, level, active);
}

/* walks the actors of a clump (an element of g_502420) */
struct s_clump_iterator_1b57f0
{
	long index;
	long next_index;
};

static inline s_actor_view *clump_actor_iterator_advance_1b57f0(s_clump_iterator_1b57f0 *iterator)
{
	s_actor_view *member = 0;

	if (iterator->next_index != NONE)
	{
		member = actor_get(iterator->next_index);
		iterator->index = iterator->next_index;
		iterator->next_index = member->next_index;
	}
	return member;
}

/* sends the nearest clump member at the actor's prop (in slot group 0x22 or
   0x1b) to slot 0xb, unless a near one is already in it */
// @retail 0x1b57f0
short __stdcall function_1b57f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && actor->unknown07c != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && view->unknown70 == 0)
		{
			real best_weight = 0.0f;
			long best_index = NONE;
			s_clump_iterator_1b57f0 iterator;
			s_actor_view *other;

			iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
			while ((other = clump_actor_iterator_advance_1b57f0(&iterator)) != 0)
			{
				long other_index = iterator.index;

				if (other == actor || other->prop_index == NONE)
					continue;

				s_prop_node_view *other_node = prop_node_get(other->prop_index);

				if (other_node->unknown08 != node->unknown08)
					continue;
				if (actor_behavior_find_level(other_index, 0x22) == NONE && actor_behavior_find_level(other_index, 0x1b) == NONE)
					continue;

				s_prop_view_fields *other_view = prop_node_view(other_node);

				if (!other_view || other_view->unknown70 != 0)
					continue;

				real distance = distance3d(&actor->position, &other->position);

				if (!(distance < 4.0f))
					continue;
				if (actor_behavior_find_level(other_index, 0xb) != NONE)
					return g_46fbe4;

				real weight = 1.0f / (distance + 1.0f);

				if (weight > best_weight)
				{
					best_weight = weight;
					best_index = other_index;
				}
			}
			if (best_index != NONE)
			{
				s_actor_view *best = actor_get(best_index);

				best->unknown3cc = actor_index;
				best->unknown3d0 = 0x24;
				best->unknown3d2 = 0x92;
				function_1a8220(best_index, 0xb, g_510c54->field_2_3, 3, 0x22, 0x23, 3);
			}
		}
	}
	return g_46fbe4;
}

/* the children of slot group 0x38 */
s_slot_child g_46f7f0[4] =
{
	{0x39, 1, -2, {0}, -1.0f, 0, 0},
	{0x3a, 0, NONE, {0}, 0.0f, 0, 0},
	{0x4a, 0, NONE, {0}, 0.0f, 0, 0},
	{0x3b, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47e4f8 =
{
	{
		0x38, 1, 0, -2, 0,
		function_1b75a0, function_1b56b0, function_1b5700, function_1b5790, 1, {0},
		0, function_1c1520, 0, 0, 0, 0, 0
	},
	function_1b57c0, 4, g_46f7f0
};

s_slot_handler_0 g_47e594 =
{
	0x28, 0, 0x7ff, -2, 0, function_1b57f0
};
