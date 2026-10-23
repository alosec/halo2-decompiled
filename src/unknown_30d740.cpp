// @flags /O2 /GL- /GX
// Havok (third-party physics library): arrays whose storage comes from the
// allocator's stack area, for elements of 4, 8 and 16 bytes: their
// constructors and destructors (the same shape as the arrays in
// unknown_2e18e0.cpp).
#include "stack_array_30d740.h"

// @retail 0x30d740
longs_30d740::longs_30d740(long capacity)
{
	void *block = stack_allocate_30d740((capacity * sizeof(long) + 15) & ~15);
	data = (long *)block;
	capacity_and_flags = capacity | 0x80000000;
	stack_block = (long *)block;
	stack_capacity = capacity;
}

// @retail 0x30d7d0
longs_30d740::~longs_30d740()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(long) + 15) & ~15);
}


// @retail 0x30d870
pairs_30d870::pairs_30d870(long capacity)
{
	void *block = stack_allocate_30d740((capacity * sizeof(pair_30d870) + 15) & ~15);
	data = (pair_30d870 *)block;
	capacity_and_flags = capacity | 0x80000000;
	stack_block = (pair_30d870 *)block;
	stack_capacity = capacity;
}

// @retail 0x30d900
pairs_30d870::~pairs_30d870()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(pair_30d870) + 15) & ~15);
}


// @retail 0x30d9a0
vectors_30d9a0::vectors_30d9a0(long capacity)
{
	void *block = stack_allocate_30d740((capacity * sizeof(__m128) + 15) & ~15);
	data = (__m128 *)block;
	capacity_and_flags = capacity | 0x80000000;
	stack_block = (__m128 *)block;
	stack_capacity = capacity;
}

// @retail 0x30da30
vectors_30d9a0::~vectors_30d9a0()
{
	g_memory_2d88a0->release_stack(stack_block, (stack_capacity * sizeof(__m128) + 15) & ~15);
}
