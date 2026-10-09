// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "data_array.h"
#include <string.h>

struct hash_node
{
	void *key;
	dword hash;
	hash_node *next;
	byte data[1];
};

typedef dword (__stdcall *hash_table_hash_proc)(const void *key);
typedef bool (__stdcall *hash_table_compare_proc)(const void *key_a, const void *key_b);

struct hash_table
{
	byte unknown00[0x20];
	dword bucket_count;
	long maximum_count;
	long data_size;
	dword (__stdcall *hash_proc)(const void *key);
	bool (__stdcall *compare_proc)(const void *key_a, const void *key_b);
	c_data_allocator *allocator;
	hash_node *free_list;
	hash_node *buckets[1];
};

// @retail 0x13e210
void hash_table_initialize(hash_table *table)
{
	long buckets_size = table->bucket_count * sizeof(hash_node *);
	memset(table->buckets, 0, (table->data_size + 12) * table->maximum_count + buckets_size);
	hash_node *node = (hash_node *)((byte *)table->buckets + buckets_size);
	long node_size = table->data_size + 12;
	table->free_list = NULL;
	for (long i = table->maximum_count; i > 0; i--)
	{
		node->next = table->free_list;
		table->free_list = node;
		node = (hash_node *)((byte *)node + node_size);
	}
}

// @retail 0x13e1a0
hash_table *function_13e1a0(const char *name, long data_size, long bucket_count,
	hash_table_hash_proc hash_proc, hash_table_compare_proc compare_proc,
	long maximum_count, c_data_allocator *allocator)
{
	hash_table *table = (hash_table *)allocator->allocate(0x3c + bucket_count * sizeof(hash_node *) +
		maximum_count * (data_size + 12));
	if (table)
	{
		strncpy((char *)table->unknown00, name, sizeof(table->unknown00));
		table->hash_proc = hash_proc;
		table->compare_proc = compare_proc;
		table->unknown00[0x1f] = 0;
		table->bucket_count = bucket_count;
		table->maximum_count = maximum_count;
		table->allocator = allocator;
		table->data_size = data_size;
		hash_table_initialize(table);
	}
	return table;
}

#ifndef H2_NATIVE_PORT
// @retail 0x13e270
bool function_13e270(hash_table *table, void *key, const void *data)
{
	bool result = false;
	if (table->free_list)
	{
		dword hash = table->hash_proc(key);
		dword bucket = hash % table->bucket_count;
		hash_node *node = table->free_list;
		table->free_list = node->next;
		node->hash = hash;
		node->key = key;
		memcpy(node->data, data, table->data_size);
		node->next = table->buckets[bucket];
		table->buckets[bucket] = node;
		result = true;
	}
	return result;
}
#endif

#ifndef H2_NATIVE_PORT
// @retail 0x13e2d0
hash_node *function_13e2d0(hash_table *table, void *key)
{
	dword hash = table->hash_proc(key);
	for (hash_node *node = table->buckets[hash % table->bucket_count]; node; node = node->next)
	{
		if (hash == node->hash && table->compare_proc(key, node->key))
			return node;
	}
	return NULL;
}

// @retail 0x13e320
bool hash_table_remove(hash_table *table, void *key)
{
	hash_node *previous = NULL;
	dword bucket = table->hash_proc(key) % table->bucket_count;
	for (hash_node *node = table->buckets[bucket]; node; node = node->next)
	{
		if (table->compare_proc(key, node->key))
		{
			if (!previous)
				table->buckets[bucket] = node->next;
			else
				previous->next = node->next;
			node->next = table->free_list;
			table->free_list = node;
			return true;
		}
		previous = node;
	}
	return false;
}
#endif

// @retail 0x13e3a0
long log2_floor(dword value)
{
	long result = 0;
	if (value > 0)
	{
		while (value != 1)
		{
			value >>= 1;
			result++;
		}
	}
	return result;
}

// @retail 0x13e3c0
long log2_ceiling_plus_one(dword value)
{
	long result = 0;
	if (value > 1)
	{
		value--;
		while (value != 1)
		{
			value >>= 1;
			result++;
		}
	}
	return result + 1;
}

// @retail 0x13e3e0
void function_13e3e0(const dword *a, const dword *b, dword *destination, long bit_count)
{
	for (long i = ((bit_count + 31) >> 5) - 1; i >= 0; i--)
		destination[i] = b[i] | a[i];
}

static dword bit_mask_for_count(long bit_count)
{
	long remainder = bit_count & 31;
	dword mask = 0xffffffff;
	if (remainder > 0)
		mask >>= 32 - remainder;
	return mask;
}

static dword popcount32(dword v)
{
	v = ((v >> 1) & 0x55555555) + (v & 0x55555555);
	v = ((v >> 2) & 0x33333333) + (v & 0x33333333);
	v = ((v >> 4) & 0x0f0f0f0f) + (v & 0x0f0f0f0f);
	v = ((v >> 8) & 0x00ff00ff) + (v & 0x00ff00ff);
	return (v >> 16) + (v & 0xffff);
}

// @retail 0x13e420
long bit_vector_count_bits(const dword *bits, long bit_count)
{
	long word_count = (bit_count + 31) >> 5;
	long total = 0;
	for (long i = 0; i < word_count - 1; i++)
		total += popcount32(bits[i]);
	return total + popcount32(bits[word_count - 1] & bit_mask_for_count(bit_count));
}

static long highest_set_bit(dword value)
{
	long result = -1;
	if (value)
	{
		__asm { bsr eax, value }
		__asm { mov result, eax }
	}
	return result;
}

// @retail 0x13e530
long bit_vector_highest_set_bit(const dword *bits, long bit_count)
{
	long word_count = (bit_count + 31) >> 5;
	long result = highest_set_bit(bits[word_count - 1] & bit_mask_for_count(bit_count));
	if (result >= 0)
		result += word_count * 32 - 32;
	if (result == -1)
	{
		long i = word_count - 2;
		do
		{
			if (i < 0)
				break;
			result = highest_set_bit(bits[i]);
			if (result >= 0)
				result += i * 32;
			i--;
		}
		while (result == -1);
	}
	return result;
}
