// @flags /O2 /GL-
// Havok (third-party physics library): a zeroed 128-byte table with a mask
// of 15.
#include "reference_counted_2d88a0.h"

void __cdecl function_2d8de0(void *buffer, int value, unsigned long size);

class table_2d9470
{
public:
	void *entries;
	long field_4;
	long mask;

	table_2d9470();
};

// @retail 0x2d9470
table_2d9470::table_2d9470()
{
	entries = g_memory_2d88a0->allocate_block(0x80, 0x12);
	function_2d8de0(entries, 0, 0x80);
	field_4 = 0;
	mask = 0xf;
}
