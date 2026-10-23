// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FB940.CPP: the clusters an event in one cluster reaches (with
   event_dispatch_group's 0x1fbac0..0x1fc210) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_249e20.h"
#include "unknown_25d020.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"
#include "unknown_26b230.h"
#include <string.h>

/* the clusters near the last cluster asked about, one bit each */
dword g_4f5728[0x10];
extern short g_4f5768; /* ai.cpp resets it */

#define MACRO_46A44D(count) (((count) + 31) >> 5)
#define BIT_VECTOR_SIZE_IN_BYTES(count) (4 * MACRO_46A44D(count))

/* the clusters that hear the cluster: not cut off from it, and nearer than
   40 world units */
// @retail 0x1fb940
dword *function_1fb940(short cluster_index)
{
	if (cluster_index != g_4f5768)
	{
		s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
		short i;

		memset(g_4f5728, 0, BIT_VECTOR_SIZE_IN_BYTES(bsp->cluster_count));
		for (i = 0; i < bsp->cluster_count; i++)
		{
			if (!function_249c20(bsp, cluster_index, i) && function_249d60(bsp, cluster_index, i) < 40.0f)
				g_4f5728[i >> 5] |= 1 << (i & 31);
		}
		g_4f5768 = cluster_index;
	}

	return g_4f5728;
}

struct s_perception_origin_view;
struct s_joint_header;

short function_263ed0(long actor_index, s_perception_origin_view const *origin, point3f const *point,
	s_location const *location, short type, short mode);
bool function_1df560(short team_a, short team_b);
bool __stdcall function_25d020(long actor_index, long object_index, long type,
	long prop_index, long unknown, long unknown2);
void function_26eb70(long actor_index, s_joint_header const *behavior);
void function_25bf10(long actor_index, long other_index, long prop_index);

// @retail 0x1fba10
bool function_1fba10(long actor_index, long source_index, s_location const *location,
	s_1fbac0_event const *event, dword const *clusters, point3f const *point)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (actor_index != source_index && actor->unknown009 && (*(short *)((byte *)actor + 0x254)) != NONE &&
		*(short *)((byte *)actor + 0x3e) == g_4686c4)
	{
		if (!clusters || (clusters[(*(short *)((byte *)actor + 0x254)) >> 5] & (1 << ((*(short *)((byte *)actor + 0x254)) & 31))))
		{
			s_2641c0 origin;
			if (function_2641c0(actor_index, &origin, point))
				result = function_263ed0(actor_index, (s_perception_origin_view const *)&origin,
					point, location, event->unknown02, 0) >= 1;
		}
	}
	return result;
}

struct s_actor_event_request
{
	long type;
	long unknown4;
	long target_index;
};

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1fc160
bool function_1fc160(long actor_index, long source_index, s_actor_event_request const *request)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (!function_1df560(actor_get(actor_index)->unknown024, actor->unknown024))
	{
		switch (request->type)
		{
		case 1:
			result = false;
			break;
		case 2:
			result = true;
			break;
		case 3:
			function_25d020(actor_index, request->target_index, NONE, NONE, NONE, NONE);
			result = true;
			break;
		case 4:
			result = true;
			break;
		case 5:
			function_26eb70(actor_index, (s_joint_header const *)&request->target_index);
			result = true;
			break;
		case 6:
			function_25bf10(actor_index, source_index, NONE);
			result = true;
			break;
		}
	}
	return result;
}

long function_25d810(long object_index, long actor_index, bool create);
void function_25bba0(long actor_index, long prop_index);

// @retail 0x1fc210
void __stdcall function_1fc210(long object_index, long actor_index)
{
	long prop_index = function_25d810(object_index, actor_index, false);
	if (prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(prop_index);
		s_type_5cfb45 *state = (s_type_5cfb45 *)prop_node_state(node);
		short *counter = (short *)((byte *)node + 0x1c);
		if (*counter == NONE || *counter <= 0)
		{
			real seconds;
			long ticks;
			*counter = 0;
			seconds = (real)g_510c54->field_2_3;
			__asm
			{
				fld seconds
				fistp ticks
			}
			*(short *)((byte *)node + 0x1e) = (short)ticks;
		}
		function_25d020(actor_index, object_index, 4, prop_index, NONE, NONE);
		s_prop_view_fields *view = prop_node_view(node);
		if (view)
			*(short *)((byte *)view + 4) = 3;
		function_25bba0(actor_index, prop_index);
		node->unknown26 = 3;
		node->unknown27 = 3;
		state->unknown68 = true;
	}
}

struct s_event_squad
{
	byte unknown0[2];
	char flags;
	byte unknown3[0x98 - 3];
};

void function_25bc20(long actor_index, long object_index);
void function_25bcb0(long actor_index, long prop_index);
void function_25bcf0(long actor_index, long prop_index, long source_index);

// @retail 0x1fbff0
void function_1fbff0(long actor_index, long source_index, long object_index, long prop_index,
	s_1fbac0_event const *event)
{
	long const *prop_reference = &prop_index;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = NULL;

	if (*prop_reference != NONE)
		node = prop_node_get(*prop_reference);
	bool blocked = false;
	if (actor->unknown030 != NONE)
	{
		s_event_squad *squad = (s_event_squad *)(g_51e9d8->data +
			(actor->unknown030 & 0xffff) * sizeof(s_event_squad));
		blocked = (bool)(((dword)squad->flags >> 1) & 1);
	}
	if (actor->unknown009 && !blocked && *(short *)((byte *)actor + 0x3e) == g_4686c4)
	{
		switch (event->unknown00)
		{
		case 0:
			function_1fc160(actor_index, source_index, (s_actor_event_request const *)&event->data);
			break;
		case 1:
			if (node)
			{
				s_type_5cfb45 *state = (s_type_5cfb45 *)prop_node_state(node);
				s_prop_view_fields *view = prop_node_view(node);
				node->unknown26 = 3;
				state->unknown63 = true;
				state->unknown68 = true;
				if (view)
					*(short *)((byte *)view + 2) = 3;
				short *counter = (short *)((byte *)node + 0x1c);
				if (*counter == NONE || *counter <= 1)
				{
					real seconds;
					long ticks;
					*counter = 1;
					seconds = (real)g_510c54->field_2_3;
					__asm
					{
						fld seconds
						fistp ticks
					}
					*(short *)((byte *)node + 0x1e) = (short)ticks;
				}
			}
			function_25bcf0(actor_index, *prop_reference, source_index);
			break;
		case 2:
			break;
		case 3:
			function_25bcb0(actor_index, object_index);
			break;
		case 4:
			function_25bc20(actor_index, object_index);
			break;
		}
	}
}

bool __stdcall function_1df820(volatile short team_a, volatile short team_b, short incident_type);
point3f *function_b9dd0(long object_index, point3f *result);
long function_baf80(long object_index);

// @retail 0x1fbe00
void __stdcall function_1fbe00(long clump_index, long source_index, long object_index,
	bool filter, dword const *clusters, s_1fbac0_event const *event)
{
	s_502420_element *clump = element_502420_get(clump_index);
	s_object_header_view *local_0 = (s_object_header_view *)g_4e0300->data;
	s_slot_object_view *object = (s_slot_object_view *)local_0[object_index & 0xffff].object;
	if (event->unknown00 == 0 && (event->data.unknown00 - 1) == 0)
	{
		function_1df820(*(short const *)((byte const *)event + 0xc),
			*(short const *)((byte const *)event + 0xe),
			*(short const *)((byte const *)event + 0x10));
		return;
	}

	point3f position;
	function_b9dd0(object_index, &position);
	s_location const *location = (s_location const *)((byte *)object + 0x28);
	if (object->parent_index != NONE)
		location = (s_location const *)((byte *)(s_slot_object_view *)local_0[function_baf80(object_index) & 0xffff].object + 0x28);

	long prop_index = function_26b230(clump_index, object_index);
	if (prop_index != NONE)
	{
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data +
			(prop_index & 0xffff) * sizeof(s_clump_prop));
		if (!*(byte *)((byte *)prop + 0x26))
		{
			long node_index = prop->first_node;
			while (node_index != NONE)
			{
				s_clump_node *node = (s_clump_node *)prop_node_get(node_index);
				long actor_index = node->actor_index;
				if (actor_get(actor_index)->unknown009 &&
					(!filter || function_1fba10(actor_index, source_index, location, event, clusters, &position)))
					function_1fbff0(node->actor_index, source_index, object_index, node_index, event);
				node_index = node->next;
			}
		}
	}
	else
	{
		struct actor_step
		{
			static inline s_actor_view *next(long *next_index, long *current_index)
			{
				long index = *next_index;
				if (index == NONE)
					return NULL;
				s_actor_view *actor = actor_get(index);
				*current_index = index;
				*next_index = actor->next_index;
				return actor;
			}
		};
		long next_index = clump->first_actor_index;
		long current_index;
		s_actor_view *actor;
		while ((actor = actor_step::next(&next_index, &current_index)) != NULL)
		{
			if (actor->unknown009 &&
				(!filter || function_1fba10(current_index, source_index, location, event, clusters, &position)))
				function_1fbff0(current_index, source_index, object_index, NONE, event);
		}
	}
}

// BLOCKED-BY the full check (#578, batch r294): enabling 0x1fbac0 loses 25 matched functions under
// whole-program optimisation (0x1acfd0..0x1b6930, 0x1bf4e0, 0x1c9500, 0x1fb7e0, 0x20ba60, 0x25ac00..0x25d420,
// 0x266540); it does not byte-match yet. fecheck: behaves like retail on its in-game snapshots with every
// argument on the stack (retail's callers push all five; ret 0x14). Enabling it also needs the signature
// retail reads: (long actor_index, long unit_index, short mode, bool filter, event), in the declarations in
// unknown_1c95d0.cpp, 1ca130, 1ca690, 1fb7e0 and 20d570. The lane_b stub stays.
#if 0
#include "player_queries.h"
#include <math.h>

s_ai_player *ai_player_get(long player_index);
void function_295320(point3f const *point);

/* the clumps an event reaches: a data iteration over g_502420 that runs only
   while the ai is active */
struct s_clump_walk
{
	s_data_datum_iterator iterator;
	long clump_index;
};

static inline void clump_walk_begin(s_clump_walk *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->iterator.data = g_502420;
		iterator->iterator.index = NONE;
		iterator->iterator.datum_index = NONE;
	}
}

static inline s_502420_element *clump_walk_step(s_clump_walk *iterator)
{
	s_502420_element *clump = NULL;

	if (g_4f55d0->active)
	{
		if (data_datum_iterator_next(&iterator->iterator))
			clump = (s_502420_element *)iterator->iterator.datum;
		iterator->clump_index = iterator->iterator.datum_index;
	}
	return clump;
}

/* a unit (and its actor, when it has one) makes an event: mode 0 tells the
   members of the event's joint, mode 1 the actor's own clump, mode 2 and up
   every clump within reach (30 units, 40 for events other than type 0, plus
   6), skipping clumps of other teams below mode 3 */
// Retail 0x1fbac0, disabled with its blocker above.
void __stdcall function_1fbac0(long actor_index, long unit_index, short mode, bool filter,
	s_1fbac0_event *event)
{
	/* retail keeps every argument on the stack (ret 0x14); its callers push them */
	long const *actor_on_stack = &actor_index;
	long const *unit_index_reference = &unit_index;
	short const *mode_reference = &mode;
	bool const *filter_reference = &filter;
	s_1fbac0_event *const *event_reference = &event;
	s_object_header_view *headers = (s_object_header_view *)g_4e0300->data;
	s_slot_object_view *unit = (s_slot_object_view *)headers[*unit_index_reference & 0xffff].object;
	s_actor_view *actor = NULL;
	short team = NONE;
	point3f position;

	function_b9dd0(unit_index, &position);
	s_location const *location = (s_location const *)((byte *)unit + 0x28);
	if (unit->parent_index != NONE)
		location = (s_location const *)((byte *)(s_slot_object_view *)headers[function_baf80(unit_index) & 0xffff].object + 0x28);

	if (*actor_on_stack != NONE)
	{
		actor = actor_get(actor_index);
		team = actor->unknown024;
	}
	else if (unit->type == 0 && unit->player_index != NONE)
	{
		s_player_datum *player = (s_player_datum *)(g_4e8c24->data + (unit->player_index & 0xffff) * sizeof(s_player_datum));
		team = player->team;
	}

	if ((*event_reference)->unknown00 == 1 && unit->type == 0 && unit->player_index != NONE)
	{
		s_ai_player *ai_player = ai_player_get(unit->player_index);
		if (ai_player)
			ai_player->unknown0c = g_510c54->game_time;
	}

	dword const *clusters = NULL;
	if (*filter_reference && location->cluster_index != NONE)
		clusters = function_1fb940(location->cluster_index);

	real radius = event->unknown00 != 0 ? 40.0f : 30.0f;

	if (*mode_reference == 0)
	{
		s_slot_target_list *joint = (s_slot_target_list *)element_502424_get(event->data.unknown0c);
		for (long i = 0; i < 10; i++)
		{
			s_slot_target_entry *entry = &joint->entries[i];
			long member_index = entry->actor_index;
			if (member_index != NONE && entry->type == 0 &&
				(!filter || function_1fba10(member_index, actor_index, location, event, clusters, &position)))
				function_1fbff0(member_index, actor_index, unit_index, NONE, event);
		}
	}
	else if (mode == 1)
	{
		if (actor && actor->unknown07c != NONE)
			function_1fbe00(actor->unknown07c, actor_index, unit_index, filter, clusters, event);
	}
	else if (mode >= 2)
	{
		s_clump_walk iterator;
		s_502420_element *clump;

		clump_walk_begin(&iterator);
		while ((clump = clump_walk_step(&iterator)) != NULL)
		{
			short clump_team = *(short *)((byte *)clump + 0x12);
			if (actor_index != NONE && mode < 3 && team != clump_team && function_1df560(team, clump_team))
				continue;

			point3f const *center = (point3f const *)((byte *)clump + 4);
			real dx = position.x - center->x;
			real dy = position.y - center->y;
			real dz = position.z - center->z;
			if (sqrt(dx * dx + dy * dy + dz * dz) < radius + 6.0)
				function_1fbe00(iterator.clump_index, actor_index, unit_index, filter, clusters, event);
		}
	}

	if (event->unknown00 == 3)
		function_295320(&position);
	else if (event->unknown00 == 1 && ((1 << unit->type) & 3) && unit->player_index != NONE)
		g_4f55d0->unknown36c = g_510c54->game_time;
}
#endif
