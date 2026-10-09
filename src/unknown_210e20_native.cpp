// Native source split of recovered structure-node bitmap refresh at retail
// 0x210e20. The matching Xbox definition remains in unknown_20fe20.cpp,
// whose unrelated audio/geometry helpers use x86 inline assembly.

#include "unknown_11c920.h"
#include "data_array.h"
#include "object_iterator.h"
#include <stddef.h>

struct s_match_node_210e20_native
{
    byte unknown00[0x18];
    byte flags;
    byte unknown19[0x24 - 0x19];
};

struct s_match_nodes_210e20_native
{
    byte unknown00[0x5c];
    long count;
    s_match_node_210e20_native *nodes;
};

static_assert(sizeof(s_match_node_210e20_native) == 0x24, "match node size");
static_assert(offsetof(s_match_nodes_210e20_native, count) == 0x5c, "match node count offset");
static_assert(offsetof(s_match_nodes_210e20_native, nodes) == 0x60, "match node pointer offset");

extern void *g_4e0348;
extern void *g_4f93a4;

void function_210e20(void)
{
    s_match_nodes_210e20_native *globals = (s_match_nodes_210e20_native *)g_4e0348;
    for (long index = 0; index < globals->count; index++)
    {
        if (globals->nodes[index].flags & 8)
            ((dword *)g_4f93a4)[index >> 5] &= ~(1u << (index & 0x1f));
    }
}

struct s_object_values_210f00_native
{
    byte unknown000[0x1da];
    short value1da;
    short value1dc;
};

struct s_mapping_source_210f00_native
{
    long identifier;
    short index;
    byte type;
    byte source;
    short value1da;
    short value1dc;
};

struct s_mapping_block_210f00_native
{
    long count;
    s_mapping_source_210f00_native *entries;
};

struct s_mapping_globals_210f00_native
{
    byte unknown000[0x220];
    s_mapping_block_210f00_native *mappings;
};

struct s_mapped_object_210f00_native
{
    byte unknown000[0xa4];
    long identifier;
    short index;
    byte type;
    byte source;
    byte unknown0ac[0x1da - 0xac];
    short value1da;
    short value1dc;
};

static_assert(sizeof(s_mapping_source_210f00_native) == 0x0c, "mapping source size");
static_assert(offsetof(s_mapping_globals_210f00_native, mappings) == 0x220,
              "mapping block offset");
static_assert(offsetof(s_mapped_object_210f00_native, value1da) == 0x1da,
              "mapped object value offset");

extern s_record_pool *g_4e0300;

static byte *object_from_index_210f00_native(long object_index)
{
    return *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
}

static void mapping_values_reset_210f00_native(void)
{
    s_type_f1af8e iterator;
    function_bae80(&iterator, 0x80, 0);
    while (function_baeb0(&iterator))
    {
        s_object_values_210f00_native *object =
            (s_object_values_210f00_native *)object_from_index_210f00_native(iterator.object_index);
        object->value1da = NONE;
        object->value1dc = 0;
    }
}

void function_210f00(void)
{
    mapping_values_reset_210f00_native();
    s_mapping_block_210f00_native *mappings =
        ((s_mapping_globals_210f00_native *)g_4e0348)->mappings;
    for (long index = 0; index < mappings->count; ++index)
    {
        s_mapping_source_210f00_native *source = &mappings->entries[index];
        s_type_f1af8e iterator;
        function_bae80(&iterator, 0x80, 0);
        while (function_baeb0(&iterator))
        {
            s_mapped_object_210f00_native *object =
                (s_mapped_object_210f00_native *)object_from_index_210f00_native(iterator.object_index);
            bool matches = source->identifier == object->identifier &&
                source->source == object->source && source->type == object->type;
            if (matches && source->source == 0)
                matches = source->index == object->index;
            if (matches)
            {
                object->value1da = source->value1da;
                object->value1dc = source->value1dc;
                break;
            }
        }
    }
}
