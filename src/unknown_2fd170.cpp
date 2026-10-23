// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object holding two arrays of
// vectors: its destructor, which gives back the storage each array owns.
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>

struct vectors_2fd170
{
	__m128 *data;
	long size;
	unsigned long capacity_and_flags;

	~vectors_2fd170()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			void *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(__m128), 0x12);
		}
	}
};

class two_arrays_2fd170
{
public:
	long field_0;
	long field_4;
	vectors_2fd170 first;
	vectors_2fd170 second;

	~two_arrays_2fd170();
};

// @retail 0x2fd170
two_arrays_2fd170::~two_arrays_2fd170()
{
}

// destroys one object in place, as a callback
// @retail 0x2fd200
void __cdecl function_2fd200(two_arrays_2fd170 *object)
{
	object->~two_arrays_2fd170();
}
