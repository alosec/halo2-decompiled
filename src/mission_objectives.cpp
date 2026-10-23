// @flags /O1 /Gr
/* MISSION_OBJECTIVES.CPP: the campaign objectives' states, kept in the game state,
   and the strings the pause screen shows for them */

#include "unknown_11c920.h"
#include "mission_objectives.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

void *function_123d40(char const *name, char const *type, long size);

/* each objective's e_mission_objective_state (unknown_29f5b0.cpp) */
extern long *g_502248;

/* allocates the objectives' states in the game state */
// @retail 0x22c033
void mission_objectives_init()
{
	g_502248 = (long *)function_123d40("unknown 22c033", NULL, k_mission_objective_count * sizeof(long));
}

/* shows every hidden objective up to and including this one (script
   function 751) */
// @retail 0x22c041
void mission_objectives_reveal_through(long last_index)
{
	if (PIN(last_index, 0, k_mission_objective_count - 1) == last_index)
	{
		for (long index = 0; index <= last_index; index++)
		{
			if (!g_502248[index])
			{
				g_502248[index] = _mission_objective_state_shown;
			}
		}
	}
}

/* an objective's e_mission_objective_state; hidden for an index out of range */
// @retail 0x22c075
long mission_objectives_state(long index)
{
	long state = _mission_objective_state_hidden;

	if (PIN(index, 0, k_mission_objective_count - 1) == index)
	{
		state = g_502248[index];
	}
	return state;
}

/* the string id of an objective's text */
// @retail 0x232371
long mission_objectives_text_id(long index)
{
	long objective_text_id;

	switch (index)
	{
	case 0:
		objective_text_id = 0x100030b;
		break;
	case 1:
		objective_text_id = 0x100030c;
		break;
	case 2:
		objective_text_id = 0x100030d;
		break;
	case 3:
		objective_text_id = 0x100030e;
		break;
	case 4:
		objective_text_id = 0x100030f;
		break;
	}
	return objective_text_id;
}
