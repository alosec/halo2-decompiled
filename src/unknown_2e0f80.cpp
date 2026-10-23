// @flags /O2 /GL-
// Havok (third-party physics library): inserting an interval's two endpoints
// into a sorted list of endpoints, returning where each one went.
#include "growable_array_2d91d0.h"

struct endpoint_2e0f80
{
	unsigned short value;
	unsigned short id;
};

struct endpoints_2e0f80
{
	ELEMENTS_2D91D0_MEMBERS(endpoint_2e0f80)

	void function_2e0f80(long unused, long id, unsigned short low, unsigned short high,
		unsigned short *first_slot, unsigned short *second_slot);
};

// the list starts with an endpoint lower than any other, so neither search
// runs off its start
// @retail 0x2e0f80
void endpoints_2e0f80::function_2e0f80(long unused, long id, unsigned short low, unsigned short high,
	unsigned short *first_slot, unsigned short *second_slot)
{
	long count = size + 2;
	ensure_capacity(count);
	endpoint_2e0f80 *entry = &data[count - 3];
	size = count;
	while (high <= entry->value)
	{
		entry[2] = entry[0];
		entry--;
	}
	entry[2].value = high;
	entry[2].id = (unsigned short)id;
	*second_slot = (unsigned short)(entry - data + 2);
	while (low < entry->value)
	{
		entry[1] = entry[0];
		entry--;
	}
	entry[1].id = (unsigned short)id;
	entry[1].value = low;
	*first_slot = (unsigned short)(entry - data + 1);
}
