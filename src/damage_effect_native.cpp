#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <stddef.h>
#include <string.h>

struct s_damage_owner_native
{
    long object_index;
    long player_index;
    long team;
};

struct s_type_1e6529
{
    long definition_index;
    dword flags;
    s_damage_owner_native owner;
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

static_assert(sizeof(s_damage_owner_native) == 0xc, "damage owner layout");
static_assert(sizeof(s_type_1e6529) == 0x88, "damage event layout");
static_assert(offsetof(s_type_1e6529, unknown7c) == 0x7c, "material offset");
static_assert(offsetof(s_type_1e6529, unknown84) == 0x84, "terminal flag offset");

extern short g_47d8e0;

// @retail 0xd6660
void function_d6660(s_type_1e6529 *data, long definition_index)
{
    memset(data, 0, sizeof(*data));
    data->definition_index = definition_index;
    data->unknown7c = g_47d8e0;
    data->owner.object_index = NONE;
    data->owner.player_index = NONE;
    data->owner.team = NONE;
    data->unknown14 = NONE;
    data->unknown1c = NONE;
    data->unknown20 = NONE;
    data->unknown22 = g_4686c4;
    data->unknown18 = NONE;
    data->unknown54 = 1.0f;
    data->unknown58 = 1.0f;
    data->unknown5c = 1.0f;
    data->unknown7e = NONE;
    data->unknown84 = false;
}
