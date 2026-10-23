// @flags /O2 /GL-
// Havok (third-party physics library): an array of 16-byte entries giving
// its storage back unless the top bit of its capacity is set.
#include "physics_allocator.h"

class class_2fc610
{
public:
	long field_0;
	void *data;
	long size;
	unsigned long capacity;

	void release();
};

// @retail 0x2fc610
void class_2fc610::release()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, (capacity & 0x7fffffff) * 16, 0x12);
	}
}
