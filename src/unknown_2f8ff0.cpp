// @flags /O2 /GL- /GX
// Havok (third-party physics library): the object kept for a pair of shapes
// whose first holds a number of parts: one cleared flag per part.
#include "parts_agent_2f8ff0.h"

// @retail 0x2f8ff0
parts_agent_2f8ff0::parts_agent_2f8ff0(parts_description_2f8ff0 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3) :
	sized_2ed170(arg_3)
{
	long count = description->shape->part_count;
	parts.resize(count);
	for (long i = 0; i < count; i++)
	{
		parts.data[i].flag = 0;
	}
}
