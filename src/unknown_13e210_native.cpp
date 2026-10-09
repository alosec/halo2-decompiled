// Native source split of the recovered hash-table insertion at retail 0x13e270.
// The matching Xbox definition remains in unknown_13e210.cpp, whose unrelated
// bit-scan helper uses x86 inline assembly. This routine deliberately preserves
// the table-owned hash callback; callback readiness is a BSP-dispatch execution
// precondition rather than something this dormant provider may fabricate.

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>

struct hash_node
{
    void *key;
    dword hash;
    hash_node *next;
    byte data[1];
};

typedef dword (__stdcall *hash_table_hash_proc_13e270_native)(void const *key);
typedef bool (__stdcall *hash_table_compare_proc_13e270_native)(
    void const *key_a, void const *key_b);

struct hash_table
{
    byte unknown00[0x20];
    dword bucket_count;
    long maximum_count;
    long data_size;
    hash_table_hash_proc_13e270_native hash_proc;
    hash_table_compare_proc_13e270_native compare_proc;
    void *allocator;
    hash_node *free_list;
    hash_node *buckets[1];
};

static_assert(offsetof(hash_node, data) == 0x0c,
    "hash node data offset");
static_assert(offsetof(hash_table, bucket_count) == 0x20,
    "hash table bucket-count offset");
static_assert(offsetof(hash_table, data_size) == 0x28,
    "hash table data-size offset");
static_assert(offsetof(hash_table, hash_proc) == 0x2c,
    "hash table hash callback offset");
static_assert(offsetof(hash_table, free_list) == 0x38,
    "hash table free-list offset");
static_assert(offsetof(hash_table, buckets) == 0x3c,
    "hash table buckets offset");

bool function_13e270(hash_table *table, void *key, void const *data)
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

// @retail 0x13e2d0
hash_node *function_13e2d0(hash_table *table, void *key)
{
    dword hash = table->hash_proc(key);
    for (hash_node *node = table->buckets[hash % table->bucket_count];
         node; node = node->next)
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
    for (hash_node *node = table->buckets[bucket];
         node; node = node->next)
    {
        if (table->compare_proc(key, node->key))
        {
            if (previous)
                previous->next = node->next;
            else
                table->buckets[bucket] = node->next;
            node->next = table->free_list;
            table->free_list = node;
            return true;
        }
        previous = node;
    }
    return false;
}
