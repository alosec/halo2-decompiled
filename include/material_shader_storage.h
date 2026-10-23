#ifndef MATERIAL_SHADER_STORAGE_H
#define MATERIAL_SHADER_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_material_shader_storage
{
    byte field_0[0x78];
};

extern s_material_shader_storage g_material_shader_storage;
#define g_4c1a90 (g_material_shader_storage.field_0)
typedef char assert_material_shader_offset_0[(offsetof(s_material_shader_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_material_shader_size[(sizeof(s_material_shader_storage) == 0x78) ? 1 : -1];

#endif
