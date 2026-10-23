// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"

/* slot type 3 */

// @retail 0x1b2d90
short __stdcall function_1b2d90(long actor_index, s_slot *slot, bool active)
{
	return g_46fbe4;
}

s_slot_handler_2 g_47e120 =
{
	{
		3, 2, NONE, -2, 0,
		actor_behavior_priority_minimum, function_1b2d90, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, 0
};
