// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"
#include <xmmintrin.h>

struct s_object_link_owner
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};

struct s_object_link_iterator
{
    s_object_link_owner const *owner;
    long index;
};

struct s_object_cluster_link
{
    short salt;
    short unknown02;
    short cluster;
    short unknown06;
    long next;
};

struct s_object_b8a80
{
    long definition_index;
    dword flag0_8 : 9;
    dword flag9 : 1;
    dword flag10_31 : 22;
    byte unknown08[0x14 - 8];
    long parent_index;
    byte unknown18[0x60 - 0x18];
    long first_cluster_reference;
};

struct s_object_header_b8a80
{
    short identifier;
    byte flags;
    byte type;
    short unknown04;
    byte unknown06[2];
    s_object_b8a80 *object;
};

#define OBJECT_GET(index) (((s_object_header_b8a80 *)g_4e0300->data)[(index) & 0xffff].object)

// Starts an iteration over the clusters of an object's root parent and
// returns the first cluster, or NONE.
// @retail 0xb8a80
short function_b8a80(long object_index, s_object_link_iterator *iterator)
{
    long root = NONE;
    while (object_index != NONE)
    {
        root = object_index;
        object_index = OBJECT_GET(object_index)->parent_index;
    }
    s_object_b8a80 *object = OBJECT_GET(root);
    iterator->owner = TEST_FIELD_BIT(object->flag9)
        ? (s_object_link_owner const *)&g_partition_colliding_storage
        : (s_object_link_owner const *)&g_partition_noncolliding_storage;
    iterator->index = object->first_cluster_reference;
    short result;
    s_record_pool *pool = iterator->owner->cluster_references;
    if (iterator->index != NONE)
    {
        s_object_cluster_link *reference = (s_object_cluster_link *)(pool->data + (iterator->index & 0xffff) * pool->size);
        long next = reference->next;
        if (next != NONE)
            _mm_prefetch((char *)(pool->data + (next & 0xffff) * pool->size), _MM_HINT_T0);
        iterator->index = next;
        result = reference->cluster;
    }
    else
    {
        result = NONE;
    }
    return result;
}
