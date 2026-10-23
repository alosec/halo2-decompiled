#ifndef SLOT_TABLE_372A40_H
#define SLOT_TABLE_372A40_H
// Havok (third-party physics library): a table of 64 slots that all start out
// pointing at the empty object it keeps after them (0x372a40 builds it); its
// blocks come from the allocator with tag 0x22.
#include "reference_counted_2d88a0.h"

// the empty object kept after the slots
struct empty_slot_372a40
{
	virtual void slot_0() {}
};

class slot_table_372a40
{
public:
	void *slots[64];
	empty_slot_372a40 empty;

	static void *operator new(unsigned int size)
	{
		return g_memory_2d88a0->allocate_block(size, 0x22);
	}
	static void operator delete(void *block, unsigned int size)
	{
		g_memory_2d88a0->release_block(block, size, 0x22);
	}

	slot_table_372a40();
	void store_slot_372a40(int index, void *value)
	{
		slots[index] = value;
	}
};

#endif
