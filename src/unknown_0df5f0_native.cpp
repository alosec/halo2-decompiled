// Native source split of recovered biped center/height/radius query at retail
// 0xdf5f0. The matching Xbox definition remains in bipeds.cpp, whose broader
// physics translation unit uses x86 inline assembly.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

struct s_biped_0df5f0_native
{
    long definition_index;
    byte unknown004[0x2c4 - 4];
    real crouch;
};

struct s_biped_header_0df5f0_native
{
    byte unknown00[8];
    s_biped_0df5f0_native *biped;
};

static_assert(offsetof(s_biped_0df5f0_native, crouch) == 0x2c4, "biped crouch offset");
static_assert(offsetof(s_biped_header_0df5f0_native, biped) == 8, "biped header pointer offset");

point3f *function_b9dd0(long object_index, point3f *result);

void function_df5f0(long object_index, point3f *center, real *height, real *radius)
{
    s_biped_0df5f0_native *biped =
        ((s_biped_header_0df5f0_native *)g_4e0300->data)[object_index & 0xffff].biped;
    byte *definition = g_4e3b44[biped->definition_index & 0xffff].bytes;

    function_b9dd0(object_index, center);
    if (!(*(dword *)(definition + 0x264) & 1))
        center->z += *(real *)(definition + 0x270);

    if ((*(dword *)(definition + 0x264) >> 1) & 1)
    {
        *height = 0.0f;
        *radius = *(real *)(definition + 0x270);
    }
    else
    {
        *height = (*(real *)(definition + 0x26c) - *(real *)(definition + 0x268)) * biped->crouch +
            *(real *)(definition + 0x268) - *(real *)(definition + 0x270) * 2.0f;
        *radius = *(real *)(definition + 0x270);
    }
}
