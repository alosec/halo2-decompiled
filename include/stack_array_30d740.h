#ifndef STACK_ARRAY_30D740_H
#define STACK_ARRAY_30D740_H
// Havok (third-party physics library): arrays whose storage comes from the
// allocator's stack area, for elements of 4, 8 and 16 bytes (their
// constructors and destructors are in unknown_30d740.cpp).
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>

// a block from the top of the allocator's stack area, or from its slot 9 when
// the area is too small
inline void *stack_allocate_30d740(long size)
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

#define ARRAY_30D740(name, T) \
	struct name \
	{ \
		T *data; \
		long size; \
		unsigned long capacity_and_flags; \
		name() : data(0), size(0), capacity_and_flags(0x80000000) {} \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(T), 0x12); \
			} \
		} \
	};

#define STACK_ARRAY_30D740(name, base, T) \
	struct name : base \
	{ \
		T *stack_block; \
		long stack_capacity; \
		name(long capacity); \
		~name(); \
	};

struct pair_30d870
{
	long first;
	long second;
};

ARRAY_30D740(array_longs_30d740, long)
STACK_ARRAY_30D740(longs_30d740, array_longs_30d740, long)


ARRAY_30D740(array_pairs_30d870, pair_30d870)
STACK_ARRAY_30D740(pairs_30d870, array_pairs_30d870, pair_30d870)

ARRAY_30D740(array_vectors_30d9a0, __m128)
STACK_ARRAY_30D740(vectors_30d9a0, array_vectors_30d9a0, __m128)

#endif
