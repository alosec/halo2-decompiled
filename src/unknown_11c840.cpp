#include "unknown_11c920.h"
#include "globals.h"
// @flags /O2 /Gr

struct s_tag_iterator
{
    long unknown00;
    long unknown04;
    long datum_index;
    long next_index;
    long group_tag;
};

class c_part;
struct s_pair_element;
struct s_single_element;
struct s_part_table
{
    byte unknown00[0x20];
    long pair_a_count;
    s_pair_element *pairs_a;
    long single_count;
    s_single_element *singles;
    long pair_b_count;
    s_pair_element *pairs_b;
    void function_1c2390();
    c_part *function_1c2460(long index);
};

long function_122c70(s_tag_iterator *arg_0);
void function_2122b0(long arg_0);
void function_205550(void *arg_0);
void function_1ea9c0(long arg_0, long arg_1);

#ifdef H2_NATIVE_PORT
extern "C" long h2_native_cache_tag_count();
extern "C" void h2_native_physics_runtime_reset();
extern "C" void h2_native_physics_model_write_reset();
extern "C" int h2_native_physics_model_write_status();
extern "C" void h2_native_physics_model_initialize(long tag_index);

struct s_native_tag_instance_view
{
    long group_tag;
    long datum_index;
    void *address;
    long size;
};

static long g_native_initialized_tags;
static long g_native_deferred_tags;
static long g_native_physics_model_nonzero_mask;
static long g_native_physics_model_entry_count;
static long g_native_physics_model_planned_objects;
static long g_native_physics_model_planned_resolutions;
static bool g_native_physics_model_plan_valid;

struct s_shape_block_table;
void *function_1eaf30(short type, s_shape_block_table *table, short index);

static bool h2_native_physics_range(void const *address, unsigned long size)
{
    dword begin = (dword)address;
    dword const physical_begin = 0x80061000;
    dword const physical_end = physical_begin + 0x02000000;
    return begin >= physical_begin && begin <= physical_end &&
        size <= physical_end - begin;
}

static byte *h2_native_physics_block(byte *definition, long count_offset,
    long stride, long *count)
{
    *count = *(long *)(definition + count_offset);
    byte *address = *(byte **)(definition + count_offset + 4);
    if (*count < 0 || *count > 4096 ||
        (*count && (!address || !h2_native_physics_range(address,
            (unsigned long)*count * (unsigned long)stride))))
        return 0;
    return address;
}

static bool h2_native_physics_resolve(byte *definition, dword key)
{
    void *resolved = function_1eaf30((short)key,
        (s_shape_block_table *)definition, (short)(key >> 16));
    if (!resolved || !h2_native_physics_range(resolved, 4))
        return false;
    g_native_physics_model_planned_resolutions++;
    return true;
}

static bool h2_native_validate_physics_model(long tag_index)
{
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
    if (!definition || !h2_native_physics_range(definition, 0x110))
        return false;

    long count;
    byte *entries = h2_native_physics_block(definition, 0x38, 0x90, &count);
    if (count && !entries)
        return false;
    for (long i = 0; i < count; i++)
        if (!h2_native_physics_resolve(definition,
            *(dword *)(entries + i * 0x90 + 0x38)))
            return false;

    entries = h2_native_physics_block(definition, 0x58, 0x50, &count);
    if (count && !entries)
        return false;
    g_native_physics_model_planned_objects += count;

    entries = h2_native_physics_block(definition, 0x60, 0x90, &count);
    if (count && !entries)
        return false;
    g_native_physics_model_planned_objects += count * 2;

    entries = h2_native_physics_block(definition, 0x70, 0x100, &count);
    if (count && !entries)
        return false;
    long external_vertices = 0;
    long external_values = 0;
    byte *vertex_storage = *(byte **)(definition + 0x7c);
    byte *value_storage = *(byte **)(definition + 0x84);
    for (long i = 0; i < count; i++)
    {
        byte *entry = entries + i * 0x100;
        long vertex_count = *(long *)(entry + 0x54);
        long vertex_capacity = *(long *)(entry + 0x58) & 0x7fffffff;
        long value_count = *(long *)(entry + 0xf8);
        if (vertex_count < 0 || vertex_count > 4096 ||
            vertex_capacity < vertex_count || vertex_capacity > 4096 ||
            value_count < 0 || value_count > 4096)
            return false;
        if (vertex_capacity > 3)
        {
            if (vertex_count && (!vertex_storage || !h2_native_physics_range(
                vertex_storage + external_vertices * 0x30,
                (unsigned long)vertex_count * 0x30)))
                return false;
            external_vertices += vertex_count;
        }
        if (value_count && (!value_storage || !h2_native_physics_range(
            value_storage + external_values * 0x10,
            (unsigned long)value_count * 0x10)))
            return false;
        external_values += value_count;
    }
    g_native_physics_model_planned_objects += count;

    entries = h2_native_physics_block(definition, 0x90, 0x38, &count);
    if (count && !entries)
        return false;
    long external_children = 0;
    byte *child_storage = *(byte **)(definition + 0x9c);
    for (long i = 0; i < count; i++)
    {
        byte *entry = entries + i * 0x38;
        long child_count = *(long *)(entry + 0x10);
        long child_capacity = *(long *)(entry + 0x14) & 0x7fffffff;
        if (child_count < 0 || child_count > 4096 ||
            child_capacity < child_count || child_capacity > 4096)
            return false;
        byte *children = child_capacity > 4 ? child_storage : entry + 0x18;
        if (child_capacity > 4 && children)
            children += external_children * 8;
        if (child_count && (!children ||
            !h2_native_physics_range(children, (unsigned long)child_count * 8)))
            return false;
        for (long child = 0; child < child_count; child++)
            if (!h2_native_physics_resolve(definition,
                *(dword *)(children + child * 8)))
                return false;
        if (child_capacity > 4)
            external_children += child_count;
    }
    g_native_physics_model_planned_objects += count;

    entries = h2_native_physics_block(definition, 0x108, 0x20, &count);
    if (count && !entries)
        return false;
    for (long i = 0; i < count; i++)
        if (!h2_native_physics_resolve(definition,
            *(dword *)(entries + i * 0x20 + 0x0c)))
            return false;
    g_native_physics_model_planned_objects += count * 2;
    return true;
}

extern "C" int h2_native_physics_model_plan_status()
{
    if (!g_native_physics_model_plan_valid)
        return -1;
    return (g_native_physics_model_planned_resolutions << 16) |
        (g_native_physics_model_planned_objects & 0xffff);
}

extern "C" int h2_native_physics_model_census()
{
    return (g_native_physics_model_entry_count << 16) |
        (g_native_physics_model_nonzero_mask & 0xffff);
}

extern "C" int h2_native_tag_initialization_status()
{
    return (g_native_initialized_tags << 16) |
        (g_native_deferred_tags & 0xffff);
}

static void h2_native_census_physics_model(long tag_index)
{
    static short const count_offsets[] =
    {
        0x38, 0x48, 0x50, 0x58, 0x60,
        0x68, 0x70, 0x90, 0xa0, 0x108,
    };
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
    for (long i = 0; i < sizeof(count_offsets) / sizeof(count_offsets[0]); i++)
    {
        long count = *(long *)(definition + count_offsets[i]);
        if (count > 0)
        {
            g_native_physics_model_nonzero_mask |= 1 << i;
            g_native_physics_model_entry_count += count;
        }
    }
}

static long h2_native_next_tag(long *next_index)
{
    s_native_tag_instance_view *instances =
        (s_native_tag_instance_view *)g_4e3b44;
    while (*next_index < h2_native_cache_tag_count())
    {
        s_native_tag_instance_view *instance = &instances[(*next_index)++];
        if (instance->group_tag != NONE && instance->datum_index != NONE)
            return instance->datum_index;
    }
    return NONE;
}

static void h2_native_initialize_validated_physics_models()
{
    if (!g_native_physics_model_plan_valid ||
        g_native_deferred_tags != 32 ||
        g_native_physics_model_entry_count != 175 ||
        g_native_physics_model_nonzero_mask != 0x2d9 ||
        g_native_physics_model_planned_resolutions != 94 ||
        g_native_physics_model_planned_objects != 102)
        return;

    h2_native_physics_model_write_reset();
    long written_tags = 0;
    long next_index = 0;
    for (long tag_index = h2_native_next_tag(&next_index);
        tag_index != NONE; tag_index = h2_native_next_tag(&next_index))
    {
        if (*(dword *)&g_4e3b44[(short)tag_index] == 0x70686d6f)
        {
            h2_native_physics_model_initialize(tag_index);
            written_tags++;
        }
    }
    if (written_tags == 32 &&
        h2_native_physics_model_write_status() == ((94 << 16) | 102))
    {
        g_native_initialized_tags += written_tags;
        g_native_deferred_tags = 0;
    }
}
#endif

// @retail 0x11c840
void function_11c840()
{
#ifdef H2_NATIVE_PORT
    g_native_initialized_tags = 0;
    g_native_deferred_tags = 0;
    g_native_physics_model_nonzero_mask = 0;
    g_native_physics_model_entry_count = 0;
    g_native_physics_model_planned_objects = 0;
    g_native_physics_model_planned_resolutions = 0;
    g_native_physics_model_plan_valid = true;
    h2_native_physics_runtime_reset();
    long native_next_index = 0;
    long local_1 = h2_native_next_tag(&native_next_index);
#else
    s_tag_iterator local_0;
    local_0.next_index = 0;
    local_0.group_tag = NONE;
    long local_1 = function_122c70(&local_0);
#endif
    while (local_1 != NONE)
    {
        switch (*(dword *)&g_4e3b44[(short)local_1])
        {
        case 0x62697064:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0x264))->function_1c2390();
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x636f6c6c:
            function_2122b0(local_1);
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x63726561:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0xd4))->function_1c2390();
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        case 0x70686d6f:
#ifndef H2_NATIVE_PORT
            function_1ea9c0(local_1, 0);
#else
            h2_native_census_physics_model(local_1);
            if (g_native_physics_model_plan_valid &&
                !h2_native_validate_physics_model(local_1))
                g_native_physics_model_plan_valid = false;
            g_native_deferred_tags++;
#endif
            break;
        case 0x76656869:
            function_205550(g_4e3b44[local_1 & 0xffff].bytes + 0x2ac);
#ifdef H2_NATIVE_PORT
            g_native_initialized_tags++;
#endif
            break;
        }
#ifdef H2_NATIVE_PORT
        local_1 = h2_native_next_tag(&native_next_index);
#else
        local_1 = function_122c70(&local_0);
#endif
    }
#ifdef H2_NATIVE_PORT
    h2_native_initialize_validated_physics_models();
#endif
}
