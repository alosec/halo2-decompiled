// @flags /O2 /GL-
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "child_query_2f9950.h"

void __cdecl function_2ead90(void const *a, void const *b, void const *input, void *collector);

// the query with the second side replaced by the child of the shape that wraps it
// @retail 0x2fab30
void class_2eab70::function_2fab30(void const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	wrapper_2f9950 const *wrapper = (wrapper_2f9950 const *)side->shape;
	void const *placement = side->placement;
	body_2ea940 child;
	child.placement = placement;
	child.parent = side;
	child.shape = wrapper->child;
	child.key = side->key;
	function_2eab70(a, &child, input, collector);
}

// the query with the second side replaced by the child of the shape that wraps it
// @retail 0x2fab70
void __cdecl function_2fab70(void const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	function_2ead90(a, &child, input, collector);
}

// the query with the second side replaced by the child of the shape that wraps it
// @retail 0x2fabb0
void class_2ea830::function_2fabb0(void const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	wrapper_2f9950 const *wrapper = (wrapper_2f9950 const *)side->shape;
	void const *placement = side->placement;
	body_2ea940 child;
	child.placement = placement;
	child.parent = side;
	child.shape = wrapper->child;
	child.key = side->key;
	function_2ea830(a, &child, input, collector);
}
