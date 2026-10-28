// @flags /O2 /GL-
// Havok (third-party physics library): finding a key's slot in an
// open-addressing table (mask + 1 when it is absent), and emptying a slot.

#include "hash_table_2d9240.h"

// @retail 0x2d9370
unsigned long class_2d9240::find(unsigned long key) const
{
	for (unsigned long index = (key >> 4) * 0x9e3779b1 & mask; slots[index] != 0; index = (index + 1) & mask)
	{
		if (slots[index] == key)
		{
			return index;
		}
	}
	return mask + 1;
}

// empties a slot (values follow the mask + 1 keys), then moves later keys of
// the same run back into the hole when their home slot allows it
// @retail 0x2d93b0
void class_2d9240::function_2d93b0(unsigned long hole)
{
	field_4--;
	slots[hole] = 0;
	unsigned long start = (hole + mask) & mask;
	while (slots[start] != 0)
	{
		start = (start + mask) & mask;
	}
	start = (start + 1) & mask;
	for (unsigned long index = (hole + 1) & mask; slots[index] != 0; index = (index + 1) & mask)
	{
		unsigned long const key = slots[index];
		unsigned long const home = (key >> 4) * 0x9e3779b1 & mask;
		if ((index >= start && home > hole) || (index < hole && (home > hole || home <= index)) || (home > hole && home < start))
		{
			continue;
		}
		slots[hole] = key;
		slots[mask + 1 + hole] = slots[mask + 1 + index];
		slots[index] = 0;
		hole = index;
	}
}
