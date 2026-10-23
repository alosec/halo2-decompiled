// @flags /O2 /GL- /GX
// Havok (third-party physics library): the destructor of a list of groups,
// which frees each group's index list before the list's own storage.
#include "joined_groups_3720a0.h"

// @retail 0x372300
groups_372300::~groups_372300()
{
	for (int i = 0; i < size; i++)
	{
		long *indices = data[i].data;
		g_memory_2d88a0->release(indices);
	}
}
