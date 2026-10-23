// @flags /O2 /Ob1 /Gr
/* SQUADS.CPP: the iterators over the actors of a squad and over the squads
   of a squad group */

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"

// @retail 0x204d30
void function_204d30(s_squad_actor_iterator *iterator, long squad_index)
{
	squad_actors_begin(iterator, squad_index);
}

// @retail 0x204d70
s_actor_datum *function_204d70(s_squad_actor_iterator *iterator)
{
	return squad_actors_next(iterator);
}

// @retail 0x204db0
void function_204db0(s_squad_group_iterator *iterator, long squad_group_index)
{
	s_squad_group_datum *group = squad_group_get(squad_group_index);
	iterator->group = group;
	iterator->root = group;
	iterator->previous_squad_index = NONE;
	iterator->squad_index = NONE;
	for (long child_index = group->first_child_index; child_index != NONE; child_index = iterator->group->first_child_index)
		iterator->group = squad_group_get(child_index);
	iterator->next_squad_index = iterator->group->first_squad_index;
}

// @retail 0x204e10
s_squad_datum *function_204e10(s_squad_group_iterator *iterator)
{
	while (iterator->group)
	{
		if (iterator->next_squad_index == NONE)
		{
			if (iterator->group == iterator->root)
			{
				iterator->group = NULL;
				return NULL;
			}
			long index = iterator->group->next_sibling_index;
			if (index == NONE)
			{
				iterator->group = squad_group_get(iterator->group->parent_index);
				iterator->next_squad_index = iterator->group->first_squad_index;
			}
			else
			{
				do
				{
					iterator->group = squad_group_get(index);
					index = iterator->group->first_child_index;
				} while (index != NONE);
				iterator->next_squad_index = iterator->group->first_squad_index;
			}
		}
		else
		{
			long squad_index = iterator->next_squad_index;
			s_squad_datum *squad = squad_get(squad_index);
			iterator->previous_squad_index = iterator->squad_index;
			iterator->squad_index = squad_index;
			iterator->next_squad_index = squad->next_squad_index;
			return squad;
		}
	}
	return NULL;
}