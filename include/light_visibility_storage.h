#ifndef LIGHT_VISIBILITY_STORAGE_H
#define LIGHT_VISIBILITY_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "visibility_storage.h"




struct s_light_visibility_storage
{
    s_bit_vector_pool field_0;
};

extern s_light_visibility_storage g_light_visibility_storage;
#define g_54aa68 (g_light_visibility_storage.field_0)
typedef char assert_light_visibility_offset_0[(offsetof(s_light_visibility_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_light_visibility_size[(sizeof(s_light_visibility_storage) == 0x2ae0) ? 1 : -1];

#endif
