// Source-local native reconstruction of material texture binding refresh.
// The Xbox body remains in unknown_01cf50.cpp.

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>
#include <string.h>

extern byte *g_485a80;
extern long g_485a60;
void function_19530(long type, byte *state, short slot, long *width, long *height);

// @retail 0x18a90
void function_18a90(byte *state, word const *descriptor)
{
    byte *binding = *(byte **)(*(byte **)(state + 0x20) + 0x0c) +
        (*descriptor & 0x01ff) * 4;
    long i = 0;
    if ((*descriptor >> 9) > 0)
    {
        do
        {
            long slot = binding[2];
            byte *destination = state + 0x1320 + slot * 0x40;
            if (binding[0] != 0xff)
            {
                byte *source = state + 0x320 + (signed char)binding[0] * 0x40;
                memcpy(destination, source, 0x40);
                if ((binding[3] & 2) && *(long *)source != NONE)
                {
                    byte *definition = g_4e3b44[*(long *)source & 0xffff].bytes;
                    real index = *(real *)(source + 0x18);
                    if ((real)*(long *)(definition + 0x44) > index)
                    {
                        byte *bitmap = *(byte **)(definition + 0x48) +
                            (long)floor(index) * 0x74;
                        if (bitmap[0x0e] & 0x10)
                        {
                            *(long *)(source + 8) = *(short *)(bitmap + 4);
                            *(long *)(source + 0x0c) = *(short *)(bitmap + 6);
                        }
                    }
                }
                *(dword *)(state + 0x1420) |= 1u << slot;
            }
            else if (binding[1])
            {
                if (binding[1] != 11)
                {
                    *(long *)destination = NONE;
                    function_19530((signed char)binding[1], state, (short)slot,
                        (long *)(destination + 8), (long *)(destination + 0x0c));
                }
                else
                {
                    byte *location = NULL;
                    if (g_4e0344 && *(long *)((byte *)g_4e0344 + 0x80) > 0 && g_4e0348)
                    {
                        byte *first = *(byte **)((byte *)g_4e0344 + 0x84);
                        if (*(long *)(first + 0x1c) != NONE &&
                            *(long *)(first + 4) == *(long *)((byte *)g_4e0348 + 8) &&
                            g_485a60 != NONE)
                            location = first;
                    }
                    if (location)
                    {
                        long tag = *(long *)(location + 0x1c);
                        if (*(long *)destination != tag ||
                            *(real *)(destination + 0x18) != (real)g_485a60)
                        {
                            byte *definition = g_4e3b44[tag & 0xffff].bytes;
                            byte *bitmap = *(byte **)(definition + 0x48) + g_485a60 * 0x74;
                            *(real *)(destination + 0x10) = 3.0f;
                            *(long *)destination = tag;
                            *(real *)(destination + 0x18) = (real)g_485a60;
                            short width = *(short *)(bitmap + 4);
                            short height = *(short *)(bitmap + 6);
                            long maximum = width > height ? width : height;
                            *(real *)(destination + 0x14) =
                                (real)(log((double)maximum) * 0.6931471824645996f);
                        }
                        *(dword *)(state + 0x1420) |= 1u << slot;
                    }
                    else
                        *(long *)destination = *(long *)(g_485a80 + 0x64);
                }
            }
            else
                *(long *)destination = NONE;
            binding += 4;
            ++i;
        } while (i < (*descriptor >> 9));
    }
}
