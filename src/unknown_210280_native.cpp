// Native source split for the recovered BSP object/output table rebuild. The
// matching Xbox definitions remain in their original translation units, whose
// unrelated paths contain x86-only assembly and intrinsics.

#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"
#include <stddef.h>
#include <string.h>

struct s_scenario_identifier_210280
{
    long unique_id;
    short origin_bsp;
    char type;
    char source;
};

struct s_scenario_type_210280
{
    byte unknown00[0x0a];
    short block_offset;
    short palette_offset;
    short element_size;
};

struct s_scenario_block_210280
{
    long count;
    byte *elements;
};

struct s_pair_item_210280
{
    short value;
    byte byte2;
    byte byte3;
};

struct s_pair_entry_210280
{
    dword flags;
    long lower;
    long upper;
    long range_count;
    void *ranges;
    long item_count;
    s_pair_item_210280 *items;
};

struct s_pair_table_210280
{
    long count;
    void *flags;
    byte unknown08[0x28];
    long entry_count;
    s_pair_entry_210280 *entries;
};

struct s_pair_globals_210280
{
    byte unknown00[0xc4];
    long count;
    s_pair_table_210280 *table;
};

struct s_pair_object_210280
{
    byte unknown00[0xaa];
    byte type;
    byte unknownab[0x130 - 0xab];
    short value130;
    byte unknown132[0x1d4 - 0x132];
    short value1d4;
};

struct s_output_entry
{
    long object_index;
    short unknown04;
    short index;
    byte unknown08[2];
    byte byte0a;
    byte byte0b;
    byte unknown0c[4];
};

struct s_object_header_210280
{
    byte unknown00[8];
    byte *object;
};

struct s_2960e0_counts_210280
{
    byte unknown00[0x50];
    long first_count;
    byte unknown54[0xa8 - 0x54];
    long second_count;
};

static_assert(sizeof(s_scenario_identifier_210280) == 0x08,
    "scenario identifier size");
static_assert(sizeof(s_scenario_type_210280) == 0x10,
    "scenario type size");
static_assert(sizeof(s_pair_item_210280) == 0x04, "pair item size");
static_assert(sizeof(s_pair_entry_210280) == 0x1c, "pair entry size");
static_assert(offsetof(s_pair_table_210280, entry_count) == 0x30,
    "pair table entry count offset");
static_assert(offsetof(s_pair_table_210280, entries) == 0x34,
    "pair table entries offset");
static_assert(offsetof(s_pair_globals_210280, count) == 0xc4,
    "pair globals count offset");
static_assert(offsetof(s_pair_globals_210280, table) == 0xc8,
    "pair globals table offset");
static_assert(offsetof(s_pair_object_210280, value130) == 0x130,
    "pair object first value offset");
static_assert(offsetof(s_pair_object_210280, value1d4) == 0x1d4,
    "pair object second value offset");
static_assert(sizeof(s_output_entry) == 0x10,
    "pair output size");
static_assert(sizeof(s_object_header_210280) == 0x0c,
    "object header size");
static_assert(offsetof(s_2960e0_counts_210280, first_count) == 0x50,
    "first object census count offset");
static_assert(offsetof(s_2960e0_counts_210280, second_count) == 0xa8,
    "second object census count offset");

extern s_output_entry *g_4f93a0;

// @retail 0xb7a40
static void *scenario_identifier_lookup_210280(
    s_scenario_identifier_210280 const *identifier, long *index_out)
{
    char source = identifier->source;
    if (source == 1 || source == 0)
    {
        char type = identifier->type;
        s_scenario_type_210280 *definition =
            (s_scenario_type_210280 *)g_468630[type];
        if (definition->block_offset != NONE)
        {
            s_scenario_block_210280 *block = (s_scenario_block_210280 *)
                ((byte *)g_4e0350 + definition->block_offset);
            byte *element = block->elements;
            for (long i = 0; i < block->count; ++i, element += definition->element_size)
            {
                s_scenario_identifier_210280 *current =
                    (s_scenario_identifier_210280 *)(element + 0x28);
                bool match = (current->unique_id == identifier->unique_id) &
                    (current->type == type) & (current->source == source);
                if (match && current->source == 0)
                    match &= current->origin_bsp == identifier->origin_bsp;
                if (match)
                {
                    if (index_out)
                        *index_out = i;
                    return element;
                }
            }
        }
    }
    return NULL;
}

// @retail 0x2960e0
static long object_output_census_210280(s_2960e0_counts_210280 *counts,
    long *objects, long maximum_count)
{
    memset(objects, 0xff, maximum_count * sizeof(long));
    s_type_f1af8e iterator;
    iterator.signature = 0x86868686;
    iterator.type_mask = 0x40;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    s_object *object;
    while ((object = function_baeb0(&iterator)) != NULL)
    {
        long index = NONE;
        if (scenario_identifier_lookup_210280(
                (s_scenario_identifier_210280 const *)((byte *)object + 0xa4), &index))
        {
            short slot = (short)index;
            if (slot >= maximum_count)
                break;
            objects[slot] = iterator.object_index;
        }
    }
    iterator.signature = 0x86868686;
    iterator.type_mask = 0x80;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    while ((object = function_baeb0(&iterator)) != NULL)
    {
        long index = NONE;
        if (scenario_identifier_lookup_210280(
                (s_scenario_identifier_210280 const *)((byte *)object + 0xa4), &index))
        {
            short slot = (short)(counts->first_count + (short)index);
            if (slot >= maximum_count)
                break;
            objects[slot] = iterator.object_index;
        }
    }
    short extra_count = *(short *)((byte *)g_4e0348 + 0x140);
    for (short i = 0; i < extra_count; ++i)
    {
        short slot = (short)((word)(counts->first_count + counts->second_count) + i);
        if (slot >= maximum_count)
            break;
        objects[slot] = NONE;
    }
    return counts->first_count + counts->second_count + extra_count;
}

// @retail 0x2101d0
static bool pair_table_insert_object_210280(s_pair_table_210280 *table,
    long object_index)
{
    byte *object_pointer = ((s_object_header_210280 *)g_4e0300->data)
        [object_index & 0xffff].object;
    s_pair_object_210280 *object = (s_pair_object_210280 *)object_pointer;
    short value = NONE;
    if (object->type == 6)
        value = object->value130;
    else if (object->type == 7)
        value = object->value1d4;
    if (value >= 0 && value < table->entry_count)
    {
        s_pair_entry_210280 *entry = &table->entries[value];
        if ((entry->flags & 1) && entry->item_count > 0)
        {
            for (short i = 0; i < entry->item_count; ++i)
            {
                s_pair_item_210280 *item = &entry->items[i];
                s_output_entry *output = &g_4f93a0[item->value];
                output->index = i;
                output->object_index = object_index;
                output->byte0a = item->byte2;
                output->byte0b = item->byte3;
            }
        }
    }
    return false;
}

// @retail 0x210280
void function_210280(void)
{
    long objects[2000];
    long count = object_output_census_210280(
        (s_2960e0_counts_210280 *)g_4e0350, objects, 2000);
    s_pair_globals_210280 *globals = (s_pair_globals_210280 *)g_4e0348;
    s_pair_table_210280 *table = globals->count > 0 ? globals->table : NULL;
    for (long i = 0; i < 100; ++i)
        g_4f93a0[i].object_index = NONE;
    if (table)
    {
        for (short i = 0; i < count; ++i)
        {
            long object_index = objects[i];
            if (object_index != NONE)
                pair_table_insert_object_210280(table, object_index);
        }
    }
}
