// Source-local native reconstruction of material combiner-state expansion.
// The Xbox body remains in unknown_020560.cpp.

#include "unknown_11c920.h"

// @retail 0x1ba00
void function_1ba00(byte *state)
{
    byte *definition = *(byte **)(state + 0x20);
    byte *blocks = *(byte **)(definition + 0x2c);
    dword *values = *(dword **)(definition + 0x34);
    byte *selection = *(byte **)(state + 0x24);
    *(dword *)(state + 0x14fc) = *(dword *)(selection + 0xde);
    *(dword *)(state + 0x1504) = *(dword *)(selection + 0xe6);
    *(dword *)(state + 0x1500) = *(dword *)(selection + 0xe2);

    dword output = 0;
    bool final = false;
    long i = 0;
    if ((*(word *)(selection + 0xf6) >> 9) > 0)
    {
        do
        {
            word range = *(word *)(*(byte **)(state + 0x24) + 0xf6);
            short *entry = *(short **)(*(byte **)(state + 0x20) + 0x24) +
                ((range & 0x01ff) + i) * 2;
            state[0x1520 + i] = (byte)output;
            long block_index = (word)entry[1] & 0x01ff;
            byte *block;
            if (entry[0] == NONE)
                block = blocks + block_index * 6;
            else
                block = blocks + (block_index +
                    *(long *)(state + 0x300 + entry[0] * 4)) * 6;
            dword *value = values + (*(word *)(block + 4) & 0x01ff) * 8;
            long count = *(word *)(block + 4) >> 9;
            for (long j = 0; j < count; ++j, value += 8)
            {
                if ((block[2] & 1) && j + 1 == count)
                {
                    *(dword *)(state + 0x1444) = value[0];
                    *(dword *)(state + 0x1448) = value[2];
                    *(dword *)(state + 0x14d0) = value[4];
                    *(dword *)(state + 0x14d4) = value[5];
                    final = true;
                    break;
                }
                *(dword *)(state + 0x14ac + output * 4) = value[0];
                *(dword *)(state + 0x14d8 + output * 4) = value[1];
                *(dword *)(state + 0x1424 + output * 4) = value[2];
                *(dword *)(state + 0x148c + output * 4) = value[3];
                *(dword *)(state + 0x144c + output * 4) = value[4];
                *(dword *)(state + 0x146c + output * 4) = value[5];
                ++output;
            }
            ++i;
        } while (i < (*(word *)(*(byte **)(state + 0x24) + 0xf6) >> 9));
    }
    if (!final)
    {
        *(dword *)(state + 0x1444) = 0x0c;
        *(dword *)(state + 0x1448) = 0x2000;
    }
    *(dword *)(state + 0x14f8) = (output < 1 ? 1 : output) | 0x11100;
}
