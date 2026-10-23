#ifndef FILTER_KERNEL_STORAGE_H
#define FILTER_KERNEL_STORAGE_H
#include <stddef.h>
#include "unknown_11c920.h"

/* 0x4b9c74-0x4b9da0: the filter's five 14-real rows and the settings that follow
   them, one object (0x27aa0 fills it in one pass) */
struct s_filter_kernel_storage
{
    real field_0[5][14];
    long field_118;
    real field_11c;
    real field_120;
    real field_124;
    bool field_128;
};

extern s_filter_kernel_storage g_filter_kernel_storage;
#define g_4b9c74 (g_filter_kernel_storage.field_0)
#define g_4b9d8c (g_filter_kernel_storage.field_118)
#define g_4b9d90 (g_filter_kernel_storage.field_11c)
#define g_4b9d94 (g_filter_kernel_storage.field_120)
#define g_4b9d98 (g_filter_kernel_storage.field_124)
#define g_4b9d9c (g_filter_kernel_storage.field_128)
typedef char assert_filter_kernel_offset_118[(offsetof(s_filter_kernel_storage, field_118) == 0x118) ? 1 : -1];
typedef char assert_filter_kernel_offset_128[(offsetof(s_filter_kernel_storage, field_128) == 0x128) ? 1 : -1];
typedef char assert_filter_kernel_size[(sizeof(s_filter_kernel_storage) == 0x12c) ? 1 : -1];
#endif
