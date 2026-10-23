#ifndef FONT_LOADING_STORAGE_H
#define FONT_LOADING_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "font_loading.h"




struct s_font_loading_storage
{
    s_font_cache_entry field_0[k_maximum_font_count];
};

extern s_font_loading_storage g_font_loading_storage;
#define g_4e2920 (g_font_loading_storage.field_0)
typedef char assert_font_loading_offset_0[(offsetof(s_font_loading_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_font_loading_size[(sizeof(s_font_loading_storage) == 0x1220) ? 1 : -1];

#endif
