#include "unknown_11c920.h"
#include "globals.h"
#include "path.h"
#include <string.h>

// @flags /O2 /Gr

/* the structure bsp's pathfinding data (g_4e0348) */
struct s_structure_bsp_path_view
{
	byte unknown00[0xc4];
	long pathfinding_count;
	void *pathfinding;
};

// @retail 0x271300
void function_271300(s_type_f17a25 *state, s_path_location const *location, s_path_settings const *settings,
	s_path_source const *source, long flags)
{
	s_structure_bsp_path_view *bsp;
	void *pathfinding;

	state->unknown54 = 0;
	state->unknown90 = NONE;
	state->unknownae = 0;
	state->heap_count = 0;
	if (location)
	{
		state->location = *location;
	}
	else
	{
		state->location.unknown00 = 0;
		state->location.unknown02 = 0;
	}
	bsp = (s_structure_bsp_path_view *)g_4e0348;
	pathfinding = NULL;
	if (bsp->pathfinding_count > 0)
	{
		pathfinding = bsp->pathfinding;
	}
	state->pathfinding = pathfinding;
	state->source = *source;
	state->flags = flags;
	state->unknownac = 0;
	if (settings)
	{
		state->settings = *settings;
	}
	else
	{
		memset(&state->settings, 0, sizeof(state->settings));
	}
	state->unknown14188 = 0;
}

// @retail 0x271e50
PRIVATE void function_271e50(s_type_f17a25 *state, short index)
{
	short node = state->heap[index].node;
	short cost = state->heap[index].cost;

	while (index > 1)
	{
		short parent = index >> 1;
		short parent_node = state->heap[parent].node;
		short parent_cost = state->heap[parent].cost;
		if (cost >= parent_cost)
		{
			break;
		}
		state->heap[index].node = parent_node;
		state->heap[index].cost = parent_cost;
		state->nodes[parent_node].heap_index = index;
		index = parent;
	}

	state->heap[index].node = node;
	state->heap[index].cost = cost;
	state->nodes[node].heap_index = index;
}

// @retail 0x271ef0
PRIVATE void function_271ef0(s_type_f17a25 *state, short index)
{
	short cost;
	short node;
	node = state->heap[index].node;
	cost = state->heap[index].cost;

	for (;;)
	{
		short best = index;
		short best_node = node;
		short best_cost = cost;
		short i = 0;
		short child = index * 2;
		for (; i < 2; i++, child++)
		{
			if (child >= state->heap_count)
			{
				break;
			}
			short child_cost = state->heap[child].cost;
			if (child_cost < best_cost)
			{
				best_node = state->heap[child].node;
				best = child;
				best_cost = child_cost;
			}
		}
		if (best == index)
		{
			break;
		}
		state->heap[index].cost = best_cost;
		state->heap[index].node = best_node;
		state->nodes[best_node].heap_index = index;
		index = best;
	}

	state->heap[index].node = node;
	state->heap[index].cost = cost;
	state->nodes[node].heap_index = index;
}
