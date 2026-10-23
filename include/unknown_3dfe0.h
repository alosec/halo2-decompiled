#ifndef UNKNOWN_3DFE0_H
#define UNKNOWN_3DFE0_H
#include "unknown_11c920.h"

struct s_3dd10_object_header
{
	byte unknown00[8];
	byte *object;
};

struct s_effect_color_query
{
    real unknown00, unknown04, unknown08;
    dword color_a, color_b;
};
struct s_lighting_record
{
    color3f color0;
    vector3f direction0;
    real length, value1c;
    color3f color1;
    vector3f direction1;
    color3f color2;
    vector3f direction2;
    dword unknown50;
};
struct s_3dfe0_cache
{
    short identifier;
    bool valid, initialized;
    long parent;
    dword sample_time, update_time;
    point3f position;
    s_effect_color_query current, target;
    byte unknown44[0x10];
    s_lighting_record lighting;
    byte unknowna8[0x58];
};

#endif
