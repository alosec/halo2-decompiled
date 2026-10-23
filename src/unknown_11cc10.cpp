// @flags /O2 /Gr
#include "pool_storage.h"
/* UNKNOWN_11CC10.CPP: a lifecycle callback (entry 2, field_c_5) */

#include "unknown_11c920.h"
#include "crc.h"
#include "data_array.h"
#include "globals.h"


/* the static memory pool (defined with the sound code, unknown_221490.cpp) */

// @retail 0x11cc10
void function_11cc10(void)
{
	g_5107fc = true;
}

// @retail 0x11cc20
s_record_pool *function_11cc20(long maximum_count, const char *name, long size)
{
	byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
	byte *memory = (byte *)(((dword)top + 3) & ~3);
	long aligned_size = (memory - top) + sizeof(s_record_pool) + maximum_count * size + ((maximum_count + 31) >> 5) * 4;

	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));
	function_16b5f0((s_record_pool *)memory, name, maximum_count, size, 0, g_46875c);
	return (s_record_pool *)memory;
}

s_pool_storage g_pool_storage;
