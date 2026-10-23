#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1523c0.h"
#include "data_array.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_2BC1B0.CPP: the game engine whose vtable is at 0x45c7a8 (the second
   engine object at 0x47fc84): its slots 30..50, which unknown_1523c0.h numbers
   v0..v20 (its slots 0..28 are c_game_engine_derived's v22..v50 in
   unknown_072c70.cpp), and the helpers they use. The engine keeps up to three
   marker indices in its state (g_51ecc4, defined by unknown_072c70.cpp). */

struct s_game_engine_data;
extern s_game_engine_data *g_51ecc4;

/* the engine's state, as these functions read it */
struct s_state_2bc1
{
	byte unknown00[0xc];
	long markers[3];
	/* each team's countdown to its next point while a player holds its
	   item, and the points counted so far in this hold */
	char point_timers[3];
	char point_tallies[3];
	/* how long each team's item has been away from home */
	short ticks_away[3];
};

long player_find_unit_item_of_type(long player_index, long type);
bool function_15eaf0();
void function_15b930(long player_index, bool by_team, long counter, long delta);

class c_game_engine_45c7a8 : public c_game_engine
{
public:
	virtual void v0(long, long, bool, long);
	virtual void v2(long, long);
	virtual bool v5(long, long);
	virtual long v7(long, byte *);
};

__declspec(noinline) bool function_2bc1b0(point3f *position, long index);

// @retail 0x2bc1b0
bool function_2bc1b0(point3f *position, long index)
{
	bool marker_found_flag = false;
	long marker_index = ((s_state_2bc1 *)g_51ecc4)->markers[index];

	if (marker_index != NONE)
	{
		s_marker_entry *marker = &g_4e0350->marker_entries[marker_index];

		if (position)
		{
			*position = marker->position;
		}
		marker_found_flag = true;
	}
	return marker_found_flag;
}

// @retail 0x2bcc10
void c_game_engine_45c7a8::v0(long player_index, long other_player_index, bool flag, long)
{
	if (player_find_unit_item_of_type(other_player_index, 2) != NONE && !flag && player_index != NONE && player_index != other_player_index)
	{
		function_15b930(player_index, function_15eaf0(), 0x1a, 1);
	}
	if (player_index != NONE && !flag && player_index != other_player_index && player_find_unit_item_of_type(player_index, 2) != NONE)
	{
		function_15b930(player_index, function_15eaf0(), 0x19, 1);
	}
}

// @retail 0x2bcc90
bool c_game_engine_45c7a8::v5(long player_index, long type)
{
	bool result = false;

	if (player_find_unit_item_of_type(player_index, 2) != NONE)
	{
		if (type == 2)
			result = g_4e6948->s232 == 0;
		else if (type == 1)
			result = g_4e6948->flags22c_bits.bit1;
		else if (type == 3)
			result = g_4e6948->flags22c_bits.bit2;
	}
	else
		result = c_game_engine::v5(player_index, type);
	return result;
}

// @retail 0x2bcd20
long c_game_engine_45c7a8::v7(long player_index, byte *b)
{
	long result = NONE;

	*b = 1;
	if (player_find_unit_item_of_type(player_index, 2) != NONE)
		result = 0xe;
	return result;
}

struct s_team_entry;
struct s_spawn_influence_list;
s_team_entry *function_15e410(short team);
void function_15e360(point3f const *point, long object_index);

// @retail 0x2bc340
void function_2bc340(long team)
{
	long *entry = (long *)function_15e410((short)team);
	if (entry)
	{
		point3f position;
		function_2bc1b0(&position, team);
		function_15e360(&position, *entry);
	}
}

void function_23ba10(long type, s_spawn_influence_list *list, point3f const *point);

struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void function_15e050(long object_index, short team);
void __stdcall function_a7870(long object_index);
struct s_placement_2bc1
{
	byte field_0[0x1c];
	point3f position;
	byte field_28[0xc4 - 0x28];
};

// @retail 0x2bc1f0
void function_2bc1f0(long team)
{
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		long tag_index = *(long *)(*(byte **)(definition + 0xc) + 0xc);
		if (tag_index != NONE)
		{
			s_placement_2bc1 placement;
			function_b7930(&placement, tag_index, NONE, 0);
			if (function_2bc1b0(&placement.position, team))
			{
				long object_index = function_b7b40(&placement);
				if (object_index != NONE)
				{
					function_15e050(object_index, (short)team);
					if (g_4e6948->mode != 4)
					{
						s_event event;

						game_engine_event_initialize_inline(&event, 4, 1);
						function_a7c50(&event);
						function_19eb30(&event);
					}
					function_a7870(object_index);
				}
			}
		}
	}
}

struct s_object_header_2bc1
{
	dword unknown00[2];
	byte *object;
};

struct s_item_position_2bc1
{
	byte unknown00[0x30];
	point3f position;
};

// @retail 0x2bcd50
void c_game_engine_45c7a8::v2(long unused, long list_pointer)
{
	for (long i = 0; i < g_4e6948->s230; i++)
	{
		long *entry = (long *)function_15e410((short)i);

		if (entry && *entry != NONE)
		{
			long object_index = *entry;
			s_item_position_2bc1 *object = (s_item_position_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[object_index & 0xffff].object;

			function_23ba10(6, (s_spawn_influence_list *)list_pointer, &object->position);
			function_23ba10(5, (s_spawn_influence_list *)list_pointer, &object->position);
		}
	}
}

bool function_15b7c0(long delta, long player_index);
long game_engine_get_player_score(dword player_index);
void player_announce_points_to_win(long player_index, long score);
void function_10da60(long item_index, point3f *position);
bool team_ball_away_from_home(long team);
bool function_162b90(long item_index);
void function_1972a0(long player_index, long value14, long value10);
void function_2bce70(long player_index);

/* a team's item, as these functions read it (function_15e410's entries) */
struct s_team_item_2bc1
{
	long object_index;
	short value04;
	byte unknown06[0xc - 6];
	long player_index;
};

/* an item object, as 0x2bc990 reads it */
struct s_item_object_2bc1
{
	long definition_index;
	dword flags;
	byte unknown08[0x14 - 8];
	long parent_index;
	byte unknown18[0x12c - 0x18];
	byte flags12c;
};

/* updates a team's item each tick: how long it has been away from home, its
   return home once it has waited too long, and the points of the player
   holding it */
// @retail 0x2bc990
void __stdcall function_2bc990(long team)
{
	long const *team_reference = &team;
	s_team_item_2bc1 *entry = (s_team_item_2bc1 *)function_15e410((short)*team_reference);

	if (entry && entry->object_index != NONE)
	{
		s_item_object_2bc1 *object = (s_item_object_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[entry->object_index & 0xffff].object;
		dword maximum = g_510c54->field_2_3 * 40;
		point3f position;
		s_state_2bc1 *state;

		function_10da60(entry->object_index, &position);
		if (entry->player_index == NONE && team_ball_away_from_home(team))
		{
			dword ticks;

			state = (s_state_2bc1 *)g_51ecc4;
			ticks = state->ticks_away[team] + 1;
			if (ticks > maximum)
				ticks = maximum;
			state->ticks_away[team] = (short)ticks;
		}
		else
		{
			state = (s_state_2bc1 *)g_51ecc4;
			state->ticks_away[team] = 0;
		}

		/* the item's object, read again for its definition */
		s_item_object_2bc1 *current = (s_item_object_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[entry->object_index & 0xffff].object;
		byte *definition = g_4e3b44[current->definition_index & 0xffff].bytes;

		if (*(short *)(definition + 0x290) == 2 && !(object->flags12c & 1) &&
			((dword)state->ticks_away[team] >= maximum || function_162b90(entry->object_index)) &&
			object->parent_index == NONE && (bool)((object->flags >> 8) & 1))
		{
			s_event event;

			game_engine_event_initialize_inline(&event, 4, 4);
			function_19eb90(&event);
			function_2bc340(team);
			state = (s_state_2bc1 *)g_51ecc4;
		}
		if (entry->player_index != NONE)
		{
			char timer = state->point_timers[team];

			if (timer != NONE)
			{
				timer--;
				state->point_timers[team] = timer;
				if (timer <= 0)
				{
					function_1972a0(entry->player_index, 3, entry->value04);
					function_2bce70(entry->player_index);
					state = (s_state_2bc1 *)g_51ecc4;
					state->point_timers[team] = (char)g_510c54->field_2_3;
					char *count = &state->point_tallies[team];

					(*count)++;
					function_15b930(entry->player_index, function_15eaf0(), 0x17, 1);
					if (*count % 5 == 0)
					{
						s_event event;

						game_engine_event_initialize_inline(&event, 4, 5);
						game_engine_event_set_cause_player(&event, entry->player_index);
						function_19eb90(&event);
					}
				}
			}
		}
		else
		{
			state->point_tallies[team] = 0;
		}
	}
}

// @retail 0x2bce70
void function_2bce70(long player_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->w6c == 1 &&
		(g_4e6948->mode == 4 || g_4e9ae8->lc04 == 1))
	{
		long old_score = game_engine_get_player_score(player_index);

		if (function_15b7c0(1, player_index))
		{
			bool by_team = false;

			if (g_55e4d0[g_4e9ae8->engine_index])
			{
				by_team = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
			}
			function_15b930(player_index, by_team, 0x17, 1);
		}
		long score = game_engine_get_player_score(player_index);
		if (old_score != score)
		{
			player_announce_points_to_win(player_index, score);
		}
	}
}
