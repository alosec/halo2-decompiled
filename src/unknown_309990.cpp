// @flags /O2 /GL- /GX
// Havok (third-party physics library): the destructor of an object holding
// three arrays (of 4-, 80- and 32-byte entries), each giving back the
// storage it owns.
#include "reference_counted_2d88a0.h"

struct entry50_309990
{
	char bytes[0x50];
};

struct entry20_309990
{
	char bytes[0x20];
};

struct values_309990
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	~values_309990()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct entries50_309990
{
	entry50_309990 *data;
	long size;
	unsigned long capacity_and_flags;

	~entries50_309990()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			void *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(entry50_309990), 0x12);
		}
	}
};

struct entries20_309990
{
	entry20_309990 *data;
	long size;
	unsigned long capacity_and_flags;

	~entries20_309990()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			void *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(entry20_309990), 0x12);
		}
	}
};

class holder_309990
{
public:
	long field_0;
	long field_4;
	values_309990 values;
	char unknown14[0x19c];
	entries50_309990 entries50;
	char unknown1bc[0x1f44];
	entries20_309990 entries20;

	~holder_309990();
};

// @retail 0x309990
holder_309990::~holder_309990()
{
}
