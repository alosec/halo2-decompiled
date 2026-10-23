#ifndef TARGET_CACHE_STORAGE_H
#define TARGET_CACHE_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"


struct s_3d4f0_entry
{
	long tag;
	long object_index;
	long flags;
	transform4x3f transforms[64];
};

/* the selected object, its model count and its models: one record, whose
   address escapes through the model array */
struct s_target_cache_storage
{
    long object_index;
    long model_count;
    s_3d4f0_entry field_0[4];
};

extern s_target_cache_storage g_target_cache_storage;
#define g_4c1bd0 (g_target_cache_storage.object_index)
#define g_4c1bd4 (g_target_cache_storage.model_count)
#define g_4c1bd8 (g_target_cache_storage.field_0)
typedef char assert_target_cache_offset_0[(offsetof(s_target_cache_storage, field_0) == 0x8) ? 1 : -1];

typedef char assert_target_cache_size[(sizeof(s_target_cache_storage) == 0x3438) ? 1 : -1];

#endif
