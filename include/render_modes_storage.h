#ifndef RENDER_MODES_STORAGE_H
#define RENDER_MODES_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"


struct s_render_mode_44370
{
    long target;
    dword clear_flags, field_8_7;
    real clear_depth;
    byte clear_stencil, unknown11[7];
    short cull_mode, unknown1a;
    long group, unknown20;
};

struct s_render_modes_storage
{
    union
    {
        byte data[0x384];
        struct { s_render_mode_44370 value[25]; } field_0;
    };
};

extern s_render_modes_storage g_render_modes_storage;
#define g_43fa1c (g_render_modes_storage.field_0.value)
typedef char assert_render_modes_offset_0[(offsetof(s_render_modes_storage, field_0.value) == 0x0) ? 1 : -1];

typedef char assert_render_modes_size[(sizeof(s_render_modes_storage) == 0x384) ? 1 : -1];

#endif
