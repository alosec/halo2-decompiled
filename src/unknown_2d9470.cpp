// @flags /O2 /GL-
// Havok (third-party physics library): a zeroed 128-byte table with a mask
// of 15.
#include "reference_name_registry.h"

void __cdecl function_2d8de0(void *buffer, int value, unsigned long size);

// @retail 0x2d9470
lane_a_name_table::lane_a_name_table()
{
	slots = (unsigned long *)g_memory_2d88a0->allocate_block(0x80, 0x12);
	function_2d8de0(slots, 0, 0x80);
	field_4 = 0;
	mask = 0xf;
}
