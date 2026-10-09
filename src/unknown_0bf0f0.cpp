#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <stddef.h>

// @flags /O2 /Gr /arch:SSE

struct s_scenario_block
{
    long count;
    byte *elements;
};
static_assert(sizeof(s_scenario_block) == 8, "scenario block layout");

struct s_type_4f0dcc
{
    short palette_index;
    short name_index;
    dword flags;
    point3f position;
    vector3f rotation;
    real scale;
    byte unknown24[2];
    word bsp_mask;
    long unique_id;
    short origin_bsp_index;
    char type;
    char source;
    char bsp_policy;
    byte unknown31[3];
    long unknown34;
    long unknown38;
    dword field_3c[4];
    byte unknown4c[0x5a - 0x4c];
    word multiplayer_flags;
};
static_assert(sizeof(s_type_4f0dcc) == 0x5c, "scenario object datum layout");
static_assert(offsetof(s_type_4f0dcc, name_index) == 2, "scenario object name layout");

struct s_object_placement_data
{
    byte unknown00[4];
    long unique_id;
    long unknown08;
    long unknown0c;
    long unknown10;
    char bsp_policy;
    byte unknown15[3];
    dword flags;
    point3f position;
    vector3f forward;
    vector3f up;
    byte unknown40[0x58 - 0x40];
    real scale;
    byte unknown5c[0x74 - 0x5c];
    long unknown74;
    color3f colors[4];
};
static_assert(sizeof(s_object_placement_data) == 0xa8, "object placement data layout");

struct s_effect_object_placement;

extern long *g_4de2d0;
bool function_d5060(long type, long index, bool unknown,
    s_type_4f0dcc const *datum, s_scenario_block *palette, s_object_placement_data *data);
bool function_a7640(s_effect_object_placement *data);
long function_b7b40(void *data);
void function_108b10(long object_index, long data);
void function_bf050(long object_index, short name_index);
void __stdcall function_a7870(long object_index);

// @retail 0xbf0f0
void function_bf0f0(s_type_4f0dcc const *datum, long type, long index,
    s_scenario_block *palette, bool validate_effect, bool unknown)
{
    if (datum->palette_index == NONE)
        return;

    short name_index = datum->name_index;
    if (name_index != NONE && name_index >= 0 && name_index < 640 &&
        g_4de2d0[name_index] != NONE)
        return;

    s_object_placement_data placement;
    if (!function_d5060(type, index, unknown, datum, palette, &placement))
        return;
    if (validate_effect && !function_a7640((s_effect_object_placement *)&placement))
        return;

    long object_index = function_b7b40(&placement);
    if (object_index != NONE)
    {
        function_108b10(object_index, (long)datum);
        if (name_index != NONE)
            function_bf050(object_index, name_index);
        function_a7870(object_index);
    }
}
