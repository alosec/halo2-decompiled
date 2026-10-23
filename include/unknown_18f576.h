/* UNKNOWN_18F576.H: the online status block of a local player slot (player_slots.h, src/player_slots.cpp) */

#ifndef UNKNOWN_18F576_H
#define UNKNOWN_18F576_H

#include "unknown_11c920.h"

/* a player's online status block at +0xb82 of a slot (0x92 bytes) */
struct s_player_slot_blockb82
{
	byte data[0x92];
};

bool function_18ffc3(long index, s_player_slot_blockb82 *block);

#endif
