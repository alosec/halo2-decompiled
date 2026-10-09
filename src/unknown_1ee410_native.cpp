#ifdef H2_NATIVE_PORT

#include "unknown_11c920.h"
#include "globals.h"

#include <stddef.h>

extern short g_54e898;

struct s_material_lookup_owner
{
    byte unknown00[0x5c];
    s_tag_block_entry *entries;
};

struct s_material_tag_instance
{
    byte unknown00[8];
    s_material_lookup_owner *owner;
    byte unknown0c[4];
};

struct s_lookup_source
{
    byte unknown00[0x18];
    short index;
    short pad1a;
    long tag_index;
};

static_assert(sizeof(s_tag_block_entry) == 0x14,
    "material entry stride must match retail");
static_assert(offsetof(s_tag_block_entry, value08) == 8,
    "match-material value offset must match retail");
static_assert(offsetof(s_tag_block_entry, value10) == 0x10,
    "tag-material value offset must match retail");
static_assert(sizeof(s_material_tag_instance) == 0x10,
    "material tag-instance stride must match retail");
static_assert(offsetof(s_material_lookup_owner, entries) == 0x5c,
    "material tag-block entries offset must match retail");
static_assert(offsetof(s_lookup_source, index) == 0x18,
    "material lookup index offset must match retail");
static_assert(offsetof(s_lookup_source, tag_index) == 0x1c,
    "material lookup tag offset must match retail");

// @retail 0x1ee410
void function_1ee410(s_lookup_source const *source, short *result)
{
    if (source->tag_index != NONE)
    {
        s_material_tag_instance *tags =
            (s_material_tag_instance *)g_4e3b44;
        *result = tags[(word)source->tag_index].owner->entries[source->index].value10;
    }
    else if (source->index != NONE)
    {
        *result = g_4e0348->entries[source->index].value08;
    }
    else
    {
        *result = g_54e898;
    }
}

#endif
