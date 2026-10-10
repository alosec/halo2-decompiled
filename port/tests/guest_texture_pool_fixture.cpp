#include "data_array.h"
#include "physical_memory.h"

#include <string.h>

static_assert(sizeof(s_record_pool) == 0x4c, "s_record_pool");
static_assert(sizeof(s_physical_block) == 0x18, "s_physical_block");

struct D3DTexture
{
	dword Common;
	dword Data;
	dword Lock;
	dword Format;
	dword Size;
};
static_assert(sizeof(D3DTexture) == 0x14, "D3DTexture");

struct s_bitmap_data
{
	dword signature;
	short width;
	short height;
	char depth;
	byte unknown09;
	short type;
	short source_format;
	word flags;
	byte unknown10[4];
	short mipmap_count;
	byte level_bias;
	byte cache_format;
	byte unknown18[4];
	long data_offsets[3];
	long block_indices[3];
	long level_sizes[3];
	long level_addresses[4];
	D3DTexture *texture;
	long unknown54;
	real minimum_scale;
	D3DTexture hardware_texture;
	long last_frame;
};
static_assert(sizeof(s_bitmap_data) == 0x74, "s_bitmap_data");

struct s_texture_pool_entry
{
	short salt;
	byte flags;
	bool resident;
	long pending;
	long priority;
	long hardware_format;
	s_bitmap_data *bitmap;
	D3DTexture resource;
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
static s_bitmap_data g_upload_bitmap;
static byte __attribute__((aligned(0x1000))) g_upload_buffer[0x1000];

s_record_pool *g_4e6454;
s_physical_object *g_4e6464;
dword g_4e6460;
long g_4e6484;
long g_450768[8][24];

struct s_type_7ba8e9;
bool function_12d5a0(s_bitmap_data *bitmap, long level, bool blocking,
	bool alternate, long priority);
long function_1365a0(s_type_7ba8e9 const *bitmap, long mipmap_index);
long log2_floor(dword value);
short bitmap_get_mipmap_count(short width, short height, short depth,
	short format, bool linear, short maximum_levels);

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

static void initialize_texture_pools(long page_count)
{
	initialize_pool(&g_physical_storage.pool, "texture physical", 4,
		sizeof(s_physical_block), (byte *)g_physical_storage.entries,
		g_physical_storage.bitmap);
	initialize_pool(&g_texture_storage.pool, "texture records", 4,
		sizeof(s_texture_pool_entry), (byte *)g_texture_storage.entries,
		g_texture_storage.bitmap);

	memset(&g_physical, 0, sizeof(g_physical));
	g_physical.state = 2;
	g_physical.page_count = page_count;
	g_physical.page_shift = 12;
	g_physical.time = 1;
	g_physical.first = NONE;
	g_physical.last = NONE;
	g_physical.blocks = &g_physical_storage.pool;
	for (long index = 0; index < 8; index++)
		g_physical.limits[index] = 0x7fffffff;

	g_4e6454 = &g_texture_storage.pool;
	g_4e6464 = &g_physical;
	g_4e6460 = (dword)g_upload_buffer;
	g_4e6484 = 0;
}

extern "C" int h2_texture_math_probe(void)
{
	s_bitmap_data bitmap = {};
	bitmap.width = 5;
	bitmap.height = 4;
	bitmap.depth = 1;
	bitmap.source_format = 10;
	bitmap.flags = 2;
	return function_1365a0((s_type_7ba8e9 const *)&bitmap, 0) == 32 &&
		log2_floor(16) == 4 &&
		bitmap_get_mipmap_count(16, 8, 1, 10, false, 10) == 4;
}

extern "C" int h2_texture_pool_probe(void)
{
	initialize_texture_pools(8);

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

extern "C" int h2_texture_upload_initialize(void)
{
	initialize_texture_pools(1);
	memset(g_upload_buffer, 0, sizeof(g_upload_buffer));
	memset(&g_upload_bitmap, 0, sizeof(g_upload_bitmap));
	memset(g_450768, 0, sizeof(g_450768));

	g_upload_bitmap.width = 4;
	g_upload_bitmap.height = 4;
	g_upload_bitmap.depth = 1;
	g_upload_bitmap.source_format = 10;
	g_upload_bitmap.cache_format = 10;
	g_upload_bitmap.data_offsets[0] = 0x800;
	g_upload_bitmap.level_sizes[0] = 16;
	for (long level = 0; level < 3; level++)
		g_upload_bitmap.block_indices[level] = NONE;
	g_450768[0][10] = 7;
	return 1;
}

extern "C" int h2_texture_upload_submit(void)
{
	return function_12d5a0(&g_upload_bitmap, 0, false, false, 3) ? 1 : 0;
}

extern "C" int h2_texture_upload_completed(void)
{
	long block_index = g_upload_bitmap.block_indices[0];
	if (block_index == NONE || g_physical_storage.pool.actual_count != 1 ||
		g_texture_storage.pool.actual_count != 1 || g_4e6484 != 1)
	{
		return 0;
	}

	s_physical_block *block = &g_physical_storage.entries[block_index & 0xffff];
	s_texture_pool_entry *entry = &g_texture_storage.entries[block_index & 0xffff];
	if (block->offset != 0 || block->pages != 1 || !entry->resident ||
		entry->pending != 0 || entry->priority != 3 ||
		entry->bitmap != &g_upload_bitmap ||
		g_upload_bitmap.level_addresses[0] != (long)g_upload_buffer ||
		entry->resource.Common != 0x40001 ||
		entry->resource.Data != ((dword)g_upload_buffer & 0x0fffffff))
	{
		return 0;
	}

	for (long index = 0; index < 16; index++)
	{
		if (g_upload_buffer[index] != (byte)(0xa0 + index))
			return 0;
	}
	for (long index = 16; index < 0x200; index++)
	{
		if (g_upload_buffer[index] != 0)
			return 0;
	}
	return 1;
}
