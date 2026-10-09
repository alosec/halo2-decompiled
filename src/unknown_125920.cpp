#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_124f90.h"
#include "object_queries.h"

// @flags /O2 /arch:SSE /Gr

struct s_sound_listener_125920
{
    long leaf_index;
    short cluster_index;
    bool active;
    byte unknown07;
    real velocity_scale;
    vector3f forward;
    vector3f left;
    vector3f up;
    point3f position;
    vector3f velocity;
};

struct s_sound_system_125920
{
    byte unknown00[0x78];
    bool initialized;
    bool hardware_available;
    bool enabled;
    byte unknown7b[0x88 - 0x7b];
    s_sound_listener_125920 listeners[4];
};

struct s_structure_bsp_125920
{
    byte unknown00[0x30];
    struct s_leaf
    {
        short cluster_index;
        byte unknown02[6];
    } *leaves;
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;
extern s_record_pool *g_4e637c;
struct s_bsp3d;
extern s_bsp3d *g_4e033c;

long function_16bc00(s_record_pool *data, long index);
void function_11bed0(s_location *location, point3f const *point);
long function_14a280(s_bsp3d *bsp, point3f *point, long index);
void sound_voices_update_locations(void);
void looping_sound_update_locations(void);

// @retail 0x125920
void sound_update_locations(void)
{
    s_sound_system_125920 *sound_system = (s_sound_system_125920 *)g_4e6380;
    if (!sound_system->initialized || !sound_system->hardware_available || !sound_system->enabled)
        return;

    s_record_pool *sounds = g_4e637c;
    long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));
    while (sound_index != NONE)
    {
        s_sound_playback *sound =
            &((s_sound_playback *)sounds->data)[sound_index & 0xffff];
        if (sound->location.audible == 1)
        {
            s_location location;
            function_11bed0(&location, &sound->location.spatial.position);
            sound->location.spatial.location = location;
            sound_system = (s_sound_system_125920 *)g_4e6380;
        }
        sound_index = data_datum_index(sounds,
            function_16bc00(sounds, (sound_index & 0xffff) + 1));
    }

    for (long i = 0; i < 4; ++i)
    {
        s_sound_listener_125920 *listener = &sound_system->listeners[i];
        if (listener->active)
        {
            listener->leaf_index = function_14a280(g_4e033c, &listener->position, 0);
            listener->cluster_index = listener->leaf_index != NONE
                ? ((s_structure_bsp_125920 *)g_4e0348)->leaves[listener->leaf_index].cluster_index
                : NONE;
        }
    }
    sound_voices_update_locations();
    looping_sound_update_locations();
}
