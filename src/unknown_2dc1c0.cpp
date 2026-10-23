// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroys an object holding an array of
// 8-byte elements, releasing the storage unless the array does not own it
// (top bit of the capacity).
#include "reference_counted_2d88a0.h"

struct array_2dc1c0
{
	void *data;
	long count;
	unsigned long capacity_and_flags;

	~array_2dc1c0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * 8, 0x12);
		}
	}
};

class array_owner_2dc1c0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	array_2dc1c0 items;
};

// @retail 0x2dc190 deleting array_owner_2dc1c0
// @retail 0x2dc1c0 destructor array_owner_2dc1c0
