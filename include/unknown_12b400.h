/* UNKNOWN_12B400.H: the physical memory map (unknown_12b400.cpp):
   allocations from the top of the current stage's heap. Retail inlines these
   everywhere without merging their loads with the caller's. */

#ifndef UNKNOWN_12B400_H
#define UNKNOWN_12B400_H

#include "globals.h"
#include <xtl.h>

__declspec(noinline) void function_12b400(void);

/* the bytes left in the current stage */
__forceinline long physical_memory_available(void)
{
	return g_global_f9ae07.field_20[g_global_f9ae07.field_0] - g_global_f9ae07.field_c_6[g_global_f9ae07.field_0];
}

/* takes size bytes, rounded up to whole pages, from the top of the current
   stage; NULL when it is full */
__forceinline void *physical_memory_malloc_fixed(long size, dword protect)
{
	void *result = NULL;
	long stage = g_global_f9ae07.field_0;
	long limit = g_global_f9ae07.field_c_6[stage];
	long *top = &g_global_f9ae07.field_20[stage];
	long aligned_size = (size + 0xfff) & 0xfffff000;
	long address = g_global_f9ae07.field_20[stage] - aligned_size;

	if (address >= limit)
	{
		*top = address;
		result = (void *)address;
		if (address)
		{
			result = (void *)(address | 0x80000000);
			if (result)
			{
				XPhysicalProtect(result, aligned_size, protect);
			}
		}
	}
	return result;
}

/* starts a new stage with the bounds of the current one */
__forceinline void function_xe0ae94(void)
{
	long stage = g_global_f9ae07.field_0;
	long next = g_global_f9ae07.field_0 + 1;
	g_global_f9ae07.field_c_6[stage + 1] = g_global_f9ae07.field_c_6[stage];
	g_global_f9ae07.field_20[g_global_f9ae07.field_0 + 1] = g_global_f9ae07.field_20[g_global_f9ae07.field_0];
	g_global_f9ae07.field_0 = next;
}

#endif
