// @flags /O2 /GL- /GX
// Havok (third-party physics library): a scratch buffer shared through four
// globals (start, cursor, limit 16 bytes before the end, end), owned by a
// reference-counted object.
#include "scratch_2d88a0.h"

char *g_480150; // start
char *g_480154; // cursor
char *g_480158; // limit
char *g_48015c; // end

// @retail 0x2d88a0
scratch_owner_2d88a0::~scratch_owner_2d88a0()
{
	g_memory_2d88a0->release(g_480150);
	g_480150 = 0;
	g_48015c = 0;
	g_480154 = 0;
	g_480158 = 0;
}

// @retail 0x2d8910
void __cdecl function_2d8910(long size)
{
	if (size != g_48015c - g_480150)
	{
		g_memory_2d88a0->release(g_480150);
		g_480150 = (char *)g_memory_2d88a0->allocate(size, 0x16);
		g_480154 = g_480150;
		g_48015c = g_480150 + size;
		g_480158 = g_48015c - 16;
	}
}

// @retail 0x2d8960 deleting scratch_owner_2d88a0
