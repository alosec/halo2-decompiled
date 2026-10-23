#ifndef OBJECT_MASKS_STORAGE_H
#define OBJECT_MASKS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_object_masks_storage
{
    bool field_0;
    byte unknown_1[0x3];
    long field_4;
    long field_8[0x800];
};

extern s_object_masks_storage g_object_masks_storage;
#define g_4de2f8 (g_object_masks_storage.field_0)
#define g_4de2fc (g_object_masks_storage.field_4)
#define g_4de300 (g_object_masks_storage.field_8)
typedef char assert_object_masks_offset_0[(offsetof(s_object_masks_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_object_masks_offset_4[(offsetof(s_object_masks_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_object_masks_offset_8[(offsetof(s_object_masks_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_object_masks_size[(sizeof(s_object_masks_storage) == 0x2008) ? 1 : -1];

#endif
