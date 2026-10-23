// @flags /O2 /Gr
/* ANIMATION_CACHE.CPP: the xbox animation cache, which streams the animation
   graphs' resources into blocks of a physical memory allocator
   (0x123180..0x123b00). */

#include <xtl.h>
#include "unknown_11c920.h"
#include "unknown_053310.h"
#include "data_array.h"
#include "animation_cache.h"
#include "physical_memory.h"
#include "globals.h"
#include "async.h"

const s_animation_cache_priority g_animation_cache_priorities[3] =
{
	{ 8, 0, 0 },
	{ 5, 10, 30 },
	{ 2, 20, 50 },
};

/* the cache's physical memory ("xbox animation cache") */
s_physical_object *g_4e3b54;
dword g_animation_cache_memory;
long g_animation_cache_memory_size;
long g_animation_cache_page_count;

/* the loads, one per block of the memory ("xbox animation") */
s_record_pool *g_4e3b48;
/* the requests waiting to be loaded ("xbox predicted animations") */
s_record_pool *g_4e3b4c;
/* when a full request array may next be noted (GetTickCount, every 30 s) */
dword g_animation_cache_request_full_time;
/* whether the cache exists, and whether it streams resources */
bool g_510c20;
bool g_510c21;

/* how many times the cache has been updated */
long g_animation_cache_update_count;

void animation_cache_process_requests(void);
long animation_cache_allocate_block(s_cache_resource *resource, long allocation_limit, bool flush);
void animation_cache_load_begin(s_animation_cache_load *load, long name, long priority);
bool animation_cache_load_wait(s_animation_cache_load *load);
void __stdcall animation_cache_block_delete(long block_index);
bool __stdcall animation_cache_block_busy(long block_index);
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long type, long priority);
bool function_120ce0(long job, long priority);

/* steps an iterator to the next datum in use, scanning the array's bitmap */
static __forceinline byte *request_iterator_next(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = iterator->index + 1;
	long next_index = NONE;
	if (index >= 0)
	{
		for (; index < *(long volatile *)&data->high_water_index; index++)
		{
			if (data->bitmap[index >> 5] & (1 << (index & 0x1f)))
			{
				next_index = index;
				break;
			}
		}
	}
	byte *datum;
	if (next_index != NONE)
	{
		datum = data->data + data->size * next_index;
		iterator->index = next_index;
		iterator->datum_index = (*(short *)datum << 16) | next_index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		datum = NULL;
	}
	return datum;
}

/* allocates the cache's memory (smaller from stage 2 on) and hands its pages
   to the physical memory */
// @retail 0x123230
void __stdcall animation_cache_memory_initialize(long stage)
{
	long size;

	if (g_4e3b54)
	{
		if (stage > 1)
		{
			size = 0x300000;
			g_animation_cache_page_count = 0x600;
		}
		else
		{
			size = 0x400000;
			g_animation_cache_page_count = 0x800;
		}

		g_animation_cache_memory_size = size;
		g_animation_cache_memory = (dword)physical_memory_malloc_fixed(size, PAGE_READWRITE);
		g_4e3b54->method_13d8b0(g_animation_cache_page_count);
	}
}

/* takes the cache's pages back from the physical memory */
// @retail 0x1232e0
void animation_cache_memory_dispose(void)
{
	g_animation_cache_page_count = 0;
	g_animation_cache_memory_size = 0;
	if (g_4e3b54)
	{
		g_4e3b54->method_13d8b0(0);
		g_animation_cache_memory = 0;
	}
}

/* the request waiting for a resource, or NONE */
// @retail 0x123680
long animation_cache_find_request(s_cache_resource *resource)
{
	long result = NONE;
	if (resource->block_index == NONE)
	{
		struct
		{
			s_animation_cache_request *request;
			s_record_pool_iterator iterator;
		} search;
		search.iterator.data = g_4e3b4c;
		search.iterator.index = NONE;
		search.iterator.datum_index = NONE;
		while ((search.request = (s_animation_cache_request *)request_iterator_next(&search.iterator)) != NULL)
		{
			if (resource == search.request->resource)
			{
				result = search.iterator.datum_index;
				break;
			}
		}
	}
	return result;
}

/* asks for a resource ahead of its use: queues a request for it, or marks
   its block as recently used if it is loaded already */
// @retail 0x1236f0
void animation_cache_request_resource(s_cache_resource *resource, bool urgent)
{
	if (g_510c21)
	{
		long priority = (urgent ? 0 : 1) + 1;

		if (resource->block_index == NONE)
		{
			if (animation_cache_find_request(resource) == NONE)
			{
				long request_index = record_pool_allocate(g_4e3b4c);

				if (request_index != NONE)
				{
					s_animation_cache_request *request = &((s_animation_cache_request *)g_4e3b4c->data)[request_index & 0xffff];

					request->priority = priority;
					request->resource = resource;
					animation_cache_process_requests();
				}
				else if (GetTickCount() > g_animation_cache_request_full_time)
				{
					g_animation_cache_request_full_time = GetTickCount() + 30000;
				}
			}
		}
		else
		{
			long age = g_animation_cache_priorities[priority].age;
			long *last_used_time = &((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff].time;
			long time = g_4e3b54->time;

			if (time - *last_used_time > age)
			{
				*last_used_time = time - age;
			}
		}
	}
}

/* the resource's data, loading it now (and waiting for it) if it is not
   loaded */
// @retail 0x1237e0
void *animation_cache_get_resource(s_cache_resource *resource, long name)
{
	long request_index = animation_cache_find_request(resource);
	long block_index;
	s_physical_block *block;

	if (request_index != NONE)
	{
		record_pool_release(g_4e3b4c, request_index);
	}
	block_index = resource->block_index;
	if (block_index == NONE)
	{
		s_animation_cache_load *load;

		block_index = animation_cache_allocate_block(resource, 0, true);
		load = &((s_animation_cache_load *)g_4e3b48->data)[datum_new_at_index_with_salt(g_4e3b48, block_index) & 0xffff];
		load->handle = NONE;
		load->done = false;
		load->resource = resource;
		resource->block_index = block_index;
		animation_cache_load_begin(load, name, 0);
	}
	else
	{
		animation_cache_load_wait(&((s_animation_cache_load *)g_4e3b48->data)[block_index & 0xffff]);
	}
	((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff].time = g_4e3b54->time;
	block = &((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff];
	return (void *)(g_animation_cache_memory + (block->offset << g_4e3b54->page_shift));
}

/* creates the load and request arrays and the physical memory, once */
// @retail 0x123180
void animation_cache_initialize(void)
{
	if (!g_510c20)
	{
		g_510c20 = true;
		g_4e3b48 = data_new("xbox animation", 0x200, sizeof(s_animation_cache_load), 0, g_468758);
		g_4e3b4c = data_new("xbox predicted animations", 0x80, sizeof(s_animation_cache_request), 0, g_468758);
		g_4e3b54 = physical_memory_new("xbox animation cache", 0, 0xb, 0x200, animation_cache_block_delete, animation_cache_block_busy, 0, g_468758);
		g_animation_cache_page_count = 0;
		g_animation_cache_memory_size = 0;
		g_animation_cache_memory = 0;
	}
}

/* deletes the load and request arrays and the physical memory */
// @retail 0x123310
void animation_cache_dispose(void)
{
	if (g_510c20)
	{
		if (g_4e3b48)
		{
			data_dispose(g_4e3b48);
			g_4e3b48 = NULL;
		}
		if (g_4e3b4c)
		{
			data_dispose(g_4e3b4c);
			g_4e3b4c = NULL;
		}
		if (g_4e3b54)
		{
			g_4e3b54->allocator->deallocate(g_4e3b54);
			g_4e3b54 = NULL;
		}
	}
	g_510c20 = false;
}

/* starts streaming: empties the load and request arrays and marks them valid */
// @retail 0x1233a0
void animation_cache_activate(void)
{
	if (g_510c20 && !g_510c21)
	{
		g_510c21 = true;
		if (g_4e3b48)
		{
			g_4e3b48->valid = true;
			record_pool_release_all(g_4e3b48);
		}
		if (g_4e3b4c)
		{
			g_4e3b4c->valid = true;
			record_pool_release_all(g_4e3b4c);
		}
	}
}

/* stops streaming: frees every block and marks the arrays invalid */
// @retail 0x1233f0
void animation_cache_deactivate(void)
{
	if (g_510c20 && g_510c21)
	{
		physical_memory_flush(g_4e3b54);
		if (g_4e3b48)
		{
			g_4e3b48->valid = false;
		}
		if (g_4e3b4c)
		{
			g_4e3b4c->valid = false;
		}
		g_510c21 = false;
	}
}

/* each update: advances the physical memory's clock and loads what is
   requested */
// @retail 0x123430
void animation_cache_update(void)
{
	if (g_510c20 && g_510c21)
	{
		s_physical_object *physical = g_4e3b54;
		*(long volatile *)&physical->state = 0;
		long time = *(long volatile *)&physical->time;
		if (time == 0x7fffffff)
			physical_memory_reset_time(physical);
		else
			physical->time = time + 1;
		for (long i = 0; i < 8; i++)
			physical->limits[i] = 0x7fffffff;
		animation_cache_process_requests();
		g_animation_cache_update_count++;
	}
}

/* allocates a block for a resource; when there is no room and flush is set,
   ages every block up to three times to make room */
// @retail 0x1234a0
long animation_cache_allocate_block(s_cache_resource *resource, long allocation_limit, bool flush)
{
	long block_index = function_13d370(g_4e3b54, resource->size, allocation_limit);

	if (block_index == NONE && flush)
	{
		long attempts;

		g_4e3b54->delete_proc = animation_cache_block_delete;
		g_4e3b54->busy_proc = NULL;
		attempts = 3;
		while (block_index == NONE && attempts > 0)
		{
			s_physical_object *physical = g_4e3b54;

			if (physical->time == 0x7fffffff)
			{
				physical_memory_reset_time(physical);
			}
			else
			{
				physical->time++;
			}
			for (long i = 0; i < 8; i++)
			{
				physical->limits[i] = 0x7fffffff;
			}
			block_index = function_13d370(g_4e3b54, resource->size, allocation_limit);
			attempts--;
		}
		g_4e3b54->delete_proc = animation_cache_block_delete;
		g_4e3b54->busy_proc = animation_cache_block_busy;
	}
	return block_index;
}

/* allocates a block and a load for a resource at a priority, or NULL */
// @retail 0x123550
s_animation_cache_load *animation_cache_load_new(s_cache_resource *resource, long priority)
{
	long size = resource->size;
	s_physical_object *physical = g_4e3b54;
	long allocation_limit = g_animation_cache_priorities[priority].allocation_limit;
	s_animation_cache_load *result = NULL;
	long block_index = function_13d370(physical, size, allocation_limit);

	if (block_index != NONE)
	{
		result = &((s_animation_cache_load *)g_4e3b48->data)[datum_new_at_index_with_salt(g_4e3b48, block_index) & 0xffff];
		result->done = false;
		result->handle = NONE;
		result->resource = resource;
		resource->block_index = block_index;
	}
	return result;
}

/* starts the async read of a load's resource into its block; a priority 0
   load is waited for */
// @retail 0x1235b0
void animation_cache_load_begin(s_animation_cache_load *load, long name, long priority)
{
	long const *const name_pointer = &name;
	s_cache_resource *resource = load->resource;
	s_physical_block *block;
	void *buffer;
	long size;

	load->done = false;
	block = &((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff];
	buffer = (void *)(g_animation_cache_memory + (block->offset << g_4e3b54->page_shift));
	size = resource->size;
	if (size & 0x1ff)
	{
		size = (size | 0x1ff) + 1;
	}
	load->done = false;
	struct
	{
		long priority;
		long tag;
	} volatile read;
	read.priority = g_animation_cache_priorities[priority].read_priority;
	dword location = resource->location;
	name = resource->unknown00;
	read.tag = *name_pointer;
	load->handle = function_213760(location, size, buffer, NULL, &load->done, 6, read.priority);
	if (priority == 0)
	{
		animation_cache_load_wait(load);
	}
	((s_physical_block *)g_4e3b54->blocks->data)[load->resource->block_index & 0xffff].time = g_4e3b54->time - g_animation_cache_priorities[priority].age;
}

/* waits for a load's read still in flight; whether it had to */
// @retail 0x1237a0
bool animation_cache_load_wait(s_animation_cache_load *load)
{
	bool result = false;

	if (load->handle != NONE)
	{
		if (!load->done)
		{
			function_120ce0(load->handle, 8);
			function_120d50(&load->done, false);
			result = true;
		}
		load->handle = NONE;
	}
	return result;
}

/* the physical memory's busy callback: whether a block's read is in flight */
// @retail 0x1238b0
bool __stdcall animation_cache_block_busy(long block_index)
{
	bool result = false;
	s_animation_cache_load *load = &((s_animation_cache_load *)g_4e3b48->data)[block_index & 0xffff];

	if (load->handle != NONE && !load->done)
	{
		result = true;
	}
	return result;
}

/* the physical memory's delete callback: forgets a block's load */
// @retail 0x1238f0
void __stdcall animation_cache_block_delete(long block_index)
{
	s_animation_cache_load *load = &((s_animation_cache_load *)g_4e3b48->data)[block_index & 0xffff];

	animation_cache_load_wait(load);
	load->resource->block_index = NONE;
	record_pool_release(g_4e3b48, block_index);
}

/* how many loads are still reading */
// @retail 0x123970
long animation_cache_count_pending_loads(void)
{
	long count = 0;
	s_record_pool_iterator iterator;
	s_animation_cache_load *load;

	iterator.data = g_4e3b48;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((load = (s_animation_cache_load *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (!load->done)
		{
			count++;
		}
	}
	return count;
}

/* steps a datum iterator, keeping the current datum */
static __forceinline byte *request_datum_iterator_next(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	iterator->datum = result;
	return result;
}

/* turns up to 8 waiting requests into loads (urgent ones first) while the
   async queue is short */
// @retail 0x1239d0
void animation_cache_process_requests(void)
{
	s_record_pool *requests = g_4e3b4c;

	if (requests->valid && async_globals.tasks_added <= 25)
	{
		long loads = animation_cache_count_pending_loads();
		s_record_pool_iterator iterator;
		s_data_datum_iterator next_iterator;
		s_animation_cache_request *request;

		iterator.data = requests;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((request = (s_animation_cache_request *)data_iterator_next_calling(&iterator)) != NULL && loads < 8)
		{
			if (request->priority == 1)
			{
				s_cache_resource *resource = request->resource;

				if (!g_510c21)
				{
					continue;
				}
				if (resource->block_index == NONE)
				{
					s_animation_cache_load *load = animation_cache_load_new(resource, 1);

					if (!load)
					{
						continue;
					}
					animation_cache_load_begin(load, 0x7000180, 1);
				}
				record_pool_release(g_4e3b4c, iterator.datum_index);
				loads++;
			}
		}

		next_iterator.data = g_4e3b4c;
		next_iterator.index = NONE;
		next_iterator.datum_index = NONE;
		while ((request = (s_animation_cache_request *)request_datum_iterator_next(&next_iterator)) != NULL && loads < 8)
		{
			long priority = request->priority;
			s_cache_resource *resource = request->resource;

			if (g_510c21)
			{
				if (resource->block_index == NONE)
				{
					s_animation_cache_load *load = animation_cache_load_new(resource, priority);

					if (!load)
					{
						continue;
					}
					animation_cache_load_begin(load, 0x7000180, priority);
				}
				record_pool_release(g_4e3b4c, next_iterator.datum_index);
				loads++;
			}
		}
	}
}
