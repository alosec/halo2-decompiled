#include "unknown_11c920.h"
#include <string.h>
#ifndef H2_NATIVE_PORT
#include <xmmintrin.h>
#endif
#include "globals.h"
#include "data_array.h"
#include "physical_memory.h"

// @flags /O2 /Gr

/* the physical memory allocator (physical_memory.h): a named, doubly linked
   list of blocks kept in a data array that follows the object */

// @retail 0x13d170
void function_13d170(s_physical_object *manager, const char *name, long page_count, long page_shift, long maximum_count, physical_block_delete_proc delete_proc, physical_block_busy_proc busy_proc, physical_block_state_proc state_proc, c_data_allocator *allocator)
{
	s_record_pool *data = (s_record_pool *)(manager + 1);

	function_16b5f0(data, name, maximum_count, sizeof(s_physical_block), 0, g_46875c);
	data->valid = 1;
	record_pool_release_all(data);

	memset(manager, 0, sizeof(s_physical_object));
	strncpy(manager->name, name, 0x20);
	manager->delete_proc = delete_proc;
	manager->state_proc = state_proc;
	manager->page_shift = page_shift;
	manager->busy_proc = busy_proc;
	manager->blocks = data;
	manager->time = 1;
	manager->page_count = page_count;
	manager->signature = 0x77656565;
	manager->name[0x1f] = 0;
	manager->state = 0;
	manager->allocator = allocator;
	manager->first = NONE;
	manager->last = NONE;
	manager->limits[0] = 0x7fffffff;
	manager->limits[1] = 0x7fffffff;
	manager->limits[2] = 0x7fffffff;
	manager->limits[3] = 0x7fffffff;
	manager->limits[4] = 0x7fffffff;
	manager->limits[5] = 0x7fffffff;
	manager->limits[6] = 0x7fffffff;
	manager->limits[7] = 0x7fffffff;
}

// @retail 0x13d230
void physical_memory_reset_time(s_physical_object *physical)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	physical->time = 0x101;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		dword age = 0x7fffffff - block->time;

		if (age > 0x100)
		{
			block->time = 0;
		}
		else
		{
			block->time = 0x100 - age;
		}
	}
}

// @retail 0x13d2b0
void physical_memory_flush(s_physical_object *physical)
{
	s_record_pool_iterator iterator;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		physical->block_delete(iterator.datum_index);
	}
}

/* frees a block: the owner's delete callback, then unlinks it */
// @retail 0x13d830
void s_physical_object::block_delete(long handle)
{
	s_physical_block *entry = &((s_physical_block *)blocks->data)[handle & 0xffff];

	if (delete_proc)
		delete_proc(handle);

	if (entry->previous != NONE)
		((s_physical_block *)blocks->data)[entry->previous & 0xffff].next = entry->next;
	else
		first = entry->next;

	if (entry->next != NONE)
		((s_physical_block *)blocks->data)[entry->next & 0xffff].previous = entry->previous;
	else
		last = entry->previous;

	record_pool_release(blocks, handle);
}

PRIVATE __forceinline byte *function_13d8b1(s_record_pool_iterator *arg_1)
{
	s_record_pool *local_1 = arg_1->data;
	long local_2 = data_find_index(local_1, arg_1->index + 1);
	byte *local_3;
	if (local_2 != NONE)
	{
		local_3 = local_1->data + local_1->size * local_2;
		arg_1->index = local_2;
		arg_1->datum_index = (*(short *)local_3 << 16) | local_2;
	}
	else
	{
		arg_1->index = local_1->maximum_count;
		arg_1->datum_index = NONE;
		local_3 = 0;
	}
	return local_3;
}

/* resizes the allocator to a number of pages, freeing the blocks past its end */
// @retail 0x13d8b0
void s_physical_object::method_13d8b0(long pages)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)function_13d8b1(&iterator)) != NULL)
	{
		if (block->offset + block->pages > pages)
		{
			block_delete(iterator.datum_index);
		}
	}
	page_count = pages;
}

// @retail 0x13d950
long physical_memory_used_pages(s_physical_object *physical, long age)
{
	long result = 0;
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if ((dword)(block->time + age) >= (dword)physical->time || physical->busy_proc && physical->busy_proc(iterator.datum_index))
		{
			result += block->pages;
		}
	}
	return result;
}

struct s_physical_block_view;
struct s_physical_allocator_view;
bool function_13d320(s_physical_allocator_view const *allocator, s_physical_block_view const *a, s_physical_block_view const *b);

struct s_block_candidate
{
	long previous;
	dword time;
	long offset;
	long pages;
};

// @retail 0x13d370
long __stdcall function_13d370(s_physical_object *physical, long size, long type)
{
	long pages = size >> physical->page_shift;
	if (size & ((1 << physical->page_shift) - 1))
		pages++;
	long result = NONE;
	long limit_index = type < 0 ? 0 : (type > 8 ? 8 : type);
	if (pages < physical->limits[limit_index] || limit_index <= 0)
	{
		s_block_candidate candidates[256];
		s_block_candidate best;
		bool have_best = false;
		long head = 0;
		long tail = 0;
		long offset = 0;
		long previous = NONE;
		long current = physical->first;
		long oldest = NONE;
		dword oldest_time;
		while (offset < physical->page_count)
		{
			long next_tail = (tail + 1) % 256;
			if (next_tail != head)
			{
				s_block_candidate *candidate = &candidates[tail];
				candidate->previous = previous;
				candidate->time = 0;
				candidate->offset = offset;
				candidate->pages = 0;
				tail = next_tail;
			}
			long segment_pages;
			dword segment_time = 0;
			bool busy = false;
			if (current == NONE)
			{
				segment_pages = physical->page_count - offset;
				offset = physical->page_count;
			}
			else
			{
				s_physical_block *block = &((s_physical_block *)physical->blocks->data)[current & 0xffff];
				if (offset == block->offset)
				{
					segment_pages = block->pages;
					segment_time = block->time;
					busy = physical->busy_proc && physical->busy_proc(current);
					if ((dword)(block->time + type) >= (dword)physical->time)
						busy = true;
					else if (!busy && (oldest == NONE || (dword)block->time < oldest_time))
					{
						oldest = current;
						oldest_time = block->time;
					}
					previous = current;
					current = block->next;
					offset = block->pages + block->offset;
#ifdef H2_NATIVE_PORT
					__builtin_prefetch(&((s_physical_block *)physical->blocks->data)[current & 0xffff]);
#else
					_mm_prefetch((char *)&((s_physical_block *)physical->blocks->data)[current & 0xffff], _MM_HINT_T0);
#endif
				}
				else
				{
					segment_pages = block->offset - offset;
					offset = block->offset;
				}
			}
			if (busy)
				head = tail;
			else
			{
				for (long i = head; i != tail; )
				{
					s_block_candidate *candidate = &candidates[i];
					long next = (i + 1) % 256;
#ifdef H2_NATIVE_PORT
					__builtin_prefetch(&candidates[next]);
#else
					_mm_prefetch((char *)&candidates[next], _MM_HINT_T0);
#endif
					if (segment_time > candidate->time)
						candidate->time = segment_time;
					candidate->pages += segment_pages;
					if (candidate->pages >= pages)
					{
						bool better = false;
						if (have_best)
						{
							switch (physical->state)
							{
							case 0:
								better = candidate->time < best.time || (candidate->time == best.time && candidate->pages < best.pages);
								break;
							case 1:
								better = function_13d320((s_physical_allocator_view *)physical, (s_physical_block_view *)candidate, (s_physical_block_view *)&best);
								break;
							default:
								{
									dword age = physical->time - candidate->time;
									dword best_age = physical->time - best.time;
									better = (long)(best.pages * age) > (long)(candidate->pages * best_age);
								}
								break;
							}
						}
						if (!have_best || better)
						{
							best = *candidate;
							have_best = true;
						}
						head = (head + 1) % 256;
					}
					i = next;
				}
			}
		}
		if (have_best)
		{
			s_record_pool_iterator iterator;
			iterator.data = physical->blocks;
			iterator.index = NONE;
			iterator.datum_index = NONE;
			s_physical_block *block;
			while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
			{
				if (block->offset < best.offset + pages && block->offset + block->pages > best.offset)
					physical->block_delete(iterator.datum_index);
			}
			if (physical->blocks->actual_count == physical->blocks->maximum_count && oldest != NONE)
			{
				if (best.previous == oldest)
					best.previous = ((s_physical_block *)physical->blocks->data)[oldest & 0xffff].previous;
				physical->block_delete(oldest);
			}
			result = record_pool_allocate(physical->blocks);
			if (result != NONE)
			{
				s_physical_block *blocks = (s_physical_block *)physical->blocks->data;
				s_physical_block *entry = &blocks[result & 0xffff];
				if (best.previous == NONE)
				{
					entry->previous = NONE;
					if (physical->first == NONE)
						physical->last = result;
					else
						blocks[physical->first & 0xffff].previous = result;
					entry->next = physical->first;
					physical->first = result;
				}
				else
				{
					s_physical_block *before = &blocks[best.previous & 0xffff];
					if (before->next == NONE)
					{
						entry->previous = physical->last;
						physical->last = result;
					}
					else
					{
						s_physical_block *after = &blocks[before->next & 0xffff];
						entry->previous = after->previous;
						after->previous = result;
					}
					entry->next = before->next;
					before->next = result;
				}
				entry->offset = best.offset;
				entry->pages = pages;
				entry->time = physical->time;
				return result;
			}
		}
		if (type == limit_index)
			physical->limits[limit_index] = physical->limits[limit_index] < pages ? physical->limits[limit_index] : pages;
	}
	return result;
}
