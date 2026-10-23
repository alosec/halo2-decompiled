#ifndef MATERIAL_CONSTANTS_STORAGE_H
#define MATERIAL_CONSTANTS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_material_constants_storage
{
    real field_0[3][4];
};

extern s_material_constants_storage g_material_constants_storage;
#define g_4c1b38 (g_material_constants_storage.field_0)
typedef char assert_material_constants_offset_0[(offsetof(s_material_constants_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_material_constants_size[(sizeof(s_material_constants_storage) == 0x30) ? 1 : -1];

#endif
