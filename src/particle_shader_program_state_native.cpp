// Source-local native reconstruction of retail particle vertex-program lookup
// and cache selection. Immutable fallback microcode is recovered source data;
// actual GPU upload remains an explicit D3D boundary.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>
#include <string.h>

struct s_shader_entry
{
    long field_00;
    dword input_count;
    word *inputs;
    long byte_count;
    dword *program;
    byte field_14[8];
};

struct s_shader_tag
{
    long field_00;
    dword entry_count;
    s_shader_entry *entries;
};

struct s_shader_program
{
    dword const *program;
    long field_04;
    dword byte_count;
    long field_0c;
    real field_10;
};

struct s_shader_binding
{
    long field_00;
    long tag;
    long index;
    long field_0c;
};

struct s_shader_stream
{
    long field_00;
    long field_04;
    long field_08;
};

struct s_shader_cache
{
    s_shader_binding bindings[8];
    long active;
    long field_84;
    bool field_88;
    byte field_89[3];
    byte const *wanted[16];
    byte const *current[16];
    byte field_10c[0x100];
    bool descriptors_changed;
    byte field_20d[3];
    s_shader_stream streams[16];
    long stream_count;
    bool streams_changed;
    byte field_2d5[3];
};

static_assert(sizeof(s_shader_cache) == 0x2d8, "shader cache size");

extern "C" void __stdcall D3DDevice_LoadVertexShaderProgram(
    dword const *program, dword address);

dword const g_43f2b0[85] =
{
	0x00152078, 0x00000000, 0x0056e000, 0x7c2a1000, 0x2ca00000,
	0x00000000, 0x0056e0aa, 0x7c021000, 0x23a00000, 0x00000000,
	0x00570000, 0x8c2a1000, 0x2cb00000, 0x00000000, 0x0096e615,
	0x38aaf856, 0x9ca00000, 0x00000000, 0x0096e601, 0x39fefaae,
	0x93a00000, 0x00000000, 0x00970615, 0x38ab1856, 0xdcb00000,
	0x00000000, 0x00d6201b, 0x08363800, 0x20b08800, 0x00000000,
	0x00d6401b, 0x08365800, 0x20b04800, 0x00000000, 0x00d6601b,
	0x08367800, 0x20b02800, 0x00000000, 0x00d6801b, 0x08369800,
	0x20b01800, 0x00000000, 0x02575215, 0xa42b586e, 0x6ca0f81c,
	0x00000000, 0x065740ab, 0xa5575bff, 0x13a10000, 0x00000000,
	0x00576015, 0xb42b7800, 0x2cb00000, 0x00000000, 0x00770015,
	0xa40012fe, 0x3ca00000, 0x00000000, 0x007720ab, 0xa4001006,
	0x73a00000, 0x00000000, 0x00772015, 0xb40012fe, 0x7cb00000,
	0x00000000, 0x0041401a, 0xc4355800, 0x20b0e800, 0x00000000,
	0x0056a015, 0xa42ab800, 0x2090c848, 0x00000000, 0x0056c0bf,
	0xa42ad800, 0x20a0c850, 0x00000000, 0x0056c015, 0xb57ed800,
	0x20a0c858, 0x00000000, 0x0081601a, 0xc5fe286a, 0xf0b0e801,
};

s_shader_program g_4670ec[1] =
{
	{ g_43f2b0, NONE, 0x154, NONE, 1.0f }
};

// @retail 0x1c2e0
dword const *function_1c2e0(long tag, long index, long *count)
{
    dword const *result;
    if (tag != NONE)
    {
        s_shader_entry *entry =
            &((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entries[index];
        result = entry->program;
        if (count)
            *count = entry->byte_count >> 4;
    }
    else
    {
        result = g_4670ec[index].program;
        if (count)
            *count = g_4670ec[index].byte_count >> 4;
    }
    return result;
}

// @retail 0x1c590
void function_1c590(s_shader_cache *state, long tag, long index)
{
    long count;
    dword const *program = function_1c2e0(tag, index, &count);
    state->active = 0;
    if (state->bindings[0].tag != tag || state->bindings[0].index != index)
    {
        state->bindings[0].field_00 = 0;
        state->bindings[0].tag = tag;
        state->bindings[0].index = index;
        state->bindings[0].field_0c = 0;
        D3DDevice_LoadVertexShaderProgram(program, 0);
        state->field_84 = 0;
        state->field_88 = true;
        memset(state->current, 0, sizeof(state->current));
        state->descriptors_changed = true;
        for (long i = 0; i < 16; ++i)
        {
            state->streams[i].field_08 = 0;
            state->streams[i].field_04 = 0;
            state->streams[i].field_00 = 0;
        }
        state->streams_changed = true;
        state->stream_count = 0;
    }
}
