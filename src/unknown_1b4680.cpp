// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"

/* slot group 0x1b and the slot tests 0x44, 0x43, 0x3d, 0x40, 0x3c, 0x41,
   0x3e, 0x3f, 0x4a and 0x39 */

struct s_slot_1b
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long element_index;
	byte unknown14[0x1c - 0x14];
	real unknown1c;
	short unknown20;
	byte unknown22[0x24 - 0x22];
	long unknown24;
	short unknown28;
	short unknown2a;
	byte unknown2c[0x40 - 0x2c];
};

/* the block of the actor's character tag function_1e4e50 returns */
struct s_character_e50
{
	byte unknown00[4];
	real unknown04;
	real unknown08;
	real unknown0c;
	real unknown10;
	real unknown14;
};

/* a joint (an element of g_502424) as slot group 0x1b reads it */
struct s_joint_participant_1b
{
	long actor_index;
	short status;
	short priority;
	real score;
};

struct s_joint_1b
{
	short salt;
	short state;
	s_joint_participant_1b participants[10];
	short participant_count;
	byte unknown7e[2];
	long unknown80;
	short unknown84;
	short unknown86;
	long prop_index;
	bool unknown8c;
	byte unknown8d[0xbc - 0x8d];
};
typedef char assert_s_joint_1b_size[(sizeof(s_joint_1b) == 0xbc) ? 1 : -1];

point3f *function_b9dd0(long object_index, point3f *result);

/* the state the slot tests fill in for slot type 0x38 */
struct s_slot_38
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	short unknown14;
	byte unknown16[0x40 - 0x16];
};

short __stdcall function_1b4680(long actor_index);
short __stdcall function_1b4bd0(long actor_index, s_slot *slot, bool active);
long __stdcall function_1b47b0(long actor_index, s_slot *slot);
/* what slot group 0x1b's release callback gets */
struct s_1b4d90_group
{
	byte unknown00[0x88];
	long unknown88;
	bool unknown8c;
};

void __stdcall function_1b4d90(long actor_index, s_slot *slot, s_1b4d90_group *group);
bool function_26ba60(long prop_index, long actor_index, long clump_index);
short __stdcall function_1b4d10(long actor_index, short level, long a, long b);
short __stdcall function_1b4e70(long actor_index, long leader_index, long a, long b);
short __stdcall function_1b4fe0(long actor_index, s_slot *slot);
short __stdcall function_1b50e0(long actor_index, s_slot *slot);
short __stdcall function_1b5180(long actor_index, s_slot *slot);
short __stdcall function_1b51f0(long actor_index, s_slot *slot);
short __stdcall function_1b52a0(long actor_index, s_slot *slot);
short __stdcall function_1b53a0(long actor_index, s_slot *slot);
short __stdcall function_1b5470(long actor_index, s_slot *slot);
short __stdcall function_1afde0(long actor_index);
short __stdcall function_1b54d0(long actor_index, s_slot *slot);

/* the block of the actor's character tag function_1e4ef0 returns */
struct s_character_ef0
{
	byte unknown00[4];
	real unknown4;
	real unknown08;
	real unknown0c;
	real unknown10;
	byte unknown14[0x2c - 0x14];
	real unknown2c;
	real unknown30;
	real unknown34;
};

void *function_1e4ef0(long actor_index);
void *function_1e4e50(long actor_index);
short function_1a77a0(long actor_index, long a, short level);

#pragma inline_depth(0)
/* 3 when the actor's prop view is of type 5 or a slot group 0x1b joint of
   the actor targets the prop's object, else 0 */
// @retail 0x1b4680
short __stdcall function_1b4680(long actor_index)
{
	short result = 0;
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));

	if (actor->unknown225 || actor->unknown223)
		result = 0;
	else if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + (actor->prop_index & 0xffff) * sizeof(s_prop_node_view));
		s_prop_view_fields *view = (s_prop_view_fields *)function_25d740((s_prop_node *)node);
		bool found = false;

		if (view && !view->unknown68 && function_1e4e50(actor_index))
		{
			if (view->unknown00 == 5)
				found = true;
			else
			{
				s_slot_entry_iterator iterator;
				s_slot_memory_entry *entry;

				iterator.actor_index = actor_index;
				iterator.reference.unknown2 = 0x1b;
				iterator.reference.unknown0 = NONE;
				for (entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
				{
					s_502424_element *joint = (s_502424_element *)(g_502424->data + (entry->unknown4 & 0xffff) * sizeof(s_502424_element));

					if (joint->target.unknown8 != NONE && ((s_prop_node_view *)(g_502418->data + (joint->target.unknown8 & 0xffff) * sizeof(s_prop_node_view)))->object_index == node->object_index)
					{
						found = true;
						break;
					}
				}
			}
		}
		if (found)
			result = 3;
	}
	return result;
}
#pragma inline_depth()

long function_25d810(long object_index, long actor_index, bool create);
bool __stdcall function_25d020(long actor_index, long object_index, long type, long prop_ref_argument,
	long reference_actor_index, long reference_index);

/* joins the slot group 0x1b joint the actor was invited to about its prop,
   or starts one, then sets the slot's timers; retail calls the random range
   (0x259d0) and the prop view (0x25d740) here instead of inlining them */
// @retail 0x1b47b0
long __stdcall function_1b47b0(long actor_index, s_slot *slot)
{
	long element_index = NONE;
	s_slot_1b *state = (s_slot_1b *)slot;
	s_actor_view *actor = actor_get(actor_index);
	s_character_e50 *character = (s_character_e50 *)function_1e4e50(actor_index);

	if (actor->prop_index != NONE && character)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_slot_entry_iterator iterator;
		s_slot_memory_entry *entry;

		iterator.actor_index = actor_index;
		iterator.reference.unknown2 = 0x1b;
		iterator.reference.unknown0 = NONE;
		for (entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
		{
			s_502424_element *element = element_502424_get(entry->unknown4);

			if (element->target.unknown8 != NONE && prop_node_get(element->target.unknown8)->object_index == node->object_index &&
				function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
			{
				element_index = entry->unknown4;
				break;
			}
		}
		if (element_index == NONE)
		{
			long created = function_26e940(actor_index);

			if (created != NONE)
			{
				state->unknown0c = true;
				state->element_index = created;
			}
			element_index = created;
		}
		if (element_index != NONE)
		{
			s_joint_1b *joint = (s_joint_1b *)element_502424_get(element_index);

			if (state->unknown0c)
			{
#pragma inline_depth(0)
				real first = function_259d0(&g_4e7408->unknown0, NULL, 0, character->unknown04, character->unknown08);
				real second = function_259d0(&g_4e7408->unknown0, NULL, 0, character->unknown0c, character->unknown10);
#pragma inline_depth()

				state->unknown1c = (real)g_510c54->game_time;
				real ticks = g_510c54->field_2_3 * first;
				long rounded;

				__asm
				{
					fld ticks
					fistp rounded
				}
				state->unknown28 = (short)rounded;
				ticks = g_510c54->field_2_3 * second;
				__asm
				{
					fld ticks
					fistp rounded
				}
				state->unknown2a = (short)rounded;
				joint->unknown80 = (long)state->unknown1c;
				joint->unknown84 = state->unknown28;
				joint->unknown86 = state->unknown2a;
				joint->prop_index = actor->prop_index;
				joint->unknown8c = false;
			}
			else
			{
				state->unknown1c = (real)joint->unknown80;
				state->unknown28 = joint->unknown84;
				state->unknown2a = joint->unknown86;

				s_prop_state_view *node_state = prop_node_state(node);
#pragma inline_depth(0)
				s_prop_view_fields *view = (s_prop_view_fields *)function_25d740((s_prop_node *)node);
#pragma inline_depth()
				long target_index = joint->prop_index;

				if (target_index != NONE)
				{
					s_prop_node_view *target = prop_node_get(target_index);
					s_prop_state_view *target_state = prop_node_state(target);

					if (target_state->unknown00 > node_state->unknown00)
					{
						function_25d020(actor_index, node->object_index, NONE, actor->prop_index, target->unknown04, target_index);
						if (view)
						{
#pragma inline_depth(0)
							s_prop_view_fields *target_view = (s_prop_view_fields *)function_25d740((s_prop_node *)target);
#pragma inline_depth()

							if (target_view)
								view->unknown18 = target_view->unknown18;
						}
						goto done;
					}
					if (target_state->unknown00 >= node_state->unknown00)
						goto done;
				}

				for (short i = 0; i < 10; i++)
				{
					long other_index = joint->participants[i].actor_index;

					if (other_index != NONE && joint->participants[i].status == 3)
					{
						long other_prop = function_25d810(node->object_index, other_index, false);

						if (other_prop != NONE)
						{
							function_25d020(other_index, node->object_index, NONE, other_prop, actor_index, actor->prop_index);
							if (view && view->unknown10 != NONE)
							{
								s_prop_view_fields *other_view = prop_view_fields_get(other_prop);

								if (other_view)
									other_view->unknown18 = view->unknown18;
							}
						}
					}
				}
				joint->prop_index = actor->prop_index;
			}
done:
			real seconds = (slot_random() + 1.0f) * 2.0f;
			real delay = g_510c54->field_2_3 * seconds;
			long delay_ticks;

			__asm
			{
				fld delay
				fistp delay_ticks
			}
			state->unknown20 = (short)delay_ticks;
			state->unknown24 = actor->prop_index;
		}
	}
	return element_index;
}

// @retail 0x1b4bd0
short __stdcall function_1b4bd0(long actor_index, s_slot *slot, bool active)
{
	s_slot_1b *state = (s_slot_1b *)slot;
	short result = g_46fbe8;
	real elapsed = (real)g_510c54->game_time - state->unknown1c;
	s_actor_view *actor = actor_get(actor_index);
	s_character_e50 *character = (s_character_e50 *)function_1e4e50(actor_index);
	long prop_index = actor->prop_index;

	bool far = true;

	if (prop_index != NONE && character && !(elapsed > (real)state->unknown2a))
	{
		far = false;
		if (elapsed > (real)state->unknown28)
		{
			s_prop_node_view *node = prop_node_get(prop_index);
			point3f position;

			function_b9dd0(node->object_index, &position);
			if (distance3d(&prop_node_state(node)->position, &position) > character->unknown14)
				far = true;
		}
	}
	if (far)
		result = g_46fbe4;

	if (result == g_46fbe8 && state->unknown20 > 0 && prop_index != NONE)
	{
		if (--state->unknown20 == 0)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (function_26ba60(node->unknown08, actor_index, actor->unknown07c))
				function_1fb7e0(0x2a, actor_index, NULL, node->object_index, NONE);
		}
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x1b4d10
short __stdcall function_1b4d10(long actor_index, short level, long a, long b)
{
	short result = g_46fbe4;
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));

	actor->unknown41c = 3;
	actor->unknown420 = 2;
	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = (s_prop_view_fields *)function_25d700(actor->prop_index);

		if (view->unknown6d)
		{
			view->unknown6d = false;
			actor->slots[level].unknown4 = NONE;
		}
		result = function_1a77a0(actor_index, a, level);
	}
	return result;
}
#pragma inline_depth()

static inline s_actor_view *member_next_1b4e70(long &next, long &current)
{
	s_actor_view *member = 0;
	if (next != NONE)
	{
		member = actor_get(next);
		current = next;
		next = member->next_index;
	}
	return member;
}

// @retail 0x1b4e70
short __stdcall function_1b4e70(long actor_index, long leader_index, long a, long b)
{
	s_actor_view *actor = actor_get(actor_index);
	long count = 0;

	if (actor->unknown07c != NONE)
	{
		long index = element_502420_get(actor->unknown07c)->first_actor_index;

		s_actor_view *other;
		long other_index;
		while ((other = member_next_1b4e70(index, other_index)) != 0)
		{
			if (actor != other && function_26eae0(leader_index, other_index, 3, 1.0f))
				count++;
		}
	}
	return (short)count;
}

// @retail 0x1b4f10
void __stdcall function_1b4f10(long actor_index, s_slot *slot, long index)
{
	s_slot_1b *state = (s_slot_1b *)slot;

	if (state->unknown24 == index)
		state->unknown24 = NONE;
	if (state->element_index != NONE)
	{
		s_502424_element *element = element_502424_get(state->element_index);

		if (element->target.unknown8 == index)
			element->target.unknown8 = NONE;
	}
}

// @retail 0x1b4f60
short __stdcall function_1b4f60(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (actor_get(actor_index)->unknown6fc & 1)
		result = 0x38;
	return result;
}

// @retail 0x1b4f90
short __stdcall function_1b4f90(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->unknown3b0 > 0 && actor->unknown3b4 != NONE)
	{
		s_slot_38 *state = (s_slot_38 *)slot;

		state->unknown10 = actor->unknown3b4;
		state->unknown14 = NONE;
		state->unknown0c = true;
		result = 0x38;
	}
	return result;
}

PRIVATE __forceinline word function_1b50e1(s_slot *arg_0, long arg_1)
{
	short result = g_46fbe4;
	s_actor_view *actor = actor_get(arg_1);
	s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(arg_1);

	if (actor->prop_index != NONE && character && actor->unknown3d8 >= character->unknown0c)
	{
		s_slot_handler *handler = g_46eeb8[0x38];

		if (handler->unknown8 == g_46f348 || (handler->mask & g_4ee4ec) != g_4ee4ec ||
			(((byte *)g_557c40)[0x38 >> 3] & (1 << (0x38 & 7))) == 0)
		{
			return result;
		}

		s_slot_38 *state = (s_slot_38 *)arg_0;

		state->unknown10 = actor->prop_index;
		state->unknown14 = 0x3d;
		state->unknown0c = true;
		return 0x38;
	}
	return result;
}

// @retail 0x1b50e0
short __stdcall function_1b50e0(long actor_index, s_slot *slot)
{
	return (short)function_1b50e1(slot, actor_index);
}

// @retail 0x1b5180
short __stdcall function_1b5180(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;
	s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(actor_index);

	if (character && character->unknown4 >= actor->unknown2d4)
	{
		s_slot_38 *state = (s_slot_38 *)slot;

		state->unknown10 = actor->prop_index;
		state->unknown14 = 0x41;
		state->unknown0c = true;
		result = 0x38;
	}
	return result;
}

// @retail 0x1b51f0
short __stdcall function_1b51f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;
	s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(actor_index);

	if (character)
	{
		long prop_index = actor->prop_index;

		if (prop_index != NONE && !actor->unknown223)
		{
			s_prop_node_view *node = prop_node_get(prop_index);

			if (node->unknown24 >= 1 && node->unknown24 <= 2 && character->unknown10 > node->unknown28)
			{
				s_slot_38 *state = (s_slot_38 *)slot;

				state->unknown10 = prop_index;
				state->unknown14 = 0x40;
				state->unknown0c = true;
				result = 0x38;
			}
		}
	}
	return result;
}

/* the actor types (unknown_25d690.cpp): a name, then the type that leads
   this one */
struct s_actor_type_definition
{
	char const *name;
	short unknown4;
	short leader_type;
};

extern s_actor_type_definition *g_471088[16];

/* unless a clump member is of the type that leads the actor's type */
PRIVATE __forceinline real function_1b52a1(void)
{
    s_random_globals *local_0 = g_4e7408;
    dword *local_1 = &local_0->unknown0;
    *local_1 = 1664525 * *local_1 + 1013904223;
    return (real)(*local_1 >> 16) * (1.f / 65535.f);
}

// @retail 0x1b52a0
short __stdcall function_1b52a0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_actor_type_definition *type = g_471088[actor->unknown004];
	short result = 0x38;
	long index = element_502420_get(actor->unknown07c)->first_actor_index;

	s_actor_view *other;
	long other_index;
	while ((other = member_next_1b4e70(index, other_index)) != 0)
	{
		if (other->unknown004 == type->leader_type)
		{
			result = g_46fbe4;
			goto done;
		}
	}

	if (result != g_46fbe4)
	{
		s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(actor_index);

		bool accepted = true;
		if (character)
			accepted = character->unknown2c > function_1b52a1();
		if (accepted)
		{
			s_slot_38 *state = (s_slot_38 *)slot;

			state->unknown10 = actor->prop_index;
			state->unknown14 = 0x3f;
			state->unknown0c = true;
		}
		else
			result = g_46fbe4;
	}
done:
	return result;
}

// @retail 0x1b53a0
short __stdcall function_1b53a0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0x38;

	if (result != g_46fbe4)
	{
		real chance = 1.0f;
		s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(actor_index);

		if (character)
		{
			if (g_510c54->game_time - actor->unknown2fc < 2 * g_510c54->field_2_3)
				chance = character->unknown34;
			else
				chance = character->unknown30;
		}
		if (!(chance > slot_random()))
			result = g_46fbe4;
	}
	actor->unknown2fc = g_510c54->game_time;
	return result;
}

// @retail 0x1b5470
short __stdcall function_1b5470(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (function_1afde0(actor_index) >= 3 && g_46eeb8[0x3a]->unknown8 == g_46f348)
	{
		actor_get(actor_index)->unknown222 = true;
		result = 0x46;
	}
	return result;
}

/* the children of slot group 0x1b */
s_slot_child g_46f758[6] =
{
	{0x20, 0, -2, {0}, -1.0f, 0, 0},
	{0x1f, 0, 7, {0}, 1.0f, 0, 0},
	{0x1d, 0, NONE, {0}, 0.0f, 0, 0},
	{0x1c, 0, NONE, {0}, 0.0f, 0, 0},
	{0x1e, 0, NONE, {0}, 0.0f, 0, 0},
	{5, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1x g_47e3c8 =
{
	{
		{
			0x1b, 1, 0, -2, 0,
			function_1b4680, function_1b4bd0, joint_initiate_b, joint_leave, NONE, {0},
			0, function_1b4f10, 0, 0, 0, 0, 1
		},
		function_26e8a0, 6, g_46f758
	},
	(t_slot_proc)function_1b47b0, (t_slot_release)function_1b4d90, (t_slot_proc4)function_1b4d10, (t_slot_proc4)function_1b4e70,
	1, 10, 50.0f
};

s_slot_handler_0 g_47e42c =
{
	0x44, 0, 0, -2, 0, function_1b4f60
};

s_slot_handler_0 g_47e440 =
{
	0x43, 0, 0x7ff, -2, 0, function_1b4f90
};

s_slot_handler_0 g_47e454 =
{
	0x3d, 0, 0, -2, 0, function_1b4fe0
};

s_slot_handler_0 g_47e468 =
{
	0x40, 0, 0, -2, 0, function_1b50e0
};

s_slot_handler_0 g_47e47c =
{
	0x3c, 0, 0, -2, 0, function_1b5180
};

s_slot_handler_0 g_47e490 =
{
	0x41, 0, 0, -2, 0, function_1b51f0
};

s_slot_handler_0 g_47e4a4 =
{
	0x3e, 0, 0x7ff, -2, 0, function_1b52a0
};

s_slot_handler_0 g_47e4b8 =
{
	0x3f, 0, 0x7ff, -2, 0, function_1b53a0
};

s_slot_handler_0 g_47e4cc =
{
	0x4a, 0, 0, -2, 0, function_1b5470
};

s_slot_handler_0 g_47e4e0 =
{
	0x39, 0, NONE, -2, 0, function_1b54d0
};

// @retail 0x1b4d90
void __stdcall function_1b4d90(long actor_index, s_slot *slot, s_1b4d90_group *group)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view)
		{
			view->unknown68 = true;
			if (node->unknown27 < 2 && !group->unknown8c &&
				function_26ba60(node->unknown08, actor_index, actor->unknown07c))
			{
				function_1fb7e0(0x34, actor_index, NULL, node->object_index, NONE);
				group->unknown8c = true;
			}
		}
	}

	if (group->unknown88 != NONE && prop_node_get(group->unknown88)->unknown04 == actor_index)
		group->unknown88 = NONE;
}

// @retail 0x1b4fe0
short __stdcall function_1b4fe0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));

		if (view)
		{
			s_character_ef0 *character = (s_character_ef0 *)function_1e4ef0(actor_index);

			if (character && character->unknown08 > 0.0f && view->unknown60 >= character->unknown08 && view->unknown54 >= 0.8f)
			{
				s_slot_handler *handler = g_46eeb8[0x38];

				if (handler->unknown8 != g_46f348 && (handler->mask & g_4ee4ec) == g_4ee4ec &&
					(((byte *)g_557c40)[0x38 >> 3] & (1 << (0x38 & 7))) != 0)
				{
					s_slot_38 *state = (s_slot_38 *)slot;

					state->unknown10 = actor->prop_index;
					state->unknown14 = 0x3e;
					state->unknown0c = true;
					result = 0x38;
				}
			}
		}
	}

	return result;
}


real function_30bf0(vector3f *vector);
real function_259a0(dword *seed);
void *function_1e5380(long actor_index);
bool function_255b10(long actor_index, s_type_c3b527 const *point, long target_index, bool unknown);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1b54d0
short __stdcall function_1b54d0(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    if (actor->unknown344 != NONE)
    {
        byte *entry = (byte *)function_1e5380(actor_index);
        s_prop_node_view *node = prop_node_get(actor->unknown344);
        if (g_4f55d0->unknown340 && entry && *(short *)(entry + 6) != NONE &&
            *(signed char *)((byte *)node + 0x27) >= 1 && node->unknown28 > *(real *)(entry + 0x18) &&
            *(short *)((byte *)actor + 0x7c4) <= 0 &&
            (*(long *)((byte *)actor + 0x7c8) == NONE ||
             (g_510c54->game_time - *(long *)((byte *)actor + 0x7c8)) * g_510c54->rate >= *(real *)(entry + 0x28)))
        {
            byte *character = (byte *)function_1e4ef0(actor_index);
            if (function_259a0(&g_4e7408->unknown0) < *(real *)(character + 0x40) && actor->unknown07c != NONE)
            {
                s_prop_state_view *state = (s_prop_state_view *)function_25d690((s_prop_datum *)node);
                vector3f direction;
                vector3d_from_points3d(&actor->position, (point3f *)((byte *)state + 4), &direction);
                real distance = function_30bf0(&direction);
                if (distance > 0.0f)
                {
                    real range = *(real *)(entry + 0x18);
                    distance = distance > range * 2.0f ? distance * 0.5f : *(real const volatile *)(entry + 0x18);
                    s_type_c3b527 point;
                    point.point.x = actor->position.x + direction.i * distance;
                    point.point.y = actor->position.y + direction.j * distance;
                    _ReadWriteBarrier();
                    point.point.z = actor->position.z + direction.k * distance;
                    point.output_index = NONE;
                    function_255b10(actor_index, &point, NONE, false);
                }
            }
        }
    }
    return g_46fbe4;
}
#pragma function(_ReadWriteBarrier)
