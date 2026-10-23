// @flags /O2 /GL- /GX
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"

void __cdecl function_2ecfa0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ece90(void const *a, void const *b, void const *input, void *collector);

// the query with the two sides the other way round, its pairs passed on swapped back
// @retail 0x2ed0b0
void __cdecl function_2ed0b0(void const *a, void const *b, void const *input, void *collector)
{
	swapped_collector_2eb1d0 swapped(collector);
	function_2ecfa0(b, a, input, &swapped);
}

// the query with the two sides the other way round, its hits passed on as seen from the other side
// @retail 0x2ed110
void __cdecl function_2ed110(void const *a, void const *b, void const *input, void *collector)
{
	flipped_collector_2ed110 flipped(collector);
	function_2ece90(b, a, input, &flipped);
}
