// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object that holds a reference to
// another lets go of it when it is destroyed.
#include "reference_holder_2dbc40.h"

bool __cdecl function_279740(long value);

class deletable_2dbc40
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// @retail 0x2dbc40
class_2dbc40::~class_2dbc40()
{
	referenced_2d88a0 *object = field_30;
	object->reference_count--;
	if (object->reference_count == 0)
	{
		if (function_279740((long)object))
		{
			object->reference_count = 1;
		}
		else
		{
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_2dbc40 *)object)->destroy(1);
		}
	}
}
