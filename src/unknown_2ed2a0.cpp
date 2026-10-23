// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query for a pair of shapes passed on
// to the handler for the pair the other way round.
#include "swapped_query_2ed2a0.h"

struct description_2ecc80;
class typed_2ecd80;
struct dispatcher_2ecd80;

void __cdecl function_2ecd80(description_2ecc80 const *description, typed_2ecd80 **other, dispatcher_2ecd80 **dispatcher, long arg_3, long arg_4);

// @retail 0x2ed2a0
void __cdecl function_2ed2a0(typed_2ecd80 **other, description_2ecc80 const *description, query_2ed2a0 const *query, void *collector, void *second_collector)
{
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2ecd80(description, other, (dispatcher_2ecd80 **)&swapped, (long)&first, (long)&second);
	}
	else
	{
		function_2ecd80(description, other, (dispatcher_2ecd80 **)&swapped, (long)&first, 0);
	}
}
