// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding a reference to
// another and an array of 8-byte elements lets go of both when destroyed.
#include "reference_holder_30b3a0.h"
#include "listened_object_315950.h"

bool __cdecl function_279740(long value);

class deletable_30b3a0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// @retail 0x30b3a0
class_30b3a0::~class_30b3a0()
{
	referenced_2d88a0 *object = collidable.body.shape;
	if (object)
	{
		object->reference_count--;
		if (object->reference_count == 0)
		{
			if (function_279740((long)object))
			{
				object->reference_count = 1;
			}
			else
			{
				// the deleting destructor, first in the vtable, without the
				// null test `delete` adds
				((deletable_30b3a0 *)object)->destroy(1);
			}
		}
	}
}

// the deleting destructor of the object that tells its listeners it is going
// @retail 0x30b450 deleting class_315950
