#ifndef SAMPLE_MASK_STORAGE_H
#define SAMPLE_MASK_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_sample_mask_storage
{
    dword field_0[8];
};

extern s_sample_mask_storage g_sample_mask_storage;
#define g_55eeb4 (g_sample_mask_storage.field_0)
typedef char assert_sample_mask_offset_0[(offsetof(s_sample_mask_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_sample_mask_size[(sizeof(s_sample_mask_storage) == 0x20) ? 1 : -1];

#endif
