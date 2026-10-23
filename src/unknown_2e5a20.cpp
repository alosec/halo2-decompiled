// @flags /O2 /GL-
// Havok (third-party physics library): puts one object in a whole row and a
// whole column of an 8x8 table of reference-counted objects.
#include "reference_counted_2d88a0.h"

static inline void assign_2e5a20(referenced_2d88a0 *&slot, referenced_2d88a0 *object)
{
	object->reference_count++;
	referenced_2d88a0 *old = slot;
	if (--old->reference_count == 0)
	{
		delete old;
	}
	slot = object;
}

class table_2e5a20
{
public:
	char unknown0[0xc];
	referenced_2d88a0 *entry[8][8];

	void set_all(referenced_2d88a0 *object, long index);
};

// @retail 0x2e5a20
void table_2e5a20::set_all(referenced_2d88a0 *object, long index)
{
	for (int i = 0; i < 8; i++)
	{
		assign_2e5a20(entry[i][index], object);
		assign_2e5a20(entry[index][i], object);
	}
}
