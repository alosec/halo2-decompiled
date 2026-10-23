// @flags /O2 /GL- /GX
// Havok (third-party physics library): three objects made for an object and
// put, as kinds 1, 2 and 3, in every row and column of its table; the table
// keeps its own references to them.
#include "owned_reference_318b90.h"

bool __cdecl function_279740(long value);

class deletable_30c770
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

class table_2e5a20
{
public:
	char unknown0[0xc];
	referenced_2d88a0 *entry[8][8];

	void set_all(referenced_2d88a0 *object, long index);
};

// the third object's other kind of object
class second_30c770
{
public:
	virtual void slot_0() {}
};

// the third object, of kind 3
class third_30c770 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(4)

	second_30c770 field_8;
	long field_c;
};

// drops a reference, deleting the object with the last one; objects that
// came with the cache file are never deleted, their count goes back to 1
inline void release_30c770(referenced_2d88a0 *object)
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
			// the deleting destructor, first in the vtable, without the null
			// test `delete` adds
			((deletable_30c770 *)object)->destroy(1);
		}
	}
}

class class_30c770
{
public:
	char unknown0[0xc8];
	table_2e5a20 *table;

	void function_30c770();
};

// @retail 0x30c770
void class_30c770::function_30c770()
{
	referenced_2d88a0 *first = (referenced_2d88a0 *)new class_319080(this);
	referenced_2d88a0 *second = (referenced_2d88a0 *)new class_318b90(this);
	third_30c770 *third = new third_30c770;
	table_2e5a20 *kinds = table;
	kinds->set_all(first, 1);
	kinds->set_all(second, 2);
	kinds->set_all(third, 3);
	release_30c770(first);
	release_30c770(second);
	release_30c770(third);
}
