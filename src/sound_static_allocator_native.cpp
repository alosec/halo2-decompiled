// @flags /O2 /Ob1 /Gr
// ARM-safe selective ownership of the compact recovered sound static-pool
// reservation. The Xbox allocator entry points remain in unknown_12c040.cpp.

#include "unknown_11c920.h"
#include "crc.h"
#include <stddef.h>
#include <stdint.h>

extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;
extern void *g_510c3c;
extern bool g_510c40;

// @retail 0x12c040
void function_12c040(void)
{
	byte *top = (byte *)(uintptr_t)(g_510800_pool_base +
		(dword)g_510804_pool_size);
	byte *memory = (byte *)(uintptr_t)(((dword)(uintptr_t)top + 15) & ~15u);
	long aligned_size = (long)(memory - top) + 0xcc;

	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size,
		(long)sizeof(aligned_size));
	g_510c3c = memory;
	g_510c40 = false;
}

static_assert(sizeof(void *) == 4, "guest pointers must remain 32-bit");
static_assert(sizeof(long) == 4, "guest long must remain 32-bit");
