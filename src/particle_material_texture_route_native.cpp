// Source-local native reconstruction of particle material texture routing.
// The Xbox body remains in unknown_01cf50.cpp.

#include "unknown_11c920.h"
#include "globals.h"

struct IDirect3DBaseTexture8;
struct s_type_7ba8e9;
struct s_unknown_01dcc0
{
    byte unknown00[4];
    long sub_header[5];
    long elements[4][6];
    long element_count;
    byte unknown7c[4];
    long width;
    long height;
    void *data;
    byte unknown8c[8];
    bool flag94;
    byte flag95;
    byte unknown96[2];
};
static_assert(sizeof(s_unknown_01dcc0) == 0x98, "s_unknown_01dcc0");

extern byte *g_485a80;
extern long g_4858b8;
extern byte g_485a77;
extern byte g_55e6bd;
extern short g_4858c8;
extern long g_4e6978;
extern long g_4e697c;
extern long g_485a78[2];
extern byte *g_4858c4;
extern byte g_485a76;
extern byte g_485607;
extern IDirect3DBaseTexture8 *g_51f3c8[2][4];
extern s_unknown_01dcc0 g_4b4b58[39];

void function_14f60(short stage, short index);
bool function_143c0(long tag, short index, short stage, real priority);
s_type_7ba8e9 *function_137550(long tag, short bitmap);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
void function_144f0(long tag, short stage, short fallback, short fallback_index, short index, real priority);
bool function_01dd60(long index, long *width, long *height);
long function_25960(void);

static __forceinline void texture_surface_dimensions_native(long index, long *width, long *height)
{
    if (g_4b4b58[index].data && !g_4b4b58[index].flag95)
    {
        *width = g_4b4b58[index].width;
        *height = g_4b4b58[index].height;
    }
    else
    {
        *width = 0;
        *height = 0;
    }
}

// @retail 0x19530
void function_19530(long mode, byte *context, short stage, long *width, long *height)
{
again:
    *width = 1;
    *height = 1;
    if (mode == 1)
        function_143c0(*(long *)(g_485a80 + 0xc), 0, stage, 0.0f);
    else if (mode == 2)
        function_14f60(stage, 1);
    else if (mode == 3)
    {
        function_14f60(stage, 1);
        texture_surface_dimensions_native(1, width, height);
    }
    else if (mode == 4)
        g_51f3c8[1][stage] = NULL;
    else if (mode == 5)
    {
        function_14f60(stage, (short)g_4858b8);
        function_01dd60(g_4858b8, width, height);
    }
    else if (mode == 6)
    {
        function_14f60(stage, 3);
        if (g_4b4b58[3].data && !g_4b4b58[3].flag95)
        {
            *width = g_4b4b58[3].width;
            *height = g_4b4b58[3].height;
        }
        else
        {
            *width = 640;
            *height = 480;
        }
    }
    else if (mode == 8)
    {
        function_14f60(stage, 10);
        texture_surface_dimensions_native(10, width, height);
    }
    else if (mode == 9)
        function_143c0(*(long *)(g_485a80 + 4), 0, stage, 0.0f);
    else if (mode == 10)
    {
        if (g_4858c8 && g_4858c4)
        {
            if (*(dword *)g_4858c4 & 0x80000)
            {
                mode = 18;
                goto again;
            }
            if (*(long *)(g_4858c4 + 0xe0) != NONE && !g_485a77)
                function_14f60(stage, 13);
            else
                function_144f0(*(long *)(g_4858c4 + 0x7c), stage, 0, 1, 0, 0.0f);
        }
        else
        {
            if (!g_55e6bd)
                g_55e6bd = true;
            long tag = *(long *)(g_485a80 + 0x64);
            if (tag != NONE)
            {
                s_type_7ba8e9 *bitmap = function_137550(tag, 1);
                if (bitmap)
                    function_14390(stage, bitmap, 0.0f);
            }
        }
    }
    else if (mode == 12)
    {
        if (!g_4858c8)
            function_143c0(*(long *)(g_485a80 + 4), 0, stage, 0.0f);
        else
            function_144f0(*(long *)(g_4858c4 + 0x7c), stage, 0, 1, 0, 0.0f);
    }
    else if (mode == 13)
    {
        function_14f60(stage, 9);
        texture_surface_dimensions_native(9, width, height);
    }
    else if (mode == 14)
        function_143c0(*(long *)(g_485a80 + 0x14), 0, stage, 0.0f);
    else if (mode == 15)
        function_143c0(*(long *)(g_485a80 + 0x14), 1, stage, 0.0f);
    else if (mode == 16)
    {
        function_143c0(g_4e6978, (short)g_4e697c, stage, 0.0f);
        if (g_4e6978 != NONE)
        {
            byte *definition = g_4e3b44[g_4e6978 & 0xffff].bytes;
            long count = *(long *)(definition + 0x44);
            long clamped = g_4e697c < 0 ? 0 : g_4e697c > count - 1 ? count - 1 : g_4e697c;
            if (g_4e697c == clamped)
            {
                byte *bitmap = *(byte **)(definition + 0x48) + g_4e697c * 0x74;
                *width = *(short *)(bitmap + 4);
                *height = *(short *)(bitmap + 6);
            }
        }
    }
    else if (mode == 17)
    {
        function_14f60(stage, 19);
        texture_surface_dimensions_native(19, width, height);
    }
    else if (mode == 18)
    {
        if (!g_485607)
        {
            long surface = function_25960();
            if (surface != NONE)
            {
                function_14f60(stage, (short)surface);
                function_01dd60(surface, width, height);
                return;
            }
        }
        function_143c0(*(long *)(g_485a80 + 0x64), 0, stage, 0.0f);
    }
    else if (mode == 19)
    {
        function_14f60(stage, 21);
        texture_surface_dimensions_native(21, width, height);
    }
    else if (mode == 20)
    {
        function_14f60(stage, 22);
        texture_surface_dimensions_native(22, width, height);
    }
    else if (mode == 21)
    {
        function_14f60(stage, 13);
        texture_surface_dimensions_native(13, width, height);
    }
    else if (mode == 22)
    {
        function_14f60(stage, 17);
        texture_surface_dimensions_native(17, width, height);
    }
    else if (mode == 23)
    {
        function_14f60(stage, 23);
        texture_surface_dimensions_native(23, width, height);
    }
    else if (mode == 24 || mode == 25)
    {
        long surface = g_485a78[mode - 24];
        function_14f60(stage, (short)surface);
        if (surface == 25 || surface == 26 || surface == 27)
            function_01dd60(surface, width, height);
    }
    else if (mode == 26)
    {
        byte *definition = g_4e3b44[*(long *)(context + 8) & 0xffff].bytes;
        byte *parameters = *(byte **)(definition + 0x24);
        short index = *(short *)(*(byte **)(parameters + 0x60) + 0x10);
        if (index == -1)
        {
            definition = g_4e3b44[*(long *)(g_485a80 + 0xd0) & 0xffff].bytes;
            parameters = *(byte **)(definition + 0x24);
            index = *(short *)(*(byte **)(parameters + 0x60) + 0x10);
        }
        byte *bitmap = *(byte **)(parameters + 8) + index * 12;
        function_143c0(*(long *)bitmap, *(short *)(bitmap + 4), stage, 0.0f);
    }
    else if (mode == 27)
    {
        if (g_485a76)
        {
            function_14f60(stage, 16);
            texture_surface_dimensions_native(16, width, height);
        }
        else
        {
            long tag = *(long *)(g_485a80 + 0x64);
            if (tag != NONE)
            {
                s_type_7ba8e9 *bitmap = function_137550(tag, 0);
                if (bitmap)
                    function_14390(stage, bitmap, 0.0f);
            }
            *width = 4;
            *height = 4;
        }
    }
    else
        __assume(0);
}
