// Source-local native reconstruction of the retail particle vertex descriptor
// compiler and its compact format helpers. This owns descriptor construction,
// not the mutable cache lifetime or D3D shader-input submission.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>
#include <string.h>

struct s_1c330_output
{
    long stream;
    long offset;
    long format;
    byte flag_c;
    byte flag_d;
    short field_0e;
};

static_assert(sizeof(s_1c330_output) == 0x10,
    "shader descriptor size");

struct s_native_shader_binding
{
    long field_00;
    long tag;
    long index;
    long field_0c;
};

struct s_native_shader_stream
{
    long field_00;
    long field_04;
    long field_08;
};

struct s_native_shader_cache
{
    s_native_shader_binding bindings[8];
    long active;
    long field_84;
    bool field_88;
    byte field_89[3];
    byte const *wanted[16];
    byte const *current[16];
    byte field_10c[0x100];
    bool descriptors_changed;
    byte field_20d[3];
    s_native_shader_stream streams[16];
    long stream_count;
    bool streams_changed;
    byte field_2d5[3];
};

static_assert(sizeof(s_native_shader_cache) == 0x2d8,
    "shader cache size");
static_assert(offsetof(s_native_shader_cache, field_10c) == 0x10c,
    "shader input descriptor offset");
static_assert(offsetof(s_native_shader_cache, streams) == 0x210,
    "shader stream offset");

extern "C" void __stdcall D3DDevice_SetVertexShaderInputDirect(
    void const *formats, long stream_count, void const *streams);
extern "C" void __stdcall D3DDevice_SelectVertexShaderDirect(
    void const *formats, long shader);

// @retail 0x35790
long function_35790(long format)
{
    // The valid retail cases form four regular four-entry bands followed by
    // the two packed terminal formats. This branch form avoids a compiler
    // switch table becoming a false provider in the extracted ARM object.
    if (format <= 3)
        return ((format + 1) << 4) | 2;
    if (format <= 7)
        return ((format - 3) << 4) | 4;
    if (format <= 11)
        return ((format - 7) << 4) | 5;
    if (format <= 15)
        return ((format - 11) << 4) | 1;
    return format == 16 ? 0x16 : 0x40;
}

// @retail 0x35850
long function_35850(long format)
{
    if (format <= 3)
        return (format + 1) * 4;
    if (format <= 6)
        return format - 3;
    if (format == 7 || format >= 16)
        return 4;
    return ((format & 3) + 1) * 2;
}

// @retail 0x1c330
void function_1c330(byte *context, s_1c330_output *output)
{
    memset(output, 0, 16 * sizeof(*output));
    for (long i = 0; i < 16; ++i)
        output[i].format = 2;

    long mapping[21];
    memset(mapping, 0xff, sizeof(mapping));
    byte *selection = context + *(long *)(context + 0x80) * 16;
    long tag = *(long *)(selection + 4);
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        byte *entry = *(byte **)(definition + 8) +
            *(long *)(selection + 8) * 0x1c;
        dword count = *(dword *)(entry + 4);
        word *indices = *(word **)(entry + 8);
        for (dword i = 0; i < count; ++i)
            mapping[indices[i]] = i;
    }
    else
    {
        mapping[0] = 0;
        mapping[3] = 3;
        mapping[14] = 9;
    }

    long stream_count = *(long *)(context + 0x2d0);
    long limit = stream_count > 0 ? stream_count : 16;
    for (long stream = 0; stream < limit; ++stream)
    {
        signed char *source = *(signed char **)(context + 0x8c + stream * 4);
        if (!source)
            continue;

        long offset = 0;
        for (dword element = 0; element < 10; ++element)
        {
            signed char *pair = source + 1 + element * 2;
            if (pair[0] >= 0)
            {
                long target = mapping[(byte)pair[0]];
                if (target >= 0)
                {
                    s_1c330_output *entry = &output[target];
                    entry->stream = stream;
                    entry->offset = offset;
                    entry->format = function_35790((byte)pair[1]);
                    entry->flag_c = false;
                    entry->flag_d = false;
                }
                offset += (byte)function_35850((byte)pair[1]);
            }
            else if (pair[0] == -2)
                offset += pair[1];
            else
                break;
        }
        *(signed char **)(context + 0xcc + stream * 4) = source;
        *(signed char **)(context + 0x8c + stream * 4) = NULL;
        context[0x20c] = 0;
    }
}


// @retail 0x1c710
void __stdcall function_1c710(void *memory)
{
    s_native_shader_cache *state = (s_native_shader_cache *)memory;
    bool changed = false;
    if (state->descriptors_changed)
        function_1c330((byte *)state,
            (s_1c330_output *)state->field_10c);
    if (state->descriptors_changed || state->streams_changed)
    {
        D3DDevice_SetVertexShaderInputDirect(
            state->stream_count > 0 ? state->field_10c : NULL,
            state->stream_count,
            state->stream_count > 0 ? state->streams : NULL);
        state->streams_changed = false;
        changed = true;
    }
    if (state->field_88 || state->descriptors_changed)
    {
        changed = true;
        D3DDevice_SelectVertexShaderDirect(state->field_10c,
            state->field_84);
        state->field_88 = false;
    }
    state->stream_count = 0;
    if (changed)
        state->descriptors_changed = false;
}
