#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_146a20.h"
#include <math.h>

#ifdef H2_NATIVE_PORT

extern bool g_4b9ee9;
extern long g_4b9eec;
extern point3f g_4b9da0;
extern transform4x3f *g_4687d0;

void function_146de0(void);
void function_146b80(void);
int __fastcall function_142a60(transform4x3f const *a,
    transform4x3f const *b, transform4x3f *result);

struct s_native_cluster_tag_entry_3ebd0
{
    dword unknown00;
    long tag;
};

struct s_native_cluster_tag_table_3ebd0
{
    byte unknown00[8];
    long count;
    s_native_cluster_tag_entry_3ebd0 *entries;
};

static_assert(sizeof(s_native_cluster_tag_entry_3ebd0) == 8,
    "cluster tag entry layout changed");
static_assert(sizeof(s_native_cluster_tag_table_3ebd0) == 0x10,
    "cluster tag table layout changed");
static_assert(sizeof(transform4x3f) == 0x34,
    "transform layout changed");

// @retail 0x3ebd0
bool __stdcall function_3ebd0(vector3f const *offset,
    transform4x3f const *matrices, transform4x3f *out, long count)
{
    bool result = false;
    bool restore = g_47989c != 0;
    if (restore)
        function_146de0();

    if (g_4b9ee9 && g_4b9eec != NONE)
    {
        s_native_cluster_tag_table_3ebd0 *table =
            (s_native_cluster_tag_table_3ebd0 *)g_4e0350;
        long index = NONE;
        if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
            index = table->entries[(short)g_4b9eec].tag;

        byte *definition = 0;
        if (index != NONE)
            definition = g_4e3b44[index & 0xffff].bytes;

        real heading = *(real *)(definition + 0x80);
        vector3f forward;
        forward.i = (real)cos(heading);
        forward.j = (real)sin(heading);
        forward.k = 0.0f;
        vector3f left;
        left.i = forward.k * g_4687b0->j - forward.j * g_4687b0->k;
        left.j = forward.i * g_4687b0->k - forward.k * g_4687b0->i;
        left.k = forward.j * g_4687b0->i - forward.i * g_4687b0->j;

        transform4x3f transform = *g_4687d0;
        transform.forward = forward;
        transform.left = left;
        transform.up = *g_4687b0;
        transform.scale = *(real *)(definition + 0x14);
        transform.position.x = *(real *)(definition + 0x18) * g_4b9da0.x;
        transform.position.y = *(real *)(definition + 0x18) * g_4b9da0.y;
        transform.position.z = *(real *)(definition + 0x18) * g_4b9da0.z;

        transform4x3f applied;
        for (long component = 0; component < 13; ++component)
            ((volatile real *)&applied)[component] =
                ((real const *)&transform)[component];
        for (long i = 0; i < count; ++i)
        {
            out[i] = matrices[i];
            out[i].position.x += offset->i;
            out[i].position.y += offset->j;
            out[i].position.z += offset->k;
            function_142a60(&applied, &out[i], &out[i]);
        }
        result = true;
    }

    if (restore)
        function_146b80();
    return result;
}

#endif
