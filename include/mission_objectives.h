#ifndef MISSION_OBJECTIVES_H
#define MISSION_OBJECTIVES_H

/* MISSION_OBJECTIVES.H: the campaign's five objectives, as the pause screen lists
   them (an empty box when shown, a checked box when completed). Their
   states live in the game state at g_502248; the scripts show and complete
   them up to an index. */

enum e_mission_objective_state
{
	_mission_objective_state_hidden,
	_mission_objective_state_shown,
	_mission_objective_state_completed,

	k_mission_objective_state_count
};

enum
{
	k_mission_objective_count = 5
};

void mission_objectives_init();
void mission_objectives_reveal_through(long last_index);
long mission_objectives_state(long index);
long mission_objectives_text_id(long index);

#endif
