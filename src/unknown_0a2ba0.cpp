// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A2BA0.CPP: the fire event of a weapon (built with SSE, apart from
   its neighbours in unknown_09b910.cpp) */

#include "unit_requests.h"
#include "effects.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>
#include <xmmintrin.h>

struct s_z_event_header
{
    word identifier;
    byte flags, type;
    dword unknown04;
    byte *object;
};

void function_d0930(long unit_index, vector3f *direction);
void __stdcall function_102fb0(long weapon_index, short barrel);
void function_103e60(long weapon_index, short barrel);
void __stdcall function_fd980(long weapon_index);
transform4x3f *function_b8c00(long object_index, long *count);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *result);

/* a unit's object relevance (0x24 bytes, at +0x1c8), which 0x82b30 fills */
struct s_object_relevance_source
{
    long object_index, identifier;
    long unknown08;
    point3f point;
    word flags;                           /* 2: point is valid */
    word unknown1a;
    real first, second;
};
typedef char assert_object_relevance_source_size[(sizeof(s_object_relevance_source) == 0x24) ? 1 : -1];

struct s_object_relevance_result
{
    real first, second;
    long object_index, identifier;
};
void function_82b30(s_object_relevance_result const *source, s_object_relevance_source *result);

/* the data of a weapon-fire event (0x44 bytes, c_weapon_fire_event::v2) */
struct s_weapon_fire_event_data
{
    long slot;
    long definition_index;
    long barrel;
    bool field_c;
    short node;
    point3f point;
    vector3f direction;
    s_object_relevance_result relevance;
    bool field_38;
    long attachment_object;
    long attachment;
};
typedef char assert_weapon_fire_event_data_size[(sizeof(s_weapon_fire_event_data) == 0x44) ? 1 : -1];

/* the unit fields read here */
struct s_fire_event_unit
{
    byte unknown000[0x10a];
    word unknown_bits0 : 2;
    word suppressed : 1;
    word unknown_bits3 : 13;
    byte unknown10c[0x1c8 - 0x10c];
    s_object_relevance_source unknown1c8;
    byte unknown1ec[0x218 - 0x1ec];
    long weapon_object_indices[4];
    byte unknown228[0x24c - 0x228];
    long unknown24c;
};

/* the weapon fields written here */
struct s_fire_event_weapon
{
    long definition_index;
    byte unknown004[0x177 - 4];
    char unknown177;
    byte unknown178[0x194 - 0x178];
    long unknown194;
};

/* a barrel of a weapon definition (0xec bytes) */
struct s_fire_event_barrel
{
    dword unknown_bits0 : 13;
    dword flag13 : 1;
    dword unknown_bits14 : 18;
    byte unknown04[0xec - 4];
};
typedef char assert_fire_event_barrel_size[(sizeof(s_fire_event_barrel) == 0xec) ? 1 : -1];

/* applies a weapon-fire event: the firing unit's object relevance and target
   point, the barrel's attachment, then fires the weapon's barrel */
// @retail 0xa2ba0
bool c_weapon_fire_event::v11(long a, long const *entities, long c, void const *data)
{
    bool result = false;
    long entity = function_a58d0(entities[0]);

    if (entity != NONE)
    {
        s_z_event_header *header = (s_z_event_header *)g_4e0300->data + (entity & 0xffff);
        long type = header->type;
        s_weapon_fire_event_data const *event = (s_weapon_fire_event_data const *)data;
        long unit_index = NONE;
        long weapon_index = NONE;

        if (type == 2)
        {
            weapon_index = entity;
        }
        else if (((1 << type) & 3) && event->slot >= 0 && event->slot < 4 &&
            !(bool)((s_fire_event_unit *)header->object)->suppressed)
        {
            s_fire_event_unit *unit = (s_fire_event_unit *)header->object;

            weapon_index = unit->weapon_object_indices[event->slot];
            unit_index = unit->unknown24c != NONE ? unit->unknown24c : entity;
        }
        if (weapon_index != NONE)
        {
            s_fire_event_weapon *weapon = (s_fire_event_weapon *)((s_z_event_header *)g_4e0300->data)[weapon_index & 0xffff].object;
            byte *definition = g_4e3b44[weapon->definition_index & 0xffff].bytes;

            if (event->definition_index == weapon->definition_index && event->barrel >= 0 && event->barrel < *(long *)(definition + 0x2d0))
            {
                if (unit_index != NONE)
                {
                    s_fire_event_unit *unit = (s_fire_event_unit *)((s_z_event_header *)g_4e0300->data)[unit_index & 0xffff].object;
                    s_object_relevance_source source;

                    source.object_index = NONE;
                    source.identifier = NONE;
                    source.unknown08 = NONE;
                    source.first = 0.0f;
                    source.second = 0.0f;
                    source.flags = 0;
                    function_82b30(&event->relevance, &source);
                    if ((source.first > 0.0f || source.second > 0.0f) && source.object_index != NONE)
                        unit->unknown1c8 = source;
                    if ((bool)(*(s_fire_event_barrel **)(definition + 0x2d4))[event->barrel].flag13 && event->field_38)
                    {
                        long attachment_object = function_a58d0(event->attachment_object);

                        if (attachment_object != NONE && event->attachment != NONE)
                        {
                            weapon->unknown177 = (char)event->attachment;
                            weapon->unknown194 = attachment_object;
                        }
                    }
                    if (event->field_c && entities[1] != NONE)
                    {
                        long target = function_a58d0(entities[1]);

                        if (target != NONE)
                        {
                            long count = 0;
                            transform4x3f *matrices = function_b8c00(target, &count);

                            if (event->node >= 0 && event->node < count)
                            {
                                transform4x3f_apply_point(&matrices[event->node], &event->point, &unit->unknown1c8.point);
                                unit->unknown1c8.flags |= 2;
                            }
                        }
                    }
                    function_d0930(unit_index, (vector3f *)&event->direction);
                }
                function_102fb0(weapon_index, (short)event->barrel);
                function_103e60(weapon_index, (short)event->barrel);
                function_fd980(weapon_index);
                if (unit_index != NONE && event->field_c)
                {
                    s_fire_event_unit *unit = (s_fire_event_unit *)((s_z_event_header *)g_4e0300->data)[unit_index & 0xffff].object;
                    s_object_relevance_source *relevance = &unit->unknown1c8;

                    relevance->flags &= ~2;
                }
                result = true;
            }
        }
    }
    return result;
}
