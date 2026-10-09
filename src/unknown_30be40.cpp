// @flags /O2 /Gr

#include "unknown_03d380.h"

void __cdecl function_2d91d0(void *array, long element_size);

// @retail 0x30be40
void s_47f048_object::function_30be40(long value)
{
	byte *array = (byte *)this + 0x60;
	long &count = *(long *)(array + 4);
	long &capacity_and_flags = *(long *)(array + 8);
	if (count == (capacity_and_flags & 0x7fffffff))
		function_2d91d0(array, sizeof(long));
	long *data = *(long **)array;
	data[count] = value;
	++count;
}
