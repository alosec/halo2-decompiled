// @flags /O2 /Gr
/* UNKNOWN_137D40.CPP: clears three 0x40-byte blocks of the game globals,
   stores the current index, then runs every subsystem's +0x10 callback */

#include "unknown_440dd8.h"
#include "globals.h"
#include <string.h>

// @retail 0x137d40
void function_137d40()
{
    s_game_options_view *g = g_4e6948;
    memset((byte *)g + 0x1138, 0, 0x40);
    memset((byte *)g + 0x1178, 0, 0x40);
    memset((byte *)g + 0x11b8, 0, 0x40);
    g->index = g_4686c4;
    for (dword i = 0; i < 68; i++)
    {
        if (g_440dd8[i].field_10)
            g_440dd8[i].field_10();
    }
}
