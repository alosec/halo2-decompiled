#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "globals.h"

#if defined(H2_NATIVE_PORT)

struct s_damage_owner_area_native
{
    long player_index;
    long object_index;
    short team;
};

struct s_type_1e6529
{
    long definition_index;
    byte unknown04[4];
    s_damage_owner_area_native owner;
    long unknown14;
    long unknown18;
    long unknown1c;
    short unknown20;
    short unknown22;
    point3f position;
    point3f origin;
    vector3f direction;
    vector3f node_direction;
    real unknown54;
    real unknown58;
    real unknown5c;
    real distance;
    real distance_scale;
    bool in_unknown_radius;
    byte unknown69[3];
    vector3f cone_direction;
    real unknown78;
    short unknown7c;
    short unknown7e;
    byte unknown80[4];
    bool unknown84;
    byte unknown85[3];
};
static_assert(sizeof(s_type_1e6529) == 0x88, "area damage event layout");

struct s_damage_definition_area_native
{
    real minimum_radius;
    real radius04;
    byte unknown08[4];
    dword flags0c;
    byte unknown10[4];
    dword flags14;
    byte unknown18[0x28 - 0x18];
    real cone_inner_angle;
    real cone_outer_angle;
    byte unknown30[0x44 - 0x30];
    real unknown44;
    real unknown48;
    real unknown4c;
    byte unknown50[0x58 - 0x50];
    real radius58;
    byte unknown5c[0x64 - 0x5c];
    real player_radius;
    real radius68;
};
static_assert(sizeof(s_damage_definition_area_native) == 0x6c, "area damage definition layout");

struct s_damage_player_area_native
{
    byte unknown00[0x2c];
    long unit_index;
};
static_assert(sizeof(s_damage_player_area_native) == 0x30, "area damage player layout");

struct s_damage_object_area_native
{
    long tag_index;
    byte unknown04[8];
    long next_object_index;
    long first_child_object_index;
    long parent_object_index;
    byte unknown18[0x30 - 0x18];
    point3f center;
    real radius;
    byte unknown40[0x13c - 0x40];
    long player_index;
};

static_assert(sizeof(s_damage_object_area_native) == 0x140, "area damage object view");

struct s_damage_object_header_area_native
{
    byte unknown00[8];
    s_damage_object_area_native *object;
};
static_assert(sizeof(s_damage_object_header_area_native) == 0x0c, "area damage object header");

struct s_object;
short __stdcall function_bb050(long mask, dword type_mask, void const *location,
    point3f const *position, real radius, long *objects, short maximum_count);
void function_d74e0(s_type_1e6529 *data, long object_index, bool child);
void __stdcall function_184250(s_type_1e6529 const *data);
s_object *function_badc0(long object_index, dword type_mask);

static s_damage_object_area_native *damage_object_area_native(long object_index)
{
    return ((s_damage_object_header_area_native *)g_4e0300->data)[object_index & 0xffff].object;
}

// @retail 0xd6c80
// This source-local split isolates the exact area dispatcher from unrelated
// x86-only damage.cpp assembly. It does not own delivery through function_d74e0.
long function_d6c80(s_type_1e6529 *data, long ignore_object_index)
{
    s_damage_definition_area_native *definition =
        (s_damage_definition_area_native *)g_4e3b44[data->definition_index & 0xffff].bytes;
    real inner_radius = definition->radius58 > definition->radius68 ?
        definition->radius58 : definition->radius68;
    real radius = definition->radius04 > inner_radius ?
        definition->radius04 : inner_radius;

    long objects[64];
    long object_count = function_bb050(0, (definition->flags0c & 2) ? 3 : 0,
        &data->unknown1c, &data->position, radius, objects, 64);
    long first_object_index = NONE;
    long last_object_index = NONE;
    *(dword *)data->unknown04 |= 1;

    if (definition->player_radius > radius)
    {
        long player_index = NONE;
        while ((player_index = data_next_absolute_index_inlined(g_4e8c24,
            player_index + 1)) != NONE)
        {
            s_damage_player_area_native *player = (s_damage_player_area_native *)
                (g_4e8c24->data + g_4e8c24->size * player_index);
            if (!player)
                break;
            if (player->unit_index == NONE)
                continue;

            long root_index = player->unit_index;
            while (damage_object_area_native(root_index)->parent_object_index != NONE)
                root_index = damage_object_area_native(root_index)->parent_object_index;
            s_damage_object_area_native *root = damage_object_area_native(root_index);
            if (object_count >= 64)
                continue;

            real dx = root->center.x - data->position.x;
            real dy = root->center.y - data->position.y;
            real dz = root->center.z - data->position.z;
            real distance_squared = dz * dz + dy * dy + dx * dx;
            real inner = root->radius + radius;
            if (inner * inner >= distance_squared)
                continue;
            real outer = definition->player_radius + root->radius;
            if (outer * outer >= distance_squared)
                objects[object_count++] = player->unit_index;
        }
    }

    for (long i = 0; i < object_count; ++i)
    {
        long object_index = objects[i];
        if (definition->flags0c & 2)
        {
            s_damage_object_area_native *unit =
                (s_damage_object_area_native *)function_badc0(object_index, 3);
            if (!unit || unit->player_index == NONE)
                continue;
        }
        if (object_index == ignore_object_index)
            continue;

        s_damage_object_area_native *unit =
            (s_damage_object_area_native *)function_badc0(object_index, 3);
        s_type_1e6529 copy = *data;
        function_d74e0(&copy, object_index, false);
        last_object_index = object_index;
        if (unit)
        {
            if (first_object_index == NONE)
                first_object_index = object_index;
            else
            {
                s_damage_object_area_native *other =
                    (s_damage_object_area_native *)function_badc0(object_index, 3);
                if (other && other->player_index != NONE)
                    first_object_index = object_index;
            }
        }
    }

    if (g_4e6948->mode != 4)
        function_184250(data);
    return first_object_index != NONE ? first_object_index : last_object_index;
}

#endif
