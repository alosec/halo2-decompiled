// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"

struct s_render_object_header
{
    byte unknown00[8];
    long *object;
};

struct s_object_flags_view_4bcc0
{
    dword : 19;
    dword flag19 : 1;
    dword : 12;
};

struct s_object_word_flags_view_4bcc0
{
    word : 2;
    word flag2 : 1;
    word : 13;
};

struct s_geometry_block_info;
#include "target_cache_storage.h"
extern long g_4ba01c;
extern bool g_4ba019;
extern byte *g_485a80;
bool object_or_parent_hidden(long object_index);
short function_3d3e0(long object_index, bool cached);
bool function_12de70(s_geometry_block_info *info, dword flags);
long function_4c450(long object_index, real distance, real const *definition, real *fraction);
bool __stdcall function_bab40(long object_index, long name, real *value);
void function_4c0d0(long object_index, byte *output, long mode);

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

// @retail 0x4bcc0
short function_4bcc0(byte *output, long object_index, real distance, long override,
    bool *cached, bool force, signed char *level)
{
    short result = 0;
    *(dword *)(output + 0x16c) = 0;
    bool eligible = false;
    bool current = false;
    if (!object_or_parent_hidden(object_index))
    {
        eligible = true;
        current = object_index == g_4c1bd0;
    }
    if (cached) *cached = current;
    if (eligible)
    {
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        if (force) *cached = false;
        long model_index = *(long *)(definition + 0x38);
        if (model_index != NONE)
        {
            byte *model = g_4e3b44[model_index & 0xffff].bytes;
            result = function_3d3e0(object_index, *cached);
            if (result > 0)
            {
                if (!override)
                {
                    long geometry = *(long *)(model + 4);
                    if (geometry != NONE)
                    {
                        byte *geometry_definition = g_4e3b44[geometry & 0xffff].bytes;
                        if (*(long *)(geometry_definition + 0x74) > 0)
                        {
                            byte *blocks = *(byte **)(geometry_definition + 0x78);
                            if (function_12de70((s_geometry_block_info *)(blocks + 0x34), 3))
                                *(dword *)(output + 0x16c) |= 0x40;
                        }
                    }
                }
                *level = *cached ? 4 : -1;
                if (!*cached)
                {
                    bool fixed_level = TEST_FIELD_BIT(((s_object_flags_view_4bcc0 const *)(object + 4))->flag19);
                    byte *object_definition = g_4e3b44[*(long *)object & 0xffff].bytes;
                    byte *level_definition = g_4e3b44[*(long *)(object_definition + 0x38) & 0xffff].bytes;
                    if (g_4ba019) distance *= 0.3f;
                    if (!fixed_level)
                    {
                        if (TEST_FIELD_BIT(((s_object_word_flags_view_4bcc0 const *)(object + 0x10a))->flag2)) distance *= 1.7f;
                        real fraction;
                        *level = (signed char)function_4c450(object_index, distance, (real const *)(level_definition + 0x28), &fraction);
                        long maximum = (*(dword *)(output + 0x16c) & 0x40) ? 4 : g_4ba01c;
                        *level = (signed char)PIN(*level, 0, maximum);
                    }
                    else
                        *level = 5;
                }
                if (*level != -1)
                {
                    bool transparent = false;
                    if (*(dword *)(model + 0x9c) & 1)
                    {
                        *(dword *)(output + 0x16c) |= 6;
                        *(long *)(output + 0x174) = *(long *)(g_485a80 + 0xd0);
                        *(real *)(output + 0x184) = 0.99f;
                        transparent = true;
                    }
                    else
                    {
                        byte *current_object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
                        if (((1 << current_object[0xaa]) & 3) && *(real *)(current_object + 0x2b0) > 0.0f)
                        {
                            *(dword *)(output + 0x16c) |= 2;
                            *(long *)(output + 0x174) = *(long *)(g_485a80 + 0xd0);
                            *(real *)(output + 0x184) = *(real *)(current_object + 0x2b0);
                            transparent = true;
                        }
                    }
                    if (transparent)
                    {
                        bool flag2 = (bool)((*(dword *)(model + 0x9c) >> 2) & 1);
                        bool flag1 = (bool)((*(dword *)(model + 0x9c) >> 1) & 1);
                        if (*cached) { flag2 = true; flag1 = false; }
                        if (flag2) *(dword *)(output + 0x16c) |= 8;
                        else *(dword *)(output + 0x16c) &= ~8;
                        if (flag1) *(dword *)(output + 0x16c) |= 0x10;
                        else *(dword *)(output + 0x16c) &= ~0x10;
                    }
                    if (!override)
                    {
                        if (*(real *)(object + 0xf4) > 0.0f && *(long *)(model + 0x60) > 0)
                        {
                            byte *materials = *(byte **)(model + 0x64);
                            long tag = *cached ? *(long *)(materials + 0x80) : *(long *)(materials + 0x88);
                            if (tag != NONE)
                            {
                                *(dword *)(output + 0x16c) |= 1;
                                *(long *)(output + 0x170) = tag;
                                *(real *)(output + 0x180) = *(real *)(object + 0xf0);
                            }
                        }
                        if (*(real *)(object + 0xf0) > 1.0f && *(long *)(model + 0x60) > 0)
                        {
                            byte *materials = *(byte **)(model + 0x64);
                            long tag = *cached ? *(long *)(materials + 0xec) : *(long *)(materials + 0xf4);
                            if (tag != NONE)
                            {
                                *(dword *)(output + 0x16c) |= 0x20;
                                *(long *)(output + 0x178) = tag;
                                *(real *)(output + 0x188) = 1.0f - *(real *)(object + 0xf0);
                            }
                        }
                        if (*(long *)(model + 0xf4) != NONE)
                        {
                            real value = 0.0f;
                            if (function_bab40(object_index, *(long *)(model + 0xf8), &value) && value > 0.0f)
                            {
                                *(dword *)(output + 0x16c) |= 0x80;
                                *(long *)(output + 0x17c) = *(long *)(model + 0xf4);
                                *(real *)(output + 0x18c) = value;
                            }
                        }
                    }
                }
                else
                    result = 0;
            }
            function_4c0d0(object_index, output, result);
        }
    }
    return result;
}
