// @flags /O2 /GL- /GX
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"

void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

void __cdecl function_2f0700(void const *a, void const *b, void const *input, void *collector);

// the query with the two sides the other way round, its hits passed on as seen from the other side
// @retail 0x2f0c40
void __cdecl function_2f0c40(void const *a, void const *b, void const *input, void *collector)
{
	flipped_collector_2ed110 flipped(collector);
	function_2f0700(b, a, input, &flipped);
}

// the query with the two sides the other way round: its direction turned
// around, and each collector wrapped in one that turns its results back
// @retail 0x2f0ca0
void __cdecl function_2f0ca0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2fe830(b, a, &swapped, &first, &second);
	}
	else
	{
		function_2fe830(b, a, &swapped, &first, 0);
	}
}

// with the two sides the other way round, the normals of the hits it adds
// flipped back
// @retail 0x2f0da0
void class_2f0100::function_2f0da0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector)
{
	int i = collector->hits->size;
	function_2f0100(b, a, input, collector);
	for (; i < collector->hits->size; i++)
	{
		__m128 *normal = &collector->hits->data[i].normal;
		*normal = _mm_xor_ps(*normal, *(const __m128 *)g_4124a0);
	}
}
