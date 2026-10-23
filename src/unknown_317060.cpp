// @flags /O2 /GL- /GX
// Havok (third-party physics library): an array of 48-byte elements whose
// storage comes from the allocator's stack area: its constructor and
// destructor (the same shape as the arrays in unknown_2e18e0.cpp).
#include "reference_counted_2d88a0.h"

// a block from the top of the allocator's stack area, or from its slot 9 when
// the area is too small
inline void *stack_allocate_317060(long size)
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

struct element_317060
{
	char bytes[0x30];
};

struct array_317060
{
	element_317060 *data;
	long size;
	unsigned long capacity_and_flags;

	array_317060() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_317060()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(element_317060), 0x12);
		}
	}
};

struct stack_array_317060 : array_317060
{
	element_317060 *stack_block;
	long stack_capacity;

	stack_array_317060(long capacity);
	~stack_array_317060();
};

// @retail 0x317060
stack_array_317060::stack_array_317060(long capacity)
{
	void *block = stack_allocate_317060((capacity * sizeof(element_317060) + 15) & ~15);
	capacity_and_flags = capacity | 0x80000000;
	stack_capacity = capacity;
	data = (element_317060 *)block;
	stack_block = (element_317060 *)block;
}

// the block goes back to the allocator's stack area, then the array's own
// storage is given back
// @retail 0x317100
stack_array_317060::~stack_array_317060()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(element_317060) + 15) & ~15);
}
