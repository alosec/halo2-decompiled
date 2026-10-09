// Native source split of the recovered motion-state reset used during a
// structure-BSP switch at retail 0x36ab0. The matching Xbox definitions remain
// in unknown_0350e0.cpp, whose unrelated renderer paths use x86-only code.

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <stddef.h>

struct s_motion_state_groups_36ab0_native
{
    long count;
    byte *entries;
    long large_count;
    byte *large_entries;
};

struct s_motion_material_entry_36ab0_native
{
    byte unknown00[0x24];
    long state_index;
    byte unknown28[0x88 - 0x28];
};

struct s_motion_material_map_36ab0_native
{
    byte unknown00[0x84];
    long count;
    s_motion_material_entry_36ab0_native *entries;
};

static_assert(sizeof(s_motion_state_groups_36ab0_native) == 0x10,
    "motion state groups size");
static_assert(offsetof(s_motion_state_groups_36ab0_native, large_count) == 0x08,
    "large motion state count offset");
static_assert(sizeof(s_motion_material_entry_36ab0_native) == 0x88,
    "motion material entry size");
static_assert(offsetof(s_motion_material_entry_36ab0_native, state_index) == 0x24,
    "motion material state offset");
static_assert(offsetof(s_motion_material_map_36ab0_native, count) == 0x84,
    "motion material count offset");
static_assert(offsetof(s_motion_material_map_36ab0_native, entries) == 0x88,
    "motion material entries offset");

extern real g_4670fc;
extern real g_467100;
extern real g_50941c;
extern long g_509420;
extern long g_509424;
extern real g_509428;

static __forceinline real motion_random_signed_36ab0(s_random_globals *random)
{
    dword value = random_next(&random->seed);
    double fraction = (double)value * (1.0f / 65535.0f);
    return (real)(fraction + fraction - 1.0f);
}

static __forceinline double motion_random_unit_36ab0(s_random_globals *random)
{
    return (double)random_next(&random->seed) * (1.0f / 65535.0f);
}

// @retail 0x36f50
static void motion_large_state_reset_36ab0(byte *state)
{
    s_random_globals *random = g_4e7408;
    byte *entry = state + 0xc8;
    for (long i = 0; i < 4; ++i, entry += 0xb8)
    {
        for (long j = 0; j < 9; ++j)
            ((dword *)entry)[j] = 0;
        for (long j = 0; j < 9; ++j)
            ((dword *)(entry + 0x24))[j] = 0;
        for (long j = 0; j < 3; ++j)
        {
            ((real *)(entry + 0x3c))[j] = ((real *)entry)[j];
            ((real *)(entry + 0x48))[j] = ((real *)entry)[j];
        }
        *(real *)(entry + 0x80) = 1.0f;
        *(real *)(entry + 0x84) = 1.0f;
        *(real *)(entry + 0x78) = 0.0f;
        real const *a = (real const *)(state + 0x24);
        real const *b = (real const *)(state + 0x30);
        real const *c = (real const *)(state + 0x3c);
        *(real *)(entry + 0xac) = (real)sqrt((double)a[0] * a[0] +
            (double)a[1] * a[1] + (double)a[2] * a[2]);
        *(real *)(entry + 0xb0) = (real)sqrt((double)b[0] * b[0] +
            (double)b[1] * b[1] + (double)b[2] * b[2]);
        *(real *)(entry + 0xb4) = (real)sqrt((double)c[0] * c[0] +
            (double)c[1] * c[1] + (double)c[2] * c[2]);
        for (long j = 0; j < 9; ++j)
            *(real *)(entry + 0x88 + j * 4) = motion_random_signed_36ab0(random);
    }
}

// @retail 0x39a80
static bool motion_small_state_reset_36ab0(byte *state)
{
    *(real *)(state + 0x80) = 0.0f;
    state[0x86] = false;
    if (*(long *)(state + 0x64) > 0)
    {
        byte *group = *(byte **)(state + 0x68);
        s_random_globals *random = g_4e7408;
        for (long i = 0; i < *(long *)group; ++i)
        {
            byte *entry = *(byte **)(group + 4) + i * 0x14;
            real *motion = (real *)(*(byte **)(group + 0x0c) + i * 0x20);
            *(long *)(entry + 0x10) = NONE;
            if ((0.0001f > *(real *)(state + 0x1c) &&
                    0.0001f > *(real *)(state + 0x20)) ||
                *(real *)(state + 0x1c) > *(real *)(state + 0x20))
            {
                *(real *)(state + 0x1c) = 0.1f;
                *(real *)(state + 0x20) = 0.8f;
            }
            motion[6] = (real)((*(real *)(state + 0x20) -
                (double)*(real *)(state + 0x1c)) *
                motion_random_unit_36ab0(random) + *(real *)(state + 0x1c));
            motion[0] = (real)(motion_random_unit_36ab0(random) * *(real *)(state + 0x30));
            motion[1] = (real)(motion_random_unit_36ab0(random) * *(real *)(state + 0x34));
            motion[2] = (real)(motion_random_unit_36ab0(random) * *(real *)(state + 0x38));
            real magnitude = (real)sqrt((double)motion[0] * motion[0] +
                (double)motion[1] * motion[1] + (double)motion[2] * motion[2]);
            if (!(fabs(magnitude) < 0.0001f))
            {
                real inverse = 1.0f / magnitude;
                motion[0] *= inverse;
                motion[1] *= inverse;
                motion[2] *= inverse;
            }
            *(real *)(entry + 0x0c) = (real)(motion_random_unit_36ab0(random) *
                (*(real *)(state + 0x28) - (double)*(real *)(state + 0x24)) +
                *(real *)(state + 0x24));
            motion[7] = 0.0f;
            if (0.0001f > *(real *)(state + 0x08))
                *(real *)(state + 0x08) = 1.0f;
            if (0.0001f > *(real *)(state + 0x0c))
                *(real *)(state + 0x0c) = 1.0f;
            if (0.0001f > *(real *)(state + 0x10))
                *(real *)(state + 0x10) = 1.0f;
            double sample = motion_random_unit_36ab0(random);
            *(real *)entry = (real)((sample + sample - 1.0f) * *(real *)(state + 0x10));
            sample = motion_random_unit_36ab0(random);
            *(real *)(entry + 4) = (real)((sample + sample - 1.0f) * *(real *)(state + 0x08));
            sample = motion_random_unit_36ab0(random);
            *(real *)(entry + 8) = (real)((sample + sample - 1.0f) * *(real *)(state + 0x0c));
            sample = motion_random_unit_36ab0(random);
            motion[3] = (real)(sample + sample - 1.0f);
            sample = motion_random_unit_36ab0(random);
            motion[4] = (real)(sample + sample - 1.0f);
            sample = motion_random_unit_36ab0(random);
            motion[5] = (real)(sample + sample - 1.0f);
        }
        state[0x86] = true;
    }
    *(real *)(state + 0x88) = 1.0f;
    return true;
}

static void motion_state_groups_reset_36ab0(s_motion_state_groups_36ab0_native *state)
{
    for (long i = 0; i < state->large_count; ++i)
        motion_large_state_reset_36ab0(state->large_entries + i * 0x3a8);
    for (long i = 0; i < state->count; ++i)
        motion_small_state_reset_36ab0(state->entries + i * 0x8c);
}

void function_36ab0(void)
{
    s_motion_material_map_36ab0_native *map =
        (s_motion_material_map_36ab0_native *)g_4e0348;
    for (long i = 0; i < map->count; ++i)
    {
        s_motion_material_entry_36ab0_native *entry = &map->entries[i];
        if (entry->state_index != NONE)
        {
            motion_state_groups_reset_36ab0(
                (s_motion_state_groups_36ab0_native *)
                g_4e3b44[entry->state_index & 0xffff].bytes);
        }
    }
    g_4670fc = 1.0f;
    g_467100 = 1.0f;
    g_509420 = 0;
    g_509424 = 0;
    g_509428 = 0.0f;
    g_50941c = 0.0f;
}
