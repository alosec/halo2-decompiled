// @flags /O2 /GL- /GX
// Havok (third-party physics library): queries between the two sides of a
// pair passed on to the function that runs them, with the sides changed.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"

void __cdecl function_2eb670(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// the query with the two sides the other way round: its direction turned
// around, and each collector wrapped in one that turns its results back
// @retail 0x2eb7d0
void __cdecl function_2eb7d0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2eb670(b, a, &swapped, &first, &second);
	}
	else
	{
		function_2eb670(b, a, &swapped, &first, 0);
	}
}

// the query with the two sides the other way round, its pairs passed on swapped back
// @retail 0x2eb8d0
void class_2eab70::function_2eb8d0(void const *a, void const *b, void const *input, void *collector)
{
	swapped_collector_2eb1d0 swapped(collector);
	function_2eab70(b, a, input, &swapped);
}

// the query with the two sides the other way round, its hits passed on as seen from the other side
// @retail 0x2eb930
void class_2ea830::function_2eb930(void const *a, void const *b, void const *input, void *collector)
{
	flipped_collector_2ed110 flipped(collector);
	function_2ea830(b, a, input, &flipped);
}

// with the two sides the other way round, the normals of the hits it adds
// flipped back
// @retail 0x2eb990
void class_2ea410::function_2eb990(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector)
{
	int i = collector->hits->size;
	function_2ea410(b, a, input, collector);
	for (; i < collector->hits->size; i++)
	{
		__m128 *normal = &collector->hits->data[i].normal;
		*normal = _mm_xor_ps(*normal, *(const __m128 *)g_4124a0);
	}
}

// the query with the two sides the other way round: its direction turned
// around, and each collector wrapped in one that turns its results back
// @retail 0x2eb9f0
void class_2ea530::function_2eb9f0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2ea530(b, a, &swapped, &first, &second);
	}
	else
	{
		function_2ea530(b, a, &swapped, &first, 0);
	}
}
