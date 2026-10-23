// @flags /O2 /GL-
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "child_query_2f9950.h"

void __cdecl function_2ea640(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// the query with the second side replaced by the child of the shape that wraps it
// @retail 0x2f9740
void class_2ea530::function_2f9740(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	wrapper_2f9950 const *wrapper = (wrapper_2f9950 const *)side->shape;
	void const *placement = side->placement;
	body_2ea940 child;
	child.placement = placement;
	child.parent = side;
	child.shape = wrapper->child;
	child.key = side->key;
	function_2ea530(a, &child, input, collector, second_collector);
}

// the query with the second side replaced by the child of the shape that wraps it
// @retail 0x2f9790
void __cdecl function_2f9790(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	function_2ea640(a, &child, input, collector, second_collector);
}
