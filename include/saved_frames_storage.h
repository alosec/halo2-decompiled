#ifndef SAVED_FRAMES_STORAGE_H
#define SAVED_FRAMES_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"


struct s_frame_view_2c560
{
    short index, mode;
    word unknown04;
    byte flag06;
    bool scaled;
    short location;
    byte unknown0a[2];
    point3f position;
    byte view[0x74];
    byte projection[0xc0];
    byte records[0x120];
    byte unknown26c[0x2c];
};

struct s_saved_frames_storage
{
    s_frame_view_2c560 field_0[2];
};

extern s_saved_frames_storage g_saved_frames_storage;
#define g_4850d0 (g_saved_frames_storage.field_0)
typedef char assert_saved_frames_offset_0[(offsetof(s_saved_frames_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_saved_frames_size[(sizeof(s_saved_frames_storage) == 0x530) ? 1 : -1];

#endif
