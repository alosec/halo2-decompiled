// @flags /O2 /GL-
// Havok (third-party physics library): an array of 80-byte entries giving
// its storage back unless the top bit of its capacity is set.
#include "physics_allocator.h"

class class_309900
{
public:
	void *data;
	long size;
	unsigned long capacity;

	void release();
};

// @retail 0x309900
void class_309900::release()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, (capacity & 0x7fffffff) * 80, 0x12);
	}
}
