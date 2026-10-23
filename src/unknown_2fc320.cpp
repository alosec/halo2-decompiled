// @flags /O2 /GL- /GX
// Havok (third-party physics library): the two constructors of a list of
// shapes: from an array of shapes, and from a description holding the
// shapes and, when there are as many, a second array paired with them.
#include "growable_array_2d91d0.h"

class c_havok_reference_counted;

struct pair_2fc320
{
	referenced_2d88a0 *field_0;
	referenced_2d88a0 *field_4;
};

// an array that starts out in the room for four pairs held inside it
struct pairs_2fc320
{
	ELEMENTS_2D91D0_MEMBERS(pair_2fc320)
	pair_2fc320 embedded[4];

	pairs_2fc320()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	~pairs_2fc320()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(pair_2fc320), 0x12);
		}
	}
};

class valued_2fc320 : public referenced_2d88a0
{
public:
	long field_8;

	valued_2fc320() : field_8(0) {}
	valued_2fc320(long const *value) : field_8(*value) {}
};

struct references_2fc320
{
	referenced_2d88a0 **data;
	long size;
	long capacity_and_flags;
};

struct lists_2fc200
{
	long field_0;
	references_2fc320 field_4;
	references_2fc320 field_10;
};

class class_2fc200 : public valued_2fc320
{
public:
	pairs_2fc320 pairs;

	class_2fc200(lists_2fc200 const *lists);
	void function_2fc2a0(referenced_2d88a0 **first, long count, referenced_2d88a0 **second);
};

class c_physics_shape_list : public valued_2fc320
{
public:
	pairs_2fc320 pairs;

	c_physics_shape_list(c_havok_reference_counted **shapes, long count);
};

// @retail 0x2fc320
c_physics_shape_list::c_physics_shape_list(c_havok_reference_counted **shapes, long count)
{
	((class_2fc200 *)this)->function_2fc2a0((referenced_2d88a0 **)shapes, count, 0);
}

// @retail 0x2fc390
class_2fc200::class_2fc200(lists_2fc200 const *lists) :
	valued_2fc320(&lists->field_0)
{
	long second_size = lists->field_10.size;
	referenced_2d88a0 **first = lists->field_4.data;
	long count = lists->field_4.size;
	function_2fc2a0(first, count, second_size == count ? lists->field_10.data : 0);
}
