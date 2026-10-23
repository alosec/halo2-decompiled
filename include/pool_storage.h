#ifndef POOL_STORAGE_H
#define POOL_STORAGE_H
#include <stddef.h>
#include "unknown_11c920.h"
struct s_pool_storage
{
    union
    {
        dword initialized;
        struct { bool initialized_byte; byte unknown_1[3]; } legacy;
    } flags;
    dword base;
    long size;
    dword checksum;
};
extern s_pool_storage g_pool_storage;
#define g_5107fc (g_pool_storage.flags.legacy.initialized_byte)
#define g_510800_pool_base (g_pool_storage.base)
#define g_510804_pool_size (g_pool_storage.size)
#define g_510808_pool_checksum (g_pool_storage.checksum)
typedef char assert_pool_flag_offset[(offsetof(s_pool_storage, flags) == 0) ? 1 : -1];
typedef char assert_pool_base_offset[(offsetof(s_pool_storage, base) == 4) ? 1 : -1];
typedef char assert_pool_size_offset[(offsetof(s_pool_storage, size) == 8) ? 1 : -1];
typedef char assert_pool_checksum_offset[(offsetof(s_pool_storage, checksum) == 12) ? 1 : -1];
typedef char assert_pool_size[(sizeof(s_pool_storage) == 0x10) ? 1 : -1];
#endif
