// @flags /O2 /GL-
// Havok (third-party physics library): arrays that own their storage unless
// the top bit of their capacity is set.
#include "physics_allocator.h"

class class_2e17d0
{
public:
	void *data;
	long size;
	unsigned long capacity;

	void release_2();
	void release_1();
	class_2e17d0();
};

// @retail 0x2e17d0
void class_2e17d0::release_2()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, (capacity & 0x7fffffff) * 2, 0x12);
	}
}

// @retail 0x2e1800
void class_2e17d0::release_1()
{
	if ((capacity & 0x80000000) == 0)
	{
		havok_allocator()->release(data, capacity & 0x7fffffff, 0x12);
	}
}

// @retail 0x2e1830
class_2e17d0::class_2e17d0()
{
	data = 0;
	size = 0;
	capacity = 0x80000000;
}
