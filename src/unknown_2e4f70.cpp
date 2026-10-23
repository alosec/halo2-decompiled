// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating an object of 0x80 bytes
// sized by the number of bits a count needs, and destroying one.
#include "tagged_objects_2e4ff0.h"

// the object for a count of at least one, built with the position of the
// count's highest set bit
// @retail 0x2e4f70
class_2e4ff0 *__cdecl function_2e4f70(long arg_0, long arg_1, int count)
{
	if (count == 0)
	{
		count = 1;
	}
	int bits = -1;
	while (count > 0)
	{
		count >>= 1;
		bits++;
	}
	return new class_2e4ff0(arg_0, arg_1, bits);
}

// @retail 0x2e4ff0
class_2e4ff0::~class_2e4ff0()
{
	for (int i = 0; i < entry_count; i++)
	{
		entries[i].~entry_2e4ff0();
	}
	entry_2e4ff0 *block = entries;
	g_memory_2d88a0->release(block);
}
