// Native impact-audio boundary. The matching Xbox implementation remains in
// unknown_189010.cpp and submits a positional sound request. Native gameplay
// has no audio graph yet, so returning NONE preserves the recovered caller's
// ordinary sound-allocation failure path without fabricating a handle.

#include "unknown_11c920.h"

struct s_sound_position;
struct s_sound_label_play;

// @retail 0x189650
long function_189650(s_sound_position const *, s_sound_label_play const *)
{
    return NONE;
}
