// ARM-safe source-local ownership of the complete static sound arena lifetime.
// Retail function 0x12000 publishes one 0x23000-byte VirtualAlloc region before
// sound startup; function 0x12080 releases it after sound teardown and clears
// all three arena globals. The recovered bump allocators advance size and fold
// each aligned allocation size into checksum. Retail has no final size or
// checksum comparison, so this lifetime does not invent one.

#include "unknown_11c920.h"
#include <xtl.h>
#include <stdint.h>

static const dword k_sound_static_arena_size = 0x23000;

dword g_510800_pool_base;
long g_510804_pool_size;
dword g_510808_pool_checksum;

bool sound_static_arena_initialize(void)
{
	void *arena = VirtualAlloc(NULL, k_sound_static_arena_size,
		MEM_COMMIT, PAGE_READWRITE);
	g_510800_pool_base = (dword)(uintptr_t)arena;
	g_510804_pool_size = 0;
	g_510808_pool_checksum = 0;
	return arena != NULL;
}

void sound_static_arena_dispose(void)
{
	if (g_510800_pool_base)
	{
		VirtualFree((void *)(uintptr_t)g_510800_pool_base, 0, MEM_RELEASE);
		g_510800_pool_base = 0;
		g_510804_pool_size = 0;
		g_510808_pool_checksum = 0;
	}
}

static_assert(sizeof(void *) == 4, "guest pointers must remain 32-bit");
static_assert(sizeof(long) == 4, "guest long must remain 32-bit");
