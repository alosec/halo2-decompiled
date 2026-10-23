#ifndef PALETTE_CACHE_STORAGE_H
#define PALETTE_CACHE_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "xtl.h"




struct s_palette_cache_storage
{
    D3DPalette field_0[32];
};

extern s_palette_cache_storage g_palette_cache_storage;
#define g_484dd0 (g_palette_cache_storage.field_0)
typedef char assert_palette_cache_offset_0[(offsetof(s_palette_cache_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_palette_cache_size[(sizeof(s_palette_cache_storage) == 0x180) ? 1 : -1];

#endif
