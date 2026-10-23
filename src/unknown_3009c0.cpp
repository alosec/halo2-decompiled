// @flags /O2 /GL-
// Havok (third-party physics library): dropping a reference, handing the
// object to the allocator with the last one.
#include "physics_allocator.h"

class class_3009c0_allocator
{
public:
	virtual void slot0() = 0;
	virtual void free_object(void *object) = 0;
};

class class_3009c0
{
public:
	char unknown0[0x24];
	long reference_count;

	void drop_one_reference();
};

// @retail 0x3009c0
void class_3009c0::drop_one_reference()
{
	if (--reference_count == 0)
	{
		class_3009c0_allocator *allocator = (class_3009c0_allocator *)g_480118;
		allocator->free_object(this);
	}
}
