#ifndef LIGHT_DEFINITION_STORAGE_H
#define LIGHT_DEFINITION_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_light_definition_storage
{
    byte field_0[0xb8];
};

extern s_light_definition_storage g_light_definition_storage;
#define g_47fda0 (g_light_definition_storage.field_0)
typedef char assert_light_definition_offset_0[(offsetof(s_light_definition_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_light_definition_size[(sizeof(s_light_definition_storage) == 0xb8) ? 1 : -1];

#endif
