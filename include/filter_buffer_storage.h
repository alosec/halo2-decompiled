#ifndef FILTER_BUFFER_STORAGE_H
#define FILTER_BUFFER_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_filter_buffer_storage
{
    byte field_0[0x4c];
};

extern s_filter_buffer_storage g_filter_buffer_storage;
#define g_4b99b0 (g_filter_buffer_storage.field_0)
typedef char assert_filter_buffer_offset_0[(offsetof(s_filter_buffer_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_filter_buffer_size[(sizeof(s_filter_buffer_storage) == 0x4c) ? 1 : -1];

#endif
