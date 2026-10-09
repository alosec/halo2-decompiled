// Native source split of recovered object fade-list removal at retail
// 0x10ace0. The matching Xbox definition remains in unknown_10aca0.cpp,
// whose unrelated fade interpolation uses x87 inline assembly.

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>

struct s_object_10ace0_native
{
    byte unknown000[0xc0];
    word flags_c0;
};

struct s_object_header_10ace0_native
{
    byte unknown00[8];
    s_object_10ace0_native *object;
};

struct s_10a980_entry_native
{
    long name;
    byte unknown04[0x14 - 4];
};

struct s_10a980_view_native
{
    s_10a980_entry_native entries[32];
    long object_indices[32];
};

static_assert(offsetof(s_object_10ace0_native, flags_c0) == 0xc0, "object fade flags offset");
static_assert(offsetof(s_object_header_10ace0_native, object) == 8, "object header pointer offset");
static_assert(offsetof(s_10a980_view_native, object_indices) == 0x280, "fade-list object array offset");

extern s_record_pool *g_4e0300;
extern void *g_5107f4;

void function_10ace0(long object_index)
{
    if (object_index == NONE)
        return;

    s_object_10ace0_native *object =
        ((s_object_header_10ace0_native *)g_4e0300->data)[object_index & 0xffff].object;
    if (object->flags_c0 & (1u << 9))
    {
        s_10a980_view_native *globals = (s_10a980_view_native *)g_5107f4;
        for (long index = 0; index < 32; index++)
        {
            if (globals->object_indices[index] == object_index)
                globals->object_indices[index] = NONE;
        }
        object->flags_c0 &= ~(1u << 9);
    }
}
