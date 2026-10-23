// @flags /O2 /GL-
// Havok (third-party physics library): an array of 32-byte entries giving
// its storage back unless the top bit of its capacity is set.
#include "physics_allocator.h"

class class_309930
{
public:
	void *data;
	long size;
	unsigned long capacity;

	void release();
};

// @retail 0x309930
void class_309930::release()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, (capacity & 0x7fffffff) * 32, 0x12);
	}
}
