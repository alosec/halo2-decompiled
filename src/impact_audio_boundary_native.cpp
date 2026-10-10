// Native impact/effect-audio boundary. The matching Xbox implementations
// submit positional, attached, and looping sound requests. Native gameplay has
// no audio graph yet, so returning NONE preserves the recovered callers'
// ordinary sound-allocation failure paths without fabricating a handle.

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_sound_position;
struct s_sound_label_play;

// @retail 0x189650
long function_189650(s_sound_position const *, s_sound_label_play const *)
{
    return NONE;
}

// Recovered effect creation treats all three results as optional sound handles.
// Until a native audio graph exists, preserve that ordinary allocation-failure
// path for attached, positioned, and looping effect sounds as well.
long function_189060(long, short, real, point3f const *, vector3f const *, long)
{
    return NONE;
}

long function_1895f0(s_sound_position const *, real, long)
{
    return NONE;
}

long function_18a750(long, long)
{
    return NONE;
}
