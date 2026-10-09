#include "unknown_11c920.h"
#include "globals.h"

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

struct s_object_header_b8b70
{
    short identifier;
    byte flags;
    byte type;
    short cluster_index;
    byte unknown06[2];
    byte *object;
};

extern void *g_4de2e0;
extern void *g_4e0310;
extern s_record_pool *g_4e030c;

void function_b88e0(long object_index, s_object_partition_record_ab *record);
void function_1cadf0(s_cluster_partition *partition, long reference_index,
    long data_index, long payload_size, void const *payload);
void function_1cae40(s_cluster_partition *partition, long data_index,
    long *first_cluster_reference);

// @retail 0xb8b70
void function_b8b70(long object_index)
{
    s_object_header_b8b70 *header =
        &((s_object_header_b8b70 *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;
    if ((header->flags & 0x40) && ((*(dword *)(object + 4) >> 9) & 1))
    {
        s_object_partition_record_ab record;
        function_b88e0(object_index, &record);
        function_1cadf0((s_cluster_partition *)&g_4de2e0,
            *(long *)(object + 0x60), object_index, sizeof(record), &record);
    }
}

// @retail 0xc3220
void function_c3220(long datum_index)
{
    byte *entry = g_4e030c->data + (datum_index & 0xffff) * 0x110;
    if (entry[2] & 2)
    {
        function_1cae40((s_cluster_partition *)&g_4e0310,
            datum_index, (long *)(entry + 0x10));
        entry[2] &= ~8;
    }
}
