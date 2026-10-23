// @flags /O2 /GL- /GX
// Havok (third-party physics library): an array of four-byte values whose
// storage comes from the allocator's stack area: its constructor and
// destructor (the same shape as the arrays in unknown_2e18e0.cpp).
#include "reference_counted_2d88a0.h"

// a block from the top of the allocator's stack area, or from its slot 9 when
// the area is too small
inline void *stack_allocate_311d10(long size)
{
	memory_2d88a0 *memory = g_memory_2d88a0;
	long free = memory->stack_free;
	if (size > free)
	{
		return memory->allocate_stack_overflow(size);
	}
	char *block = memory->stack_cursor;
	memory->stack_cursor = block + size;
	memory->stack_free = free - size;
	return block;
}

struct array_311d10
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_311d10() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_311d10()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct stack_array_311d10 : array_311d10
{
	long *stack_block;
	long stack_capacity;

	stack_array_311d10(long capacity);
	~stack_array_311d10();
};

// @retail 0x311d10
stack_array_311d10::stack_array_311d10(long capacity)
{
	void *block = stack_allocate_311d10((capacity * sizeof(long) + 15) & ~15);
	data = (long *)block;
	capacity_and_flags = capacity | 0x80000000;
	stack_block = (long *)block;
	stack_capacity = capacity;
}

// @retail 0x311da0
stack_array_311d10::~stack_array_311d10()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(long) + 15) & ~15);
}
