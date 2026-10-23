// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A36A0.CPP: the weapon effect event's v11, which retail built in a
   scalar-SSE object (its negations and 1.0f stores use xmm registers) */

#include "effects.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>

extern vector3f *g_4687bc;
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);

// @retail 0xa36a0
bool c_weapon_effect_event::v11(long a, long const *entities, long c, void const *data)
{
    long object = function_a58d0(entities[0]);
    if (object != NONE)
    {
        byte *object_data = (byte *)((s_object_header *)g_4e0300->data)[object & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object_data & 0xffff].bytes;
        if (*(short *)(definition + 0x290) == 1)
        {
            long global_tag = *(long *)((byte *)g_4e034c + 0x16c);
            byte *settings = *(byte **)(*(byte **)(g_4e3b44[global_tag & 0xffff].bytes + 0xc) + 0x534);
            byte const *event = (byte const *)data;
            long mode = *(long const *)event, effect;
            switch (mode)
            {
            case 0: effect = *(long *)(settings + 0xe0); break;
            case 1: effect = *(long *)(settings + 0xf0); break;
            default: effect = NONE; break;
            }
            if (effect != NONE)
            {
                byte const *source_position = event + 4;
                vector3f up = *g_4687b0;
                s_effect_marker markers[5];
                markers[0].position = markers[1].position = markers[2].position =
                    markers[3].position = markers[4].position = *(point3f const *)source_position;
                markers[0].forward = *g_4687b0;
                markers[1].forward = *g_4687bc;
                markers[2].forward = *g_4687b0;
                markers[3].forward.i = 0.0f - up.i;
                markers[3].forward.j = 0.0f - up.j;
                markers[3].forward.k = 0.0f - up.k;
                markers[4].forward = *g_4687b0;
                markers[0].name = 0x700054c;
                markers[1].name = 0x70000c0;
                markers[2].name = 0x20000ca;
                markers[3].name = 0x8000550;
                markers[4].name = 0x400054f;
                s_effect_parameters parameters;
                memset(&parameters, 0, sizeof(parameters));
                parameters.color_b = parameters.color_a = 0xff808080;
                parameters.unknown34 = 0;
                parameters.unknown38 = 0;
                parameters.unknown3c = 0;
                parameters.unknown30 = 0;
                parameters.source = NULL;
                parameters.unknown18 = NONE;
                parameters.object_index = NONE;
                parameters.scale_b = parameters.scale_a = 1.0f;
                parameters.owner.unknown4 = NONE;
                parameters.owner.unknown0 = NONE;
                parameters.owner.unknown8 = NONE;
                parameters.tag_index = effect;
                parameters.marker_count = 5;
                parameters.markers = markers;
                parameters.flags = 4;
                effect_new_from_parameters(&parameters);
            }
        }
    }
    return false;
}
