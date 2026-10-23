#ifndef HANDLES_STORAGE_H
#define HANDLES_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_handles_storage
{
    dword field_0[1024];
};

extern s_handles_storage g_handles_storage;
#define g_4c8798 (g_handles_storage.field_0)
typedef char assert_handles_offset_0[(offsetof(s_handles_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_handles_size[(sizeof(s_handles_storage) == 0x1000) ? 1 : -1];

#endif
