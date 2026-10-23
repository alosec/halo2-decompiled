#ifndef PARTITION_COLLIDING_STORAGE_H
#define PARTITION_COLLIDING_STORAGE_H

#include <stddef.h>
#include "unknown_0b68c0.h"




typedef s_cluster_partition s_partition_colliding_storage;

extern s_partition_colliding_storage g_partition_colliding_storage;
#define g_4de2e0 (g_partition_colliding_storage.cluster_first_data_references)
#define g_4de2e4 (g_partition_colliding_storage.data_references)
#define g_4de2e8 (g_partition_colliding_storage.cluster_references)
typedef char assert_partition_colliding_offset_0[(offsetof(s_partition_colliding_storage, cluster_first_data_references) == 0x0) ? 1 : -1];
typedef char assert_partition_colliding_offset_4[(offsetof(s_partition_colliding_storage, data_references) == 0x4) ? 1 : -1];
typedef char assert_partition_colliding_offset_8[(offsetof(s_partition_colliding_storage, cluster_references) == 0x8) ? 1 : -1];

typedef char assert_partition_colliding_size[(sizeof(s_partition_colliding_storage) == 0xc) ? 1 : -1];

#endif
