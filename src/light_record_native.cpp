#include "data_array.h"
#include "effects.h"
#include "globals.h"
#include "math.h"

#ifdef H2_NATIVE_PORT

extern s_record_pool *g_4e0300;
extern s_record_pool *g_4e030c;
extern long g_4e0308;

void function_c0c80(vector3f const *up);

struct s_native_object_header_c0350
{
    byte unknown00[8];
    byte *datum;
};

static_assert(sizeof(s_native_object_header_c0350) == 0xc,
    "object header layout changed");

static void copy_vector(byte *destination, vector3f const *source)
{
    *(vector3f *)destination = *source;
}

// @retail 0xc0350
void function_c0350(long tag_index, long object_index, long node_index,
    vector3f const *up, vector3f const *forward, point3f const *position,
    real scale)
{
    long light_index = record_pool_allocate(g_4e030c);
    if (light_index == NONE)
        return;

    byte *light = g_4e030c->data + (light_index & 0xffff) * 0x110;
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;

    *(long *)(light + 0x04) = tag_index;
    *(short *)(light + 0x02) = 0;
    *(long *)(light + 0x14) = g_510c54->game_time;
    *(long *)(light + 0x50) = 0;
    *(short *)(light + 0x56) = 0;
    *(long *)(light + 0x08) = NONE;
    *(long *)(light + 0x4c) = NONE;
    *(short *)(light + 0x54) = NONE;
    *(real *)(light + 0xcc) = scale;
    *(real *)(light + 0xd0) = scale;
    *(short *)(light + 0x02) = *(real *)(definition + 0xb0) > 0.0f ? 0x11 : 1;
    *(long *)(light + 0x10) = NONE;

    if (object_index == NONE)
    {
        copy_vector(light + 0x84, forward);
        ((point3f *)(light + 0x90))->x = forward->i + position->x;
        ((point3f *)(light + 0x90))->y = forward->j + position->y;
        ((point3f *)(light + 0x90))->z = forward->k + position->z;
        *(point3f *)(light + 0xac) = *position;
        copy_vector(light + 0xb8, up);
        *(long *)(light + 0x58) = NONE;
        *(short *)(light + 0x5c) = NONE;
    }
    else
    {
        s_native_object_header_c0350 *headers =
            (s_native_object_header_c0350 *)g_4e0300->data;
        byte *object = headers[object_index & 0xffff].datum;
        copy_vector(light + 0x60, forward);
        *(point3f *)(light + 0x6c) = *position;
        copy_vector(light + 0x78, up);
        *(short *)(light + 0x5c) = (short)node_index;
        *(long *)(light + 0x58) = object_index;
        *(dword *)(object + 0x04) |= 0x40;
    }

    function_c0c80(up);
    *(long *)(light + 0x0c) = g_4e0308 - 1;
    *(long *)(light + 0x10c) = NONE;
}

#endif
