#ifndef PLACED_CHILD_2DEC70_H
#define PLACED_CHILD_2DEC70_H
// Havok (third-party physics library): an object that places a child object
// by a 3x4 transform of SSE columns, holding a reference to the child.
#include "shape_2de750.h"
#include "havok_reference.h"

struct child_info_2dec70 : shape_info_2de750
{
	c_havok_reference_counted *child;
	long unknown8;
	long unknownc;
	__m128 transform[4];
};

class placed_child_2dec70 : public shape_2de750
{
public:
	// the class's own operator new: a block with the class's tag; unlike
	// REFERENCED_NEW_2D88A0 it leaves the size unrecorded
	static void *operator new(unsigned int size)
	{
		return g_memory_2d88a0->allocate_block(size, 0x22);
	}
	static void operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object, ((referenced_2d88a0 *)object)->memory_size, 0x22);
	}

	c_havok_reference_counted *child;
	__m128 transform[4];

	placed_child_2dec70(child_info_2dec70 const &info);
};

#endif
