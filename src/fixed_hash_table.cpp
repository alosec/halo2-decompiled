// @flags /O2 /Gr
/* FIXED_HASH_TABLE.CPP: a fixed-size hash table: a named header, its buckets, then
   maximum_count nodes of data_size bytes each, all in one allocation. The
   unused nodes form a free list. */

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>
#include <string.h>

/* a node: its key, the key's hash, the next node of its bucket (or of the
   free list), then the caller's data */
struct hash_node
{
	void *key;
	dword hash;
	hash_node *next;
	byte data[1];
};
typedef char assert_hash_node_data[(offsetof(hash_node, data) == 0xc) ? 1 : -1];

typedef dword (__stdcall *hash_table_hash_proc)(const void *key);
typedef bool (__stdcall *hash_table_compare_proc)(const void *key_a, const void *key_b);

struct hash_table
{
	char name[0x20];
	dword bucket_count;
	long maximum_count;
	long data_size;
	dword (__stdcall *hash_proc)(const void *key);
	bool (__stdcall *compare_proc)(const void *key_a, const void *key_b);
	c_data_allocator *allocator;
	hash_node *free_list;
	hash_node *buckets[1];
};
typedef char assert_hash_table_buckets[(offsetof(hash_table, buckets) == 0x3c) ? 1 : -1];
typedef char assert_hash_table_size[(sizeof(hash_table) == 0x40) ? 1 : -1];

/* empties the table: clears the buckets and nodes and chains every node
   into the free list */
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

__declspec(noinline) hash_table *function_13e1a0(const char *name, long data_size, long bucket_count,
	hash_table_hash_proc hash_proc, hash_table_compare_proc compare_proc,
	long maximum_count, c_data_allocator *allocator);

/* allocates and initializes a table of maximum_count nodes */
// @retail 0x13e1a0
hash_table *function_13e1a0(const char *name, long data_size, long bucket_count,
	hash_table_hash_proc hash_proc, hash_table_compare_proc compare_proc,
	long maximum_count, c_data_allocator *allocator)
{
	hash_table *table = (hash_table *)allocator->allocate(0x3c + bucket_count * sizeof(hash_node *) +
		maximum_count * (data_size + 12));
	if (table)
	{
		strncpy(table->name, name, sizeof(table->name));
		table->name[sizeof(table->name) - 1] = 0;
		table->bucket_count = bucket_count;
		table->maximum_count = maximum_count;
		table->hash_proc = hash_proc;
		table->compare_proc = compare_proc;
		table->allocator = allocator;
		table->data_size = data_size;
		hash_table_initialize(table);
	}
	return table;
}

#ifndef H2_NATIVE_PORT
/* adds a copy of data under key, in front of its bucket; false when the
   table is full */
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
/* the node holding key, or NULL */
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

/* removes the node holding key, returning it to the free list */
// @retail 0x13e320
bool hash_table_remove(hash_table *table, void *key)
{
	bool result = false;
	dword bucket = table->hash_proc(key) % table->bucket_count;
	hash_node *previous = NULL;
	for (hash_node *node = table->buckets[bucket]; node; node = node->next)
	{
		if (table->compare_proc(*(void *volatile *)&key, node->key))
		{
			if (!previous)
				table->buckets[bucket] = node->next;
			else
				previous->next = node->next;
			node->next = table->free_list;
			table->free_list = node;
			result = true;
			break;
		}
		previous = node;
	}
	return result;
}
#endif
