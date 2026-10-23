/* PHYSICAL_MEMORY.H: a physical memory allocator, cut into blocks of pages
   (src/unknown_13d170.cpp). The xbox animation cache (g_4e3b54,
   src/animation_cache.cpp) and the geometry cache
   (g_4e649c, src/unknown_12d9f0.cpp and src/unknown_12de70.cpp) each own one;
   the sound cache's page allocator (src/unknown_218850.cpp) is one too. The
   blocks are kept in a data array that follows the object, and are linked in
   a doubly linked list by their handles. */

#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#include "unknown_11c920.h"
#include "data_array.h"

/* a block (0x18 bytes): its pages, the list links, and the allocator's clock
   when it was last used */
struct s_physical_block
{
	short salt;
	short unknown02;
	long pages;
	long offset;
	long next;
	long previous;
	long time;
};

/* the owner's callbacks for a block: it is freed, and whether it is still
   busy (a load in flight) */
typedef void (__stdcall *physical_block_delete_proc)(long block_index);
typedef bool (__stdcall *physical_block_busy_proc)(long block_index);
typedef byte (__stdcall *physical_block_state_proc)(long block_index);

class c_data_allocator;

struct s_physical_object
{
	void method_13d8b0(long pages);
	void block_delete(long handle);

	char name[0x20];
	physical_block_delete_proc delete_proc;
	physical_block_busy_proc busy_proc;
	physical_block_state_proc state_proc;
	long state;
	long page_count;
	long page_shift;
	long time;
	long first;
	long last;
	long limits[8];
	s_record_pool *blocks;
	dword signature;
	c_data_allocator *allocator;
};

void function_13d170(s_physical_object *manager, const char *name, long a3, long page_shift, long maximum_count, physical_block_delete_proc delete_proc, physical_block_busy_proc busy_proc, physical_block_state_proc state_proc, c_data_allocator *allocator);

/* allocates an allocator with room for its blocks' data array, and builds it */
inline s_physical_object *physical_memory_new(const char *name, long a3, long page_shift, long maximum_count, physical_block_delete_proc delete_proc, physical_block_busy_proc busy_proc, physical_block_state_proc state_proc, c_data_allocator *allocator)
{
	s_physical_object *physical = (s_physical_object *)allocator->allocate(sizeof(s_physical_object) + sizeof(s_record_pool) + maximum_count * sizeof(s_physical_block) + ((maximum_count + 31) >> 5) * 4);

	function_13d170(physical, name, a3, page_shift, maximum_count, delete_proc, busy_proc, state_proc, allocator);
	return physical;
}

/* 0x13d950: the pages of the blocks used within the last age ticks, or busy */
long physical_memory_used_pages(s_physical_object *physical, long age);

long __stdcall function_13d370(s_physical_object *physical, long size, long type);

/* 0x13d230: the clock wrapped; ages every block relative to a restarted clock */
void physical_memory_reset_time(s_physical_object *physical);

/* 0x13d2b0: frees every block */
void physical_memory_flush(s_physical_object *physical);


/* a new frame: the clock advances and the per-type limits are lifted */
inline void physical_memory_new_frame(s_physical_object *physical)
{
	if (physical->time == 0x7fffffff)
	{
		physical_memory_reset_time(physical);
	}
	else
	{
		physical->time++;
	}
	physical->limits[0] = 0x7fffffff;
	physical->limits[1] = 0x7fffffff;
	physical->limits[2] = 0x7fffffff;
	physical->limits[3] = 0x7fffffff;
	physical->limits[4] = 0x7fffffff;
	physical->limits[5] = 0x7fffffff;
	physical->limits[6] = 0x7fffffff;
	physical->limits[7] = 0x7fffffff;
}

#endif
