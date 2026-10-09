#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"
#include <stddef.h>

#if H2_NATIVE_PORT

struct s_scenario_identifier_ab;
struct s_scenery_placement
{
    byte unknown00[0x4c];
    short pathfinding_policy;
};
static_assert(offsetof(s_scenery_placement, pathfinding_policy) == 0x4c, "scenery placement policy layout");

struct s_scenery_definition_295e60
{
    byte unknown000[0xbc];
    short pathfinding_policy;
};
static_assert(offsetof(s_scenery_definition_295e60, pathfinding_policy) == 0xbc, "scenery definition policy layout");

struct s_object_partition_record_ab
{
    short type;
    word flags;
    point3f centre;
    real radius;
};
static_assert(sizeof(s_object_partition_record_ab) == 0x14, "object partition record layout");

struct s_cluster_partition
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};

struct s_object_header_295e60
{
    short identifier;
    byte flags;
    byte type;
    byte unknown04[4];
    byte *object;
};
static_assert(sizeof(s_object_header_295e60) == 0x0c, "object header layout");

extern void *g_4de2e0;
void *__stdcall function_b7a40(s_scenario_identifier_ab const *identifier, long *index_out);
short function_10a040(long definition_index, s_scenery_placement *placement);
void function_b88e0(long object_index, s_object_partition_record_ab *record);
void function_1cadf0(s_cluster_partition *partition, long reference_index,
    long data_index, long payload_size, void const *payload);

static void object_partition_refresh_295e60(long object_index, byte *object)
{
    s_object_header_295e60 *header =
        &((s_object_header_295e60 *)g_4e0300->data)[object_index & 0xffff];
    if ((header->flags & 0x40) && ((*(dword *)(object + 4) >> 9) & 1))
    {
        s_object_partition_record_ab record;
        function_b88e0(object_index, &record);
        function_1cadf0((s_cluster_partition *)&g_4de2e0,
            *(long *)(object + 0x60), object_index, sizeof(record), &record);
    }
}

static void apply_game_speed_295e60(byte *object, byte *placement, short bsp_index,
    long count_offset, long entries_offset, long output_offset, short policy)
{
    *(dword *)(object + 4) &= ~0x400000;
    *(short *)(object + output_offset) = NONE;

    if (!placement)
        return;

    if (policy == 0 || policy == 3)
    {
        *(dword *)(object + 4) |= 0x800000;
    }
    else if (policy == 1)
    {
        if ((long)bsp_index < *(long *)(placement + count_offset))
        {
            byte *entry = *(byte **)(placement + entries_offset) + (long)bsp_index * 4;
            short value = *(short *)(entry + 2);
            *(short *)(object + output_offset) = value;
            if (value != NONE)
                *(dword *)(object + 4) |= 0x400000;
        }
        *(dword *)(object + 4) |= 0x800000;
    }
}

// @retail 0x295e60
void function_295e60(long bsp_index_value)
{
    short bsp_index = (short)bsp_index_value;
    s_type_f1af8e iterator;

    function_bae80(&iterator, 0x40, 0);
    byte *object;
    while ((object = (byte *)function_baeb0(&iterator)) != NULL)
    {
        byte *placement = (byte *)function_b7a40(
            (s_scenario_identifier_ab const *)(object + 0xa4), NULL);
        short policy = placement ? function_10a040(*(long *)object, (s_scenery_placement *)placement) : 2;
        apply_game_speed_295e60(object, placement, bsp_index, 0x50, 0x54, 0x130, policy);
        object_partition_refresh_295e60(iterator.object_index, object);
    }

    function_bae80(&iterator, 0x80, 0);
    while ((object = (byte *)function_baeb0(&iterator)) != NULL)
    {
        byte *placement = (byte *)function_b7a40(
            (s_scenario_identifier_ab const *)(object + 0xa4), NULL);
        short policy = 3;
        long definition_index = *(long *)object;
        if (definition_index != NONE)
        {
            short value = *(short *)(g_4e3b44[definition_index & 0xffff].bytes + 0x130);
            if (value == 0)
                policy = 2;
            else if (value == 1)
                policy = 1;
            else if (value == 2)
                policy = 0;
        }
        apply_game_speed_295e60(object, placement, bsp_index, 0x40, 0x44, 0x1d4, policy);
        object_partition_refresh_295e60(iterator.object_index, object);
    }
}

// @retail 0x10a040
short function_10a040(long definition_index, s_scenery_placement *placement)
{
    if (definition_index == NONE)
        return 3;

    switch (placement->pathfinding_policy)
    {
    case 0:
        return ((s_scenery_definition_295e60 *)g_4e3b44[definition_index & 0xffff].bytes)->pathfinding_policy;
    case 1:
        return 2;
    case 2:
        return 0;
    case 3:
        return 1;
    default:
        return 3;
    }
}

#endif
