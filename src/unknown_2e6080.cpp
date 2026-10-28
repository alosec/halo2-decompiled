// @flags /O2 /GL- /GX
// Havok (third-party physics library): destroying the collision dispatcher,
// and recording the shape types handled as other ones.
#include "agent_table_2e5d50.h"

// @retail 0x2e6160 deleting
// @retail 0x2e6080
s_1c3a40::~s_1c3a40()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			c_sized_reference_a9ef0 *object = response_objects[i][j];
			if (object)
			{
				object->reference_count--;
				if (object->reference_count == 0)
					delete object;
			}
		}
	}
}

// @retail 0x2e6190
void __cdecl function_2e6190(s_1c3a40 *table)
{
	table->function_2e5e70(4, 1);
	table->function_2e5e70(5, 1);
	table->function_2e5e70(6, 1);
	table->function_2e5e70(7, 1);
	table->function_2e5e70(8, 1);
	table->function_2e5e70(0x17, 1);
	table->function_2e5e70(0xb, 2);
	table->function_2e5e70(0xa, 2);
	table->function_2e5e70(0x13, 3);
	table->function_2e5e70(1, 0x10);
	table->function_2e5e70(0x12, 0xd);
	table->function_2e5e70(0xe, 0xd);
}
