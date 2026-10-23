// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query for a pair of shapes passed on
// to the collector's own handler for the pair the other way round.
#include "swapped_query_2ed2a0.h"

struct description_2ecc80;
class typed_2ecd80;

class class_2ecc80
{
public:
	void function_2ecd00(description_2ecc80 const *description, long arg_1, long arg_2, long arg_3, long arg_4);
};

class swapped_2ed4e0 : public class_2ecc80
{
public:
	void function_2ed4e0(typed_2ecd80 **other, description_2ecc80 const *description, query_2ed2a0 const *query, void *collector, void *second_collector);
};

// @retail 0x2ed4e0
void swapped_2ed4e0::function_2ed4e0(typed_2ecd80 **other, description_2ecc80 const *description, query_2ed2a0 const *query, void *collector, void *second_collector)
{
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2ecd00(description, (long)other, (long)&swapped, (long)&first, (long)&second);
	}
	else
	{
		function_2ecd00(description, (long)other, (long)&swapped, (long)&first, 0);
	}
}
