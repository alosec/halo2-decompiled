#ifndef COUNTERS_STORAGE_H
#define COUNTERS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0820f0.h"




struct s_counters_storage
{
    s_connection_counter field_0;
    s_connection_counter field_8;
};

extern s_counters_storage g_counters_storage;
#define g_485ab0 (g_counters_storage.field_0)
#define g_485ab8 (g_counters_storage.field_8)
typedef char assert_counters_offset_0[(offsetof(s_counters_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_counters_offset_8[(offsetof(s_counters_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_counters_size[(sizeof(s_counters_storage) == 0x10) ? 1 : -1];

#endif
