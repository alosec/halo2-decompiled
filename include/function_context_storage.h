#ifndef FUNCTION_CONTEXT_STORAGE_H
#define FUNCTION_CONTEXT_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_function_context_storage
{
    long field_0;
    void * field_4;
    byte unknown_8[0x70];
};

extern s_function_context_storage g_function_context_storage;
#define g_4e9268 (g_function_context_storage.field_0)
#define g_4e926c (g_function_context_storage.field_4)
typedef char assert_function_context_offset_0[(offsetof(s_function_context_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_function_context_offset_4[(offsetof(s_function_context_storage, field_4) == 0x4) ? 1 : -1];

typedef char assert_function_context_size[(sizeof(s_function_context_storage) == 0x78) ? 1 : -1];

#endif
