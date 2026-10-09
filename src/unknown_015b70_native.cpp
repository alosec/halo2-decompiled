// Native source split of recovered palette-state rebuild at retail 0x15b70.
// Xbox binds the default palette to four D3D stages in unknown_0158f0.cpp;
// the native renderer carries texture state per request, so this split retains
// only the recovered guest palette bookkeeping and header construction.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

struct s_palette_source_015b70_native
{
    byte unknown00[0x1c];
    long tag_index;
    byte unknown20[0x24];
};

struct s_palette_group_015b70_native
{
    byte unknown00[8];
    long palette_count;
    dword palettes;
    byte unknown10[0x58];
};

struct s_palette_tag_data_015b70_native
{
    byte unknown00[0x80];
    long group_count;
    s_palette_group_015b70_native *groups;
};

struct s_d3d_palette_015b70_native
{
    dword Common;
    dword Data;
    dword Lock;
};

static_assert(sizeof(s_palette_source_015b70_native) == 0x44, "palette source size");
static_assert(sizeof(s_palette_group_015b70_native) == 0x68, "palette group size");
static_assert(offsetof(s_palette_tag_data_015b70_native, group_count) == 0x80,
              "palette tag count offset");
static_assert(sizeof(s_d3d_palette_015b70_native) == 0x0c, "D3D palette header size");

extern long g_484dc0[4];
extern s_d3d_palette_015b70_native g_484dd0[32];
extern long g_467000;
extern long g_5093ac;

void function_015b70(void)
{
    for (long stage = 0; stage < 4; ++stage)
        g_484dc0[stage] = NONE;

    s_palette_source_015b70_native *sources =
        (s_palette_source_015b70_native *)g_4e0350->sources;
    long tag_index = sources[g_4686c4].tag_index;
    g_467000 = NONE;
    g_5093ac = 0;
    if (tag_index == NONE)
        return;

    s_palette_tag_data_015b70_native *data =
        (s_palette_tag_data_015b70_native *)g_4e3b44[tag_index & 0xffff].bytes;
    long palette_total = 0;
    for (long group_index = 0; group_index < data->group_count; ++group_index)
    {
        s_palette_group_015b70_native *group = &data->groups[group_index];
        for (long index = 0; index < group->palette_count; ++index)
        {
            if (palette_total >= 32)
                continue;
            s_d3d_palette_015b70_native *palette = &g_484dd0[palette_total++];
            palette->Data = (group->palettes + index * 0x400) & 0x7fffffff;
            palette->Common = 0x30001;
            palette->Lock = 0;
        }
    }
    g_5093ac = palette_total;
}
