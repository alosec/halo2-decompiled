// Source-local native reconstruction of the compact recovered render-state
// replay and reset providers used by particle material setup. The matching
// Xbox definitions remain in unknown_020560.cpp.

#include "unknown_11c920.h"
#include <stddef.h>

extern "C" void __stdcall D3DDevice_SetRenderStateNotInline(long state, dword value);

struct s_packed_render_states
{
    byte field_00[0x14];
    byte *data;
};

struct s_render_state_source
{
    byte field_00[0x20];
    s_packed_render_states *states;
};

struct s_render_reset_slot
{
    long index;
    byte unknown04[12];
    real value;
    byte unknown14[0x2c];
};

struct s_render_reset_state
{
    long index;
    byte unknown04[8];
    long fields[5];
    s_packed_render_states *states;
    byte *block;
    byte unknown28[0x40 - 0x28];
    long index40;
    byte unknown44[0x1320 - 0x44];
    s_render_reset_slot slots[4];
};

static_assert(sizeof(s_packed_render_states) == 0x18,
    "packed render state layout");
static_assert(offsetof(s_render_state_source, states) == 0x20,
    "render state source layout");
static_assert(offsetof(s_render_reset_state, fields) == 0x0c,
    "render reset fields layout");
static_assert(offsetof(s_render_reset_state, states) == 0x20,
    "render reset state table layout");
static_assert(offsetof(s_render_reset_state, block) == 0x24,
    "render reset block layout");
static_assert(offsetof(s_render_reset_state, index40) == 0x40,
    "render reset secondary index layout");
static_assert(offsetof(s_render_reset_state, slots) == 0x1320,
    "render reset slot layout");
static_assert(sizeof(s_render_reset_slot) == 0x40,
    "render reset slot size");
static_assert(sizeof(s_render_reset_state) == 0x1420,
    "render reset state size");

// @retail 0x18900
void __stdcall function_18900(s_render_state_source const *source,
    word const *range)
{
    byte const *entry = source->states->data + (*range & 0x1ff) * 5;
    for (long i = 0; i < (*range >> 9); ++i, entry += 5)
        D3DDevice_SetRenderStateNotInline(entry[0], *(dword const *)(entry + 1));
}

// @retail 0x16b10
void function_16b10(s_render_reset_state *state)
{
    if (state->block)
        function_18900((s_render_state_source const *)state,
            (word const *)(state->block + 0x11c));
    for (long i = 0; i < 5; ++i)
        state->fields[i] = 0;
    state->states = NULL;
    state->block = NULL;
    state->index = -1;
    state->index40 = -1;
    for (long i = 0; i < 4; ++i)
    {
        state->slots[i].index = -1;
        state->slots[i].value = 3.0f;
    }
}
