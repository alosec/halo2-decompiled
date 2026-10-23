#ifndef LIGHT_FRAME_STORAGE_H
#define LIGHT_FRAME_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_light_frame_storage
{
    byte field_0[0x88];
};

extern s_light_frame_storage g_light_frame_storage;
#define g_47fd18 (g_light_frame_storage.field_0)
typedef char assert_light_frame_offset_0[(offsetof(s_light_frame_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_light_frame_size[(sizeof(s_light_frame_storage) == 0x88) ? 1 : -1];

#endif
