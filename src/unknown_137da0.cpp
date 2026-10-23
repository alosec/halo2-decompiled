// @flags /O2 /Gr
/* UNKNOWN_137DA0.CPP: runs every subsystem's +0x14 callback, last to
   first, then clears the current index */

#include "unknown_440dd8.h"
#include "globals.h"

// @retail 0x137da0
void function_137da0()
{
    for (long i = 67; i >= 0; i--)
    {
        if (g_440dd8[i].field_14)
            g_440dd8[i].field_14();
    }
    g_4e6948->index = NONE;
}
