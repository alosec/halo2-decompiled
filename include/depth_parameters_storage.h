#ifndef DEPTH_PARAMETERS_STORAGE_H
#define DEPTH_PARAMETERS_STORAGE_H

#include "unknown_11c920.h"

struct s_depth_parameters_13450
{
    long identifier, mode;
    real distance;
};

extern s_depth_parameters_13450 g_depth_parameters_storage;
#define g_4850c4 (g_depth_parameters_storage)
#define g_4850c8 (g_depth_parameters_storage.mode)

typedef char assert_depth_parameters_size[(sizeof(s_depth_parameters_13450) == 12) ? 1 : -1];

#endif
