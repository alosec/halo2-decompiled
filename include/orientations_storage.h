#ifndef ORIENTATIONS_STORAGE_H
#define ORIENTATIONS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"


struct rigid_transform_scaled
{
    quaternionf rotation;
    point3f position;
    real scale;
};

struct s_orientations_storage
{
    rigid_transform_scaled field_0[255];
};

extern s_orientations_storage g_orientations_storage;
#define g_4dc2f0 (g_orientations_storage.field_0)
typedef char assert_orientations_offset_0[(offsetof(s_orientations_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_orientations_size[(sizeof(s_orientations_storage) == 0x1fe0) ? 1 : -1];

#endif
