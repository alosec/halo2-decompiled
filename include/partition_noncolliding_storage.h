#ifndef PARTITION_NONCOLLIDING_STORAGE_H
#define PARTITION_NONCOLLIDING_STORAGE_H

#include <stddef.h>
#include "unknown_0b68c0.h"




typedef s_cluster_partition s_partition_noncolliding_storage;

extern s_partition_noncolliding_storage g_partition_noncolliding_storage;
#define g_4de2d4 (g_partition_noncolliding_storage.cluster_first_data_references)
#define g_4de2d8 (g_partition_noncolliding_storage.data_references)
#define g_4de2dc (g_partition_noncolliding_storage.cluster_references)
typedef char assert_partition_noncolliding_offset_0[(offsetof(s_partition_noncolliding_storage, cluster_first_data_references) == 0x0) ? 1 : -1];
typedef char assert_partition_noncolliding_offset_4[(offsetof(s_partition_noncolliding_storage, data_references) == 0x4) ? 1 : -1];
typedef char assert_partition_noncolliding_offset_8[(offsetof(s_partition_noncolliding_storage, cluster_references) == 0x8) ? 1 : -1];

typedef char assert_partition_noncolliding_size[(sizeof(s_partition_noncolliding_storage) == 0xc) ? 1 : -1];

#endif
