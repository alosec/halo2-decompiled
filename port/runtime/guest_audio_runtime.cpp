#include "unknown_11c920.h"

/* The native port does not have an audio backend yet. Keep the recovered menu
 * music state explicitly silent instead of fabricating a looping-sound handle. */
struct s_main_menu_music
{
    long playing;
    long wanted;
    long enabled;
    dword time;
    long sound_index;
    dword stop_tick;
};
static_assert(sizeof(s_main_menu_music) == 0x18, "recovered menu music layout");

/* Native audio has no voices or impulse buffers to pause yet. */
void function_236299(long)
{
    /* Native menu audio remains explicitly silent. */
}

void function_125a90(long)
{
}

void main_menu_music_update(s_main_menu_music *music)
{
    if (!music)
        return;

    music->playing = 0;
    music->wanted = 0;
    music->enabled = 0;
    music->sound_index = NONE;
}
