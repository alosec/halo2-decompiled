// @flags /O2 /GL-
// Havok (third-party physics library): giving an open-addressing table's
// keys and values back to the allocator.
#include "physics_allocator.h"
#include "reference_counted_2d88a0.h"
#include "hash_table_2d9240.h"

void __cdecl function_2d8de0(void *buffer, int value, unsigned long size);

// @retail 0x2d94b0
class_2d9240::~class_2d9240()
{
	havok_allocator()->release(slots, (mask + 1) * 8, 0x12);
}

// moves the table into new storage of capacity slots (a power of two),
// adding each key again
// @retail 0x2d94d0
void class_2d9240::function_2d94d0(int capacity)
{
	unsigned long *old_slots = slots;
	int old_capacity = mask + 1;
	slots = (unsigned long *)g_memory_2d88a0->allocate_block(capacity * 8, 0x12);
	function_2d8de0(slots, 0, capacity * 4);
	field_4 = 0;
	mask = capacity - 1;
	for (int i = 0; i < old_capacity; i++)
	{
		if (old_slots[i] != 0)
		{
			function_2d92f0(old_slots[i], old_slots[old_capacity + i]);
		}
	}
	g_memory_2d88a0->release_block(old_slots, old_capacity * 8, 0x12);
}
