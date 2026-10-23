#ifndef RENDER_STATE_STORAGE_H
#define RENDER_STATE_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "visibility_slot.h"




struct s_render_state_storage
{
    byte field_0;
    byte field_1;
    byte unknown_2[0x2];
    s_visibility_storage field_4;
    long field_40a8;
    long field_40ac[4];
    long field_40bc;
};

extern s_render_state_storage g_render_state_storage;
#define g_51f408 (g_render_state_storage.field_0)
#define g_51f409 (g_render_state_storage.field_1)
#define g_51f40c (g_render_state_storage.field_4)
#define g_5234b0 (g_render_state_storage.field_40a8)
#define g_5234b4 (g_render_state_storage.field_40ac)
#define g_5234c4 (g_render_state_storage.field_40bc)
typedef char assert_render_state_offset_0[(offsetof(s_render_state_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_render_state_offset_1[(offsetof(s_render_state_storage, field_1) == 0x1) ? 1 : -1];
typedef char assert_render_state_offset_4[(offsetof(s_render_state_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_render_state_offset_40a8[(offsetof(s_render_state_storage, field_40a8) == 0x40a8) ? 1 : -1];
typedef char assert_render_state_offset_40ac[(offsetof(s_render_state_storage, field_40ac) == 0x40ac) ? 1 : -1];
typedef char assert_render_state_offset_40bc[(offsetof(s_render_state_storage, field_40bc) == 0x40bc) ? 1 : -1];

typedef char assert_render_state_size[(sizeof(s_render_state_storage) == 0x40c0) ? 1 : -1];

#endif
