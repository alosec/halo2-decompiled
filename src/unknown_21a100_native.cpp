// Native source split of recovered looping-sound BSP location refresh at
// retail 0x21a100. The matching Xbox definition remains in
// unknown_12a1b0.cpp; this avoids unrelated playback-controller code.

#include "unknown_11c920.h"
#include "data_array.h"
#include "sound_sources.h"
#include <stddef.h>

struct s_looping_sound_datum_21a100
{
    short salt;
    short sound_class;
    long definition_index;
    long identifier;
    s_type_99c531 source;
    byte flags;
    byte playing_sound_count;
    byte state;
    byte update_phase;
    long controller_index;
    long detail_sound_times[12];
    byte track_storage[0xc4 - 0x88];
    real listener_gains[4];
};

static_assert(offsetof(s_looping_sound_datum_21a100, source) == 0x0c, "looping sound source offset");
static_assert(sizeof(s_looping_sound_datum_21a100) == 0xd4, "looping sound datum size");

extern void *g_502110;
void function_11bed0(s_location *location, point3f const *point);

void looping_sound_update_locations(void)
{
    s_record_pool *data = (s_record_pool *)g_502110;
    long index = data_datum_index(data, function_16bc00(data, 0));
    while (index != NONE)
    {
        s_looping_sound_datum_21a100 *sound =
            (s_looping_sound_datum_21a100 *)data->data + (index & 0xffff);
        if (sound->source.audible == 1)
            function_11bed0(&sound->source.spatial.location, &sound->source.spatial.position);
        long const start = (index & 0xffff) + 1;
        index = data_datum_index(data, function_16bc00(data, start));
    }
}
