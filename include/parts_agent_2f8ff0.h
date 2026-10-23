#ifndef PARTS_AGENT_2F8FF0_H
#define PARTS_AGENT_2F8FF0_H
// Havok (third-party physics library): the object kept for a pair of shapes
// whose first holds a number of parts (constructor 0x2f8ff0).
#include "pair_agent_2ed170.h"
#include "growable_array_2d91d0.h"

struct parts_shape_2f8ff0
{
	char field_0[0x14];
	long part_count;
};

struct parts_description_2f8ff0
{
	parts_shape_2f8ff0 const *shape;
};

struct part_2f8ff0
{
	char flag;
	char field_1[3];
};

// an array that starts out in the room for four parts held inside it
struct parts_2f8ff0
{
	ELEMENTS_2D91D0_MEMBERS(part_2f8ff0)
	part_2f8ff0 embedded[4];

	parts_2f8ff0()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	~parts_2f8ff0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(part_2f8ff0), 0x12);
		}
	}
};

class parts_agent_2f8ff0 : public sized_2ed170
{
public:
	parts_2f8ff0 parts;

	parts_agent_2f8ff0(parts_description_2f8ff0 const *description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3);
};

#endif
