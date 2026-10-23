// @flags /O2 /GL- /GX
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "child_query_2f9950.h"

void __cdecl function_2ead90(void const *a, void const *b, void const *input, void *collector);

// the query with the two sides the other way round and the first side replaced by the child of the shape that wraps it, its pairs passed on swapped back
// @retail 0x2fac70
void __cdecl function_2fac70(void const *a, void const *b, void const *input, void *collector)
{
	swapped_collector_2eb1d0 swapped(collector);
	body_2ea940 const *side = (body_2ea940 const *)a;
	wrapper_2f9950 const *wrapper = (wrapper_2f9950 const *)side->shape;
	void const *placement = side->placement;
	body_2ea940 child(side, placement, wrapper);
	function_2ead90(b, &child, input, &swapped);
}
