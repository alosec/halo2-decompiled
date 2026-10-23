#ifndef CLUSTER_MASK_STORAGE_H
#define CLUSTER_MASK_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_cluster_mask_storage
{
    dword field_0[16];
};

extern s_cluster_mask_storage g_cluster_mask_storage;
// Preserve the existing array access view without extending retail storage.
#define g_4c56c0 (*(dword (*)[64])g_cluster_mask_storage.field_0)
typedef char assert_cluster_mask_offset_0[(offsetof(s_cluster_mask_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_cluster_mask_size[(sizeof(s_cluster_mask_storage) == 0x40) ? 1 : -1];

#endif
