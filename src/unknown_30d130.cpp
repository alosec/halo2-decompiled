// @flags /O2 /GL-
// Havok (third-party physics library): an array of 8-byte entries giving
// its storage back unless the top bit of its capacity is set.
#include "physics_allocator.h"

class class_30d130
{
public:
	void *data;
	long size;
	unsigned long capacity;

	void release();
};

// @retail 0x30d130
void class_30d130::release()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, (capacity & 0x7fffffff) * 8, 0x12);
	}
}
