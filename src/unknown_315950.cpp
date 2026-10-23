// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying an object that tells its
// listeners it is going.
#include "listened_object_315950.h"

// @retail 0x315950
class_315950::~class_315950()
{
	for (int i = listeners.size; --i >= 0; )
	{
		listener_315950 *listener = listeners.data[i];
		if (listener)
		{
			listener->object_destroyed(this);
		}
	}
}
