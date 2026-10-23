#ifndef GLYPHS_STORAGE_H
#define GLYPHS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "font_render_cache.h"

struct s_bitmap_view;
union s_font_cursor_1f5c0
{
    dword pair;
    struct { short next, x; } fields;
};

struct s_glyphs_storage
{
    byte field_0;
    byte unknown_1[0x1];
    short field_2;
    s_font_cursor_1f5c0 field_4;
    short field_8;
    short field_a;
    s_bitmap_view * field_c;
    s_font_render_entry field_10[512];
};

extern s_glyphs_storage g_glyphs_storage;
#define g_4b62a0 (g_glyphs_storage.field_0)
#define g_4b62a2 (g_glyphs_storage.field_2)
#define g_4b62a4 (g_glyphs_storage.field_4)
#define g_4b62a8 (g_glyphs_storage.field_8)
#define g_4b62aa (g_glyphs_storage.field_a)
#define g_4b62ac (g_glyphs_storage.field_c)
#define g_4b62b0 (g_glyphs_storage.field_10)
typedef char assert_glyphs_offset_0[(offsetof(s_glyphs_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_glyphs_offset_2[(offsetof(s_glyphs_storage, field_2) == 0x2) ? 1 : -1];
typedef char assert_glyphs_offset_4[(offsetof(s_glyphs_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_glyphs_offset_8[(offsetof(s_glyphs_storage, field_8) == 0x8) ? 1 : -1];
typedef char assert_glyphs_offset_a[(offsetof(s_glyphs_storage, field_a) == 0xa) ? 1 : -1];
typedef char assert_glyphs_offset_c[(offsetof(s_glyphs_storage, field_c) == 0xc) ? 1 : -1];
typedef char assert_glyphs_offset_10[(offsetof(s_glyphs_storage, field_10) == 0x10) ? 1 : -1];

typedef char assert_glyphs_size[(sizeof(s_glyphs_storage) == 0x1010) ? 1 : -1];

#endif
