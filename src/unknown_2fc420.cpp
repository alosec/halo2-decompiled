// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding an array of
// referenced children lets go of each child, then frees the array.
#include "unknown_1efac0.h"
#include "reference_counted_2d88a0.h"

class deletable_2fc420
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

struct child_2fc420
{
	referenced_2d88a0 *object;
	long value;
};

struct children_2fc420
{
	child_2fc420 *data;
	long size;
	long capacity_and_flags;

	~children_2fc420()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			child_2fc420 *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(child_2fc420), 0x12);
		}
	}
};

struct c_shape_library_base_b : c_a
{
	long field_8;
	children_2fc420 children;

	virtual ~c_shape_library_base_b();

	// as include/unknown_1eb350.h declares it
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_shape_library_base_b *)block)->flags, 0x22);
	}
};

// @retail 0x2fc420
c_shape_library_base_b::~c_shape_library_base_b()
{
	for (long i = 0; i < children.size; i++)
	{
		referenced_2d88a0 *child = children.data[i].object;
		child->reference_count--;
		if (child->reference_count == 0)
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_2fc420 *)child)->destroy(1);
		}
	}
}
