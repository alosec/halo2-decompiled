#ifndef SHADER_STORAGE_H
#define SHADER_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_shader_storage
{
    byte field_0[0x2d8];
};

extern s_shader_storage g_shader_storage;
#define g_51f0f0 (g_shader_storage.field_0)
typedef char assert_shader_offset_0[(offsetof(s_shader_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_shader_size[(sizeof(s_shader_storage) == 0x2d8) ? 1 : -1];

#endif
