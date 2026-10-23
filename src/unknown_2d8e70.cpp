// @flags /O2 /GL- /GX
// Havok (third-party physics library): a stream over another stream lets go
// of the stream beneath it when it is destroyed.
#include "stream_holder_2d8e70.h"

bool __cdecl function_279740(long value);

class deletable_2d8e70
{
public:
	virtual void destroy(unsigned int flags) = 0;
};


// @retail 0x2d8e70
stream_holder_2d8e70::~stream_holder_2d8e70()
{
	referenced_2d88a0 *object = stream;
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
				((deletable_2d8e70 *)object)->destroy(1);
			}
		}
	}
}
