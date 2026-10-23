#ifndef DEPTH_GROUPS_STORAGE_H
#define DEPTH_GROUPS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* One depth group: its nearest depth, a position, its screen bounds and its
   type. */
struct s_42760_entry
{
    real depth;
    point3f position;
    byte unknown10[0x10];
    long type;
};

/* The 32 groups and the number in use, which follows them directly. */
struct s_depth_groups_storage
{
    s_42760_entry entries[32];
    long count;
};

extern s_depth_groups_storage g_depth_groups_storage;
#define g_4c152c (g_depth_groups_storage.entries)
#define g_4c19ac (g_depth_groups_storage.count)
typedef char assert_depth_group_size[(sizeof(s_42760_entry) == 0x24) ? 1 : -1];
typedef char assert_depth_groups_count[(offsetof(s_depth_groups_storage, count) == 0x480) ? 1 : -1];

#endif
