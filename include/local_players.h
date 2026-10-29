/* LOCAL_PLAYERS.H: whether a local player slot holds a player (the slots are
   the entries of g_4e8c20, globals.h, NONE when empty), as
   unknown_155760.cpp and unknown_223240.cpp test it */
#ifndef LOCAL_PLAYERS_H
#define LOCAL_PLAYERS_H

#include "unknown_11c920.h"
#include "globals.h"

static inline bool local_player_exists(long index)
{
	bool result = false;

	if (index != NONE)
	{
		result = g_4e8c20->entries[index] != NONE;
	}
	return result;
}

#endif
