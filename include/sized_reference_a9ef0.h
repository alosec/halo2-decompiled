#ifndef SIZED_REFERENCE_A9EF0_H
#define SIZED_REFERENCE_A9EF0_H
#include "reference_counted_2d88a0.h"
// Sized reference base shared with the existing deleting destructor at a9ef0.
class c_sized_reference_a9ef0
{
public:
	unsigned short memory_size;
	unsigned short reference_count;
	c_sized_reference_a9ef0() { reference_count = 1; }
	virtual ~c_sized_reference_a9ef0() {}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((c_sized_reference_a9ef0 *)object)->memory_size, 0x10);
	}
};
#endif
