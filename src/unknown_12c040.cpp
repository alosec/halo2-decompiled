// @flags /O2 /Ob1 /Gr
#include "pool_storage.h"
/* UNKNOWN_12C040.CPP: the block the 0x82 allocator (XMemAlloc,
   unknown_012280.cpp) hands out: 0xcc bytes taken from the static memory pool
   at startup, handed out whole to the one object of type 1 that asks */

#include "unknown_11c920.h"
#include "crc.h"
#include <xtl.h>
#include <string.h>

/* the static memory pool (defined with the sound code, unknown_221490.cpp) */

/* unknown_012280.cpp */
extern void *g_510c3c;
extern bool g_510c40;

// @retail 0x12c040
void function_12c040(void)
{
	byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
	byte *memory = (byte *)(((dword)top + 15) & ~15);
	long aligned_size = (memory - top) + 0xcc;

	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));
	g_510c3c = memory;
	g_510c40 = false;
}

// @retail 0x12c090
LPVOID WINAPI function_12c090(SIZE_T dwSize, DWORD dwAllocAttributes)
{
	XALLOC_ATTRIBUTES *attributes = (XALLOC_ATTRIBUTES *)&dwAllocAttributes;

	if (attributes->dwObjectType == 1)
	{
		void *block = g_510c3c;

		g_510c40 = true;
		if (attributes->dwZeroInitialize)
		{
			memset(block, 0, 0xcc);
		}
		return block;
	}
	return XMemAllocDefault(dwSize, dwAllocAttributes);
}
