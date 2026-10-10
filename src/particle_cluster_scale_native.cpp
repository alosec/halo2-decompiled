// Native source split of recovered particle cluster scale lookup. The matching
// Xbox definition remains in unknown_0350e0.cpp, whose unrelated renderer
// helpers require Xbox D3D declarations.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

extern bool g_4b9ee9;
extern long g_4b9eec;

struct s_cluster_tag_entry
{
    dword unknown00;
    long tag;
};

struct s_cluster_tag_table
{
    byte unknown00[8];
    long count;
    s_cluster_tag_entry *entries;
};

static_assert(sizeof(s_cluster_tag_entry) == 8, "cluster tag entry layout");
static_assert(offsetof(s_cluster_tag_table, count) == 8, "cluster tag count offset");
static_assert(offsetof(s_cluster_tag_table, entries) == 0xc, "cluster tag entries offset");
static_assert(sizeof(s_cluster_tag_table) == 0x10, "cluster tag table layout");

// @retail 0x3eb70
real function_3eb70(void)
{
    real result = 1.0f;
    if (g_4b9ee9 && g_4b9eec != NONE)
    {
        long index = NONE;
        s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
        if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
            index = table->entries[(short)g_4b9eec].tag;
        byte *data = 0;
        if (index != NONE)
            data = g_4e3b44[index & 0xffff].bytes;
        result = *(real *)(data + 0x14);
    }
    return result;
}
