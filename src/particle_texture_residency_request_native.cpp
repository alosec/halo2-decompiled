// Source-local native reconstruction of texture residency requests.
// The Xbox body remains in unknown_01cf50.cpp.

#include "unknown_11c920.h"
#include "globals.h"

struct s_bitmap_view;
struct s_type_7ba8e9;
struct D3DTexture;

extern byte *g_485a80;
extern long g_485af4[4];
extern long g_485b04[4];
extern void *g_51f3c8[2][4];

s_type_7ba8e9 *function_137550(long tag, short bitmap);
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority);

// @retail 0x14390
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority)
{
    if (bitmap)
    {
        g_51f3c8[1][stage] = function_12310((s_bitmap_view *)bitmap, priority);
        g_485af4[stage] = *(short *)((byte *)bitmap + 4);
        g_485b04[stage] = *(short *)((byte *)bitmap + 6);
    }
    return true;
}

// @retail 0x144f0
void function_144f0(long tag, short stage, short fallback,
    short fallback_index, short index, real priority)
{
    s_type_7ba8e9 *bitmap = NULL;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        long count = *(long *)(group + 0x44);
        if (count > 0)
            bitmap = function_137550(tag, (short)(index % count));
    }
    if (!bitmap || *(short *)((byte *)bitmap + 0x0a) != fallback)
    {
        tag = *(long *)(g_485a80 + fallback * 8 + 0x64);
        if (tag == NONE)
            return;
        bitmap = function_137550(tag, fallback_index);
    }
    if (bitmap)
        function_14390(stage, bitmap, priority);
}

// @retail 0x143c0
bool function_143c0(long tag, short index, short stage, real priority)
{
    bool result = false;
    if (tag == NONE)
    {
        tag = *(long *)(g_485a80 + 0x64);
        index = 0;
        if (tag == NONE)
            return result;
    }
    byte *group = g_4e3b44[tag & 0xffff].bytes;
    __builtin_prefetch(*(byte **)(group + 0x48) + index * 0x74, 0, 3);
    __builtin_prefetch(g_485af4, 0, 3);
    if (index < 0 || index >= *(long *)(group + 0x44))
    {
        tag = *(long *)(g_485a80 + 0x64);
        index = 0;
        group = g_4e3b44[tag & 0xffff].bytes;
    }
    if (group && *(long *)(group + 0x44) > 0)
    {
        s_type_7ba8e9 *bitmap = function_137550(tag, index);
        if (bitmap)
        {
            g_51f3c8[1][stage] = function_12310((s_bitmap_view *)bitmap, priority);
            g_485af4[stage] = *(short *)((byte *)bitmap + 4);
            g_485b04[stage] = *(short *)((byte *)bitmap + 6);
            result = true;
        }
    }
    return result;
}
