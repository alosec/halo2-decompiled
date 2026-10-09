#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"

#include <string.h>

// @flags /O2 /Gr

struct s_object_partition_record_ab
{
    short type;
    word flags;
    point3f centre;
    real radius;
};

struct s_cluster_partition
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};

struct s_partition_location
{
    byte unknown00[4];
    short cluster_index;
};

struct s_object_header_b8600
{
    short identifier;
    byte flags;
    byte type;
    short cluster_index;
    byte unknown06[2];
    byte *object;
};

struct s_structure_bsp_globals_b8600
{
    byte unknown00[0x9c];
    long cluster_count;
};

extern void *g_4de2e0;
extern void *g_4de2d4;

void function_11bed0(s_location *location, point3f const *point);
void function_b88e0(long object_index, s_object_partition_record_ab *record);
void function_1cac60(dword const *bits, s_cluster_partition *partition,
    long data_index, long *first_cluster_reference, point3f const *point,
    real radius, s_partition_location const *location, long payload_size,
    void const *payload, bool *overflow);
void __stdcall function_bef30(long object_index, long remove, long add,
    long siblings, long own_flags);
bool function_b6d60(long object_index, dword const *clusters);
void function_b7290(long object_index);
bool function_a7670(long object_index);
void __stdcall function_b8540(long object_index);
void function_b7300(long object_index);
void function_146bf0(void);
void function_1c36f0(long object_index, long second_object_index);
void function_278f00(void);

// @retail 0xb8600
void __stdcall function_b8600(long object_index, long location_value)
{
    s_object_header_b8600 *header =
        &((s_object_header_b8600 *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;
    bool newly_located = header->cluster_index == NONE;

    s_location derived_location;
    s_location const *location = (s_location const *)location_value;
    if (!location)
    {
        function_11bed0(&derived_location, (point3f const *)(object + 0x40));
        location = &derived_location;
        if (location->cluster_index == NONE)
            function_11bed0(&derived_location, (point3f const *)(object + 0x64));
    }

    if (location->cluster_index != NONE)
    {
        *(long *)(object + 0x28) = location->leaf_index;
        *(long *)(object + 0x2c) = *(long const *)((byte const *)location + 4);
        header->cluster_index = location->cluster_index;
        *(dword *)(object + 4) &= ~0x00040000;
    }
    else
    {
        *(dword *)(object + 4) |= 0x00040000;
    }

    dword cluster_bits[12];
    dword const *bits = NULL;
    if ((*(dword *)(object + 4) >> 21) & 1)
    {
        long bytes = (((s_structure_bsp_globals_b8600 *)g_4e0348)->cluster_count + 31) / 32 * sizeof(dword);
        memset(cluster_bits, 0xff, bytes);
        bits = cluster_bits;
    }

    s_object_partition_record_ab record;
    function_b88e0(object_index, &record);
    s_cluster_partition *partition =
        ((*(dword *)(object + 4) >> 9) & 1)
            ? (s_cluster_partition *)&g_4de2e0
            : (s_cluster_partition *)&g_4de2d4;
    bool overflow = false;
    function_1cac60(bits, partition, object_index, (long *)(object + 0x60),
        (point3f const *)(object + 0x40), *(real *)(object + 0x4c),
        (s_partition_location const *)(object + 0x28), sizeof(record), &record, &overflow);

    *(dword *)(object + 4) |= 0x100;
    header->flags |= 0x40;
    function_bef30(object_index, false, true, false, false);

    if (function_b6d60(object_index, (dword const *)((byte *)g_4e6948 + 0x11b8)))
        function_b7290(object_index);
    else if (((*(dword *)(object + 4) >> 17) & 1) && !function_a7670(object_index))
        function_b8540(object_index);
    else
        function_b7300(object_index);

    if (newly_located && header->cluster_index != NONE)
    {
        function_146bf0();
        function_1c36f0(object_index, object_index);
        function_278f00();
        function_146bf0();
    }
}
