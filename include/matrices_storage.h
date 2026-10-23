#ifndef MATRICES_STORAGE_H
#define MATRICES_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_0259d0.h"




struct s_matrices_storage
{
    transform4x3f field_0[255];
};

extern s_matrices_storage g_matrices_storage;
#define g_4d8f20 (g_matrices_storage.field_0)
typedef char assert_matrices_offset_0[(offsetof(s_matrices_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_matrices_size[(sizeof(s_matrices_storage) == 0x33cc) ? 1 : -1];

#endif
