// @flags /O2 /GL- /GX
// Havok (third-party physics library): a second copy of 0x2f9f20's query with the first side
// replaced by the child of the shape that wraps it, and the two sides the
// other way round.
#include "pair_agents_2eab70.h"
#include "child_query_2f9950.h"
#include "swapped_query_2ed2a0.h"

// the query with the first side replaced by the child of the shape that wraps
// it, and the two sides the other way round: its direction turned around, and
// each collector wrapped in one that turns its results back
// @retail 0x2fb790
void class_2ea530::function_2fb790(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	body_2ea940 const *side = (body_2ea940 const *)a;
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		function_2ea530(b, &child, &swapped, &first, &second);
	}
	else
	{
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		function_2ea530(b, &child, &swapped, &first, 0);
	}
}
