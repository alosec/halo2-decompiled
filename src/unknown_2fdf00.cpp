// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding two arrays of
// four-byte values: its destructor, which gives back the storage each array
// owns.
#include "reference_counted_2d88a0.h"

struct values_2fdf00
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	~values_2fdf00()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			void *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

class two_arrays_2fdf00
{
public:
	long field_0;
	values_2fdf00 first;
	values_2fdf00 second;

	~two_arrays_2fdf00();
};

// @retail 0x2fdf00
two_arrays_2fdf00::~two_arrays_2fdf00()
{
}

// destroys one object in place, as a callback
// @retail 0x2fdf90
void __cdecl function_2fdf90(two_arrays_2fdf00 *object)
{
	object->~two_arrays_2fdf00();
}
