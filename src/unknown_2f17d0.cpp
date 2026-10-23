// @flags /O2 /GL- /GX
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"

void __cdecl function_2f1690(void const *a, void const *b, void const *input, void *collector);

// the query with the two sides the other way round, its pairs passed on swapped back
// @retail 0x2f17d0
void class_2f1560::function_2f17d0(void const *a, void const *b, void const *input, void *collector)
{
	swapped_collector_2eb1d0 swapped(collector);
	function_2f1560(b, a, input, &swapped);
}

// the query with the two sides the other way round, its pairs passed on swapped back
// @retail 0x2f1830
void __cdecl function_2f1830(void const *a, void const *b, void const *input, void *collector)
{
	swapped_collector_2eb1d0 swapped(collector);
	function_2f1690(b, a, input, &swapped);
}

// the query with the two sides the other way round, its hits passed on as seen from the other side
// @retail 0x2f1890
void class_2f1270::function_2f1890(void const *a, void const *b, void const *input, void *collector)
{
	flipped_collector_2ed110 flipped(collector);
	function_2f1270(b, a, input, &flipped);
}
