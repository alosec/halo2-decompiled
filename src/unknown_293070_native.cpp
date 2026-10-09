// Native source split of recovered scenario-flock construction and the active-
// BSP flock pass at retail 0x293070, 0x2930c0, and 0x292f10. The matching Xbox
// definitions remain in unknown_293070.cpp, whose unrelated flock updates use
// x86 inline assembly.

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>

struct s_flock_293070_native
{
    short salt;
    short definition_index;
    long first_object_index;
    short object_count;
    short unknown0a;
    bool unknown0c;
    byte unknown0d;
    bool active;
    byte unknown0f[0x12 - 0x0f];
    word nearby_player_mask;
    real position14[3];
    real value20;
    short unknown24;
    byte unknown26[0x28 - 0x26];
};

struct s_scenario_flock_293070_native
{
    short structure_bsp_index;
    byte unknown02[2];
    short trigger_volume_index;
    byte flags;
    byte unknown07;
    real boundary_distance;
    byte unknown0c[0x2c - 0x0c];
    long creature_definition_index;
    byte unknown30[0x6c - 0x30];
    real proximity_radius;
    byte unknown70[0x80 - 0x70];
    long name;
};

struct s_scenario_flocks_293070_native
{
    byte unknown000[0x350];
    long count;
    s_scenario_flock_293070_native *entries;
};

static_assert(sizeof(s_flock_293070_native) == 0x28, "flock datum size");
static_assert(offsetof(s_flock_293070_native, definition_index) == 0x02,
    "flock definition offset");
static_assert(offsetof(s_flock_293070_native, active) == 0x0e,
    "flock active offset");
static_assert(offsetof(s_flock_293070_native, unknown24) == 0x24,
    "flock tail offset");
static_assert(sizeof(s_scenario_flock_293070_native) == 0x84,
    "scenario flock size");
static_assert(offsetof(s_scenario_flock_293070_native, flags) == 0x06,
    "scenario flock flags offset");
static_assert(offsetof(s_scenario_flock_293070_native, creature_definition_index) == 0x2c,
    "scenario flock definition offset");
static_assert(offsetof(s_scenario_flocks_293070_native, count) == 0x350,
    "scenario flock count offset");
static_assert(offsetof(s_scenario_flocks_293070_native, entries) == 0x354,
    "scenario flock entries offset");

extern void *g_4e0350;
extern short g_4686c4;
extern s_record_pool *g_51ecb4;

long flock_new(short definition_index)
{
    long flock_index = record_pool_allocate(g_51ecb4);
    if (flock_index != NONE)
    {
        s_flock_293070_native *flock =
            &((s_flock_293070_native *)g_51ecb4->data)[flock_index & 0xffff];
        flock->first_object_index = NONE;
        flock->unknown0a = NONE;
        flock->object_count = 0;
        flock->definition_index = definition_index;
        flock->unknown24 = 0;
        flock->unknown0c = false;
        flock->active = true;
    }
    return flock_index;
}

bool function_2930c0(long definition_index)
{
    bool result = false;
    s_scenario_flocks_293070_native *scenario =
        (s_scenario_flocks_293070_native *)g_4e0350;
    if (definition_index >= 0 && definition_index < scenario->count)
    {
        s_scenario_flock_293070_native *definition =
            &scenario->entries[definition_index];
        if (definition->creature_definition_index != NONE)
        {
            long flock_index = flock_new((short)definition_index);
            if (flock_index != NONE)
            {
                if (definition->flags & 2)
                {
                    ((s_flock_293070_native *)g_51ecb4->data)
                        [flock_index & 0xffff].active = false;
                }
                result = true;
            }
        }
    }
    return result;
}

void function_292f10(void)
{
    s_scenario_flocks_293070_native *scenario =
        (s_scenario_flocks_293070_native *)g_4e0350;
    for (short i = 0; i < scenario->count; ++i)
    {
        if (scenario->entries[i].structure_bsp_index == g_4686c4)
            function_2930c0(i);
    }
}
