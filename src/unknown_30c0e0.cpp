// @flags /O2 /GL-
// Havok (third-party physics library): a 4-byte object's deleting
// destructor, and blocks given back to the allocator.
#include "physics_allocator.h"

class class_30c0e0
{
public:
	virtual ~class_30c0e0();

	static void operator delete(void *block, unsigned int size)
	{
		havok_allocator()->release(block, size, 0x1c);
	}
};

// @retail 0x30c0e0 deleting
class_30c0e0::~class_30c0e0()
{
}

struct s_30c110
{
	void *vtable;
	unsigned short allocation_size;
	short reference_count;
};

// @retail 0x30c110
void __cdecl function_30c110(s_30c110 *object)
{
	havok_allocator()->release(object, object->allocation_size, 0x2d);
}

// @retail 0x30c130
void __cdecl function_30c130(void *block)
{
	havok_allocator()->release(block, 0x104, 0x22);
}
