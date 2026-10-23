/* UNKNOWN_272B70.H: the ai references of the script functions (src/unknown_272b70.cpp) */

#ifndef UNKNOWN_272B70_H
#define UNKNOWN_272B70_H

#include "unknown_11c920.h"
#include "squads.h"

/* walks the actors an ai index names */
struct s_ai_actor_iterator
{
	long squad_index;
	long squad_group_index;
	long actor_index;
	bool single_actor;
	s_squad_actor_iterator field_10;
	s_squad_group_iterator group_iterator;
};

/* the type of an ai index, in its top two bits */
enum
{
	_ai_index_type_squad = 0,
	_ai_index_type_squad_group,
	_ai_index_type_actor,
	_ai_index_type_starting_location
};

inline long ai_index_get_type(long ai_index)
{
	return (dword)ai_index >> 30;
}

/* walks the squads an ai index names */
struct s_ai_squad_iterator
{
	s_squad_group_iterator group_iterator;
	long squad_group_index;
	long squad_index;
	long next_squad_index;
};

void ai_squad_iterator_new(long ai_index, s_ai_squad_iterator *iterator);

/* the inline copy of ai_squad_iterator_new (0x272d50) that retail places in
   some callers */
inline void ai_squad_iterator_new_inline(s_ai_squad_iterator *iterator, long ai_index)
{
	if (ai_index_get_type(ai_index) == _ai_index_type_squad_group)
	{
		long squad_group_index = ai_index & 0xffff;
		iterator->squad_group_index = squad_group_index;
		function_204db0(&iterator->group_iterator, squad_group_index);
	}
	else if (ai_index_get_type(ai_index) == _ai_index_type_squad)
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = ai_index & 0xffff;
	}
	else
	{
		iterator->squad_group_index = NONE;
		iterator->next_squad_index = NONE;
	}
}

/* the iterator is read through a reference to its pointer: retail's callers
   treat the iterator as reachable from memory (they reload its fields after
   every store and call) */
inline s_squad_datum *ai_squad_iterator_next(s_ai_squad_iterator *squad_cursor)
{
	s_ai_squad_iterator *const *iterator_reference = &squad_cursor;
	s_ai_squad_iterator *iterator = *iterator_reference;
	s_squad_datum *squad = NULL;
	if (iterator->squad_group_index == NONE)
	{
		if (iterator->next_squad_index != NONE)
		{
			iterator->squad_index = iterator->next_squad_index;
			iterator->next_squad_index = NONE;
			squad = squad_get(iterator->squad_index);
		}
	}
	else
	{
		squad = function_204e10(&iterator->group_iterator);
		iterator->squad_index = iterator->group_iterator.squad_index;
	}
	return squad;
}

void ai_actor_iterator_new(long ai_index, s_ai_actor_iterator *iterator);
s_actor_datum *ai_actor_iterator_next(s_ai_actor_iterator *iterator);

long function_272b70(long ai_index);
long function_272c90(long ai_index);
long function_272ff0(long object_index);
void function_273150(long list_index);
void function_273200(long ai_index, bool flag);
bool function_2732e0(long ai_index);
void function_275160(long list_index, bool flag);
void function_275270(long list_index, bool flag);
void function_2738a0(long ai_index, bool flag);
void function_273900(long ai_index, bool flag);
void function_2739d0(long ai_index, bool flag);
void function_273d30(long ai_index, long object_index);
void function_273ef0(long ai_index, bool flag);
long function_273f30(long ai_index, short mode, long *actor_count, real *average_vitality);
void function_2742f0(short team_a, short team_b);
void function_274a50(long ai_index, long vehicle_index, long filter_range, bool load);
void function_274da0(long ai_index, long filter_range);
void function_275a50(long ai_index, bool flag);
void function_275b20(long ai_index, bool value);
void function_275ad0(long unit_index, bool flag);
short function_275d70(long ai_index);
long function_275e20(long ai_index);
bool function_275eb0(long vehicle_index, long seat_label, bool flag);
bool function_275fc0(long vehicle_index, bool flag);
short function_276050(long ai_index);
real function_2760a0(long actor_index, long script_index, long name, long sound_index, real scale, real pitch);
bool function_276380(long ai_index);
void function_276440(long ai_index, short script_index);
void function_276480(long ai_index, short script_index);
void function_2764c0(long ai_index, short script_index);
bool function_276500(short script_index, long ai_index0, long ai_index1);
bool function_276560(short script_index, long ai_index0, long ai_index1, long ai_index2);
void function_2768d0(long role_name);
bool function_2766f0(long ai_index, long name_index);
bool function_276770(long ai_index, long name_index);
short function_2767f0(long ai_index);
void function_276b40(long object_index, real a, real b, real c);
void function_276cc0(real a, real b, real c);

#endif
