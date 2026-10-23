// @flags /O2 /GL- /GX
// Havok (third-party physics library): an entity built from its description
// over the object holding its shape, and destroyed: everything listening to
// it is told, it lets go of the object it holds at 0x54, and its arrays are
// freed.
#include "reference_holder_30b3a0.h"
#include "entity_listeners_315c00.h"

bool __cdecl function_279740(long value);

class deletable_30beb0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

// the entity's description, as far as this file reads it
struct s_311340 : description_30bd90
{
	s_311340();
};

// an array of 4-byte elements, empty when built, its block freed when destroyed
struct words_30beb0
{
	long *data;
	long size;
	long capacity_and_flags;

	words_30beb0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~words_30beb0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			long *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

// the same with room for one element inside it, which it does not own
struct word_30beb0
{
	long *data;
	long size;
	long capacity_and_flags;
	long storage[1];

	word_30beb0()
	{
		capacity_and_flags = 0x80000001;
		data = storage;
		size = 0;
	}
	~word_30beb0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			long *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

// a kind and two values, one half and 0.4 when built
struct material_30beb0
{
	unsigned char kind;
	float field_4;
	float field_8;

	material_30beb0() : field_4(0.5f), field_8(0.4f) {}
};

class class_30bf50 : public class_30b3a0
{
public:
	REFERENCED_NEW_2D88A0(0x28)

	// built from a body's description (0x30beb0)
	class_30bf50(s_311340 const *description);
	virtual ~class_30bf50();

	long field_3c;
	unsigned char field_40;
	long field_44;
	material_30beb0 material;
	referenced_2d88a0 *field_54;
	long field_58;
	short field_5c;
	word_30beb0 field_60;
	words_30beb0 field_70;
	words_30beb0 listeners;
	words_30beb0 field_88;
	words_30beb0 field_94;
};

// @retail 0x30beb0
class_30bf50::class_30bf50(s_311340 const *description)
	: class_30b3a0(description)
{
	collidable.owner = this;
	material.kind = description->field_18;
	field_44 = 0;
	field_40 = 0;
	field_5c = description->field_1a;
	field_54 = 0;
	field_58 = -1;
}

// @retail 0x30bf50
class_30bf50::~class_30bf50()
{
	function_315c00((entity_315c00 *)this);
	referenced_2d88a0 *object = field_54;
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
				((deletable_30beb0 *)object)->destroy(1);
			}
		}
	}
}
