#ifndef PALETTE_STORAGE_H
#define PALETTE_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "xtl.h"




struct s_palette_storage
{
    D3DPalette * field_0;
};

extern s_palette_storage g_palette_storage;
#define g_484dbc (g_palette_storage.field_0)
typedef char assert_palette_offset_0[(offsetof(s_palette_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_palette_size[(sizeof(s_palette_storage) == 0x4) ? 1 : -1];

#endif
