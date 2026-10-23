// @flags /O2 /GL-
// Havok (third-party physics library): an open-addressing table of keys
// followed by their values; an empty key slot holds 0.

#include "hash_table_2d9240.h"

// @retail 0x2d9240
int class_2d9240::first() const
{
	int index;
	for (index = 0; index <= mask; index++)
	{
		if (slots[index] != 0)
		{
			break;
		}
	}
	return index;
}

// @retail 0x2d9260
unsigned long class_2d9240::word_at(int index) const
{
	return slots[mask + index + 1];
}

// @retail 0x2d9280
void class_2d9240::store_word_at(int index, unsigned long value)
{
	slots[mask + index + 1] = value;
}

// @retail 0x2d92a0
int class_2d9240::next(int index) const
{
	for (index++; index <= mask; index++)
	{
		if (slots[index] != 0)
		{
			break;
		}
	}
	return index;
}

// @retail 0x2d92d0
bool_2d9240 class_2d9240::is_valid(int index) const
{
	return bool_2d9240(index <= mask);
}

// stores a value under a key, adding the key when it is new; doubles the
// table first when it is half full
// @retail 0x2d92f0
void class_2d9240::function_2d92f0(unsigned long key, unsigned long value)
{
	if (field_4 * 2 > mask)
	{
		function_2d94d0(mask * 2 + 2);
	}
	long index = (key >> 4) * 0x9e3779b1 & mask;
	for (; slots[index] != 0; index = (index + 1) & mask)
	{
		if (slots[index] == key)
		{
			break;
		}
	}
	field_4 += slots[index] != key;
	slots[index] = key;
	slots[mask + index + 1] = value;
}
