#include "data_array.h"
#include "physical_memory.h"

#include <string.h>

static_assert(sizeof(s_record_pool) == 0x4c, "s_record_pool");
static_assert(sizeof(s_physical_block) == 0x18, "s_physical_block");

struct s_texture_pool_entry
{
	short salt;
	byte bytes[0x26];
};
static_assert(sizeof(s_texture_pool_entry) == 0x28, "s_texture_pool_entry");

struct s_physical_pool_storage
{
	s_record_pool pool;
	s_physical_block entries[4];
	dword bitmap[1];
};

struct s_texture_pool_storage
{
	s_record_pool pool;
	s_texture_pool_entry entries[4];
	dword bitmap[1];
};

static s_physical_object g_physical;
static s_physical_pool_storage g_physical_storage;
static s_texture_pool_storage g_texture_storage;

static void initialize_pool(s_record_pool *pool, char const *name, long count,
	long size, byte *entries, dword *bitmap)
{
	memset(pool, 0, sizeof(*pool));
	for (long index = 0; index < (long)sizeof(pool->name) - 1 && name[index]; index++)
		pool->name[index] = name[index];
	pool->maximum_count = count;
	pool->size = size;
	pool->valid = true;
	pool->signature = DATA_ARRAY_SIGNATURE;
	pool->first_free_index = 0;
	pool->high_water_index = 0;
	pool->actual_count = 0;
	pool->next_salt = (short)0x8000;
	pool->data = entries;
	pool->bitmap = bitmap;
	memset(entries, 0, count * size);
	memset(bitmap, 0, ((count + 31) >> 5) * sizeof(*bitmap));
}

extern "C" int h2_texture_pool_probe(void)
{
	initialize_pool(&g_physical_storage.pool, "texture physical", 4,
		sizeof(s_physical_block), (byte *)g_physical_storage.entries,
		g_physical_storage.bitmap);
	initialize_pool(&g_texture_storage.pool, "texture records", 4,
		sizeof(s_texture_pool_entry), (byte *)g_texture_storage.entries,
		g_texture_storage.bitmap);

	memset(&g_physical, 0, sizeof(g_physical));
	g_physical.state = 2;
	g_physical.page_count = 8;
	g_physical.page_shift = 12;
	g_physical.time = 1;
	g_physical.first = NONE;
	g_physical.last = NONE;
	g_physical.blocks = &g_physical_storage.pool;
	for (long index = 0; index < 8; index++)
		g_physical.limits[index] = 0x7fffffff;

	long first = function_13d370(&g_physical, 0x1000, 2);
	long second = function_13d370(&g_physical, 0x2000, 2);
	if (first == NONE || second == NONE || first == second)
		return 0;

	s_physical_block *first_block = &g_physical_storage.entries[first & 0xffff];
	s_physical_block *second_block = &g_physical_storage.entries[second & 0xffff];
	if (first_block->offset != 0 || first_block->pages != 1 ||
		first_block->previous != NONE || first_block->next != second ||
		second_block->offset != 1 || second_block->pages != 2 ||
		second_block->previous != first || second_block->next != NONE ||
		g_physical.first != first || g_physical.last != second ||
		g_physical_storage.pool.actual_count != 2)
	{
		return 0;
	}

	if (datum_new_at_index_with_salt(&g_texture_storage.pool, first) != first ||
		datum_new_at_index_with_salt(&g_texture_storage.pool, second) != second ||
		g_texture_storage.pool.actual_count != 2 ||
		g_texture_storage.entries[first & 0xffff].salt != (short)(first >> 16) ||
		g_texture_storage.entries[second & 0xffff].salt != (short)(second >> 16))
	{
		return 0;
	}

	record_pool_release(&g_texture_storage.pool, first);
	return g_texture_storage.pool.actual_count == 1 &&
		g_texture_storage.entries[first & 0xffff].salt == 0 &&
		(g_texture_storage.bitmap[0] & (1u << (first & 0xffff))) == 0;
}
