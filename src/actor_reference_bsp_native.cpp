// Native scalar split for the recovered BSP actor-reference refresh closure.
// Xbox keeps the original definitions in unknown_25d690.cpp and
// unknown_2626b0.cpp. The reference table's callback pointers remain an
// explicit dispatcher execution precondition; this dormant provider never
// substitutes host callbacks for them.

#include "unknown_11c920.h"
#include "globals.h"
#include "ai_actor.h"
#include "props.h"
#include <stddef.h>
#include <string.h>

#if H2_NATIVE_PORT

struct hash_node
{
    void *key;
    dword hash;
    hash_node *next;
    byte data[1];
};

typedef dword (__stdcall *actor_reference_hash_proc)(void const *key);
typedef bool (__stdcall *actor_reference_compare_proc)(
    void const *key_a, void const *key_b);

struct hash_table
{
    byte unknown00[0x20];
    dword bucket_count;
    long maximum_count;
    long data_size;
    actor_reference_hash_proc hash_proc;
    actor_reference_compare_proc compare_proc;
    void *allocator;
    hash_node *free_list;
    hash_node *buckets[1];
};

static_assert(sizeof(s_reference) == 4, "actor reference layout");
static_assert(sizeof(s_type_5cfb45) == 0x6c, "prop state layout");
static_assert(sizeof(s_type_76cf92) == 0xc4, "prop layout");
static_assert(offsetof(s_type_76cf92, state) == 0x58, "prop state offset");
static_assert(sizeof(s_prop_datum) == 0x3c, "prop reference layout");
static_assert(offsetof(s_prop_datum, tracking_index) == 0x14,
    "prop tracking index offset");
static_assert(sizeof(s_type_e5ff81) == 0x124, "tracking layout");
static_assert(offsetof(s_type_e5ff81, state) == 4, "tracking state offset");
static_assert(offsetof(hash_node, data) == 0x0c,
    "hash node payload offset");
static_assert(offsetof(hash_table, data_size) == 0x28,
    "hash table data-size offset");

hash_node *function_13e2d0(
    hash_table *table, void *key);
bool hash_table_remove(hash_table *table, void *key);
bool function_13e270(
    hash_table *table, void *key, void const *data);

// @retail 0x25d690
s_type_5cfb45 *function_25d690(s_prop_datum *datum)
{
    if (g_470f10[datum->type].kind == 1)
        return &prop_get(datum->prop_index)->state;
    if (datum->tracking_index != NONE)
        return &tracking_get(datum->tracking_index)->state;
    return &prop_get(datum->prop_index)->state;
}

static bool actor_reference_equal(s_reference a, s_reference b)
{
    return *(dword *)&a == *(dword *)&b;
}

static bool actor_reference_find(
    hash_table *table, void *key, long *value)
{
    hash_node *node = function_13e2d0(table, key);
    if (!node || !value)
        return false;
    memcpy(value, node->data, table->data_size);
    return true;
}

static void actor_reference_replace(
    hash_table *table, void *key, long const *value)
{
    hash_node *node = function_13e2d0(table, key);
    if (node)
        memcpy(node->data, value, table->data_size);
}

// @retail 0x2628f0
void __stdcall function_2628f0(long actor_index, s_reference reference)
{
    s_actor_view *actor = actor_get(actor_index);
    hash_table *table =
        (hash_table *)g_557c6c;
    if (!actor_reference_equal(actor->unknown418, g_470fa0))
        hash_table_remove(table, *(void **)&actor->unknown418);
    if (!actor_reference_equal(reference, g_470fa0))
    {
        long previous = NONE;
        if (actor_reference_find(table, *(void **)&reference, &previous) &&
            previous != NONE)
        {
            actor_get(previous)->unknown418 = g_470fa0;
            actor_reference_replace(table, *(void **)&reference, &actor_index);
        }
        else
        {
            function_13e270(table, *(void **)&reference, &actor_index);
        }
    }
    actor->unknown418 = reference;
}

#endif
