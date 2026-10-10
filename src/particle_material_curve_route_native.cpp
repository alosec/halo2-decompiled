// Source-local native reconstruction of the nested material curve-range router
// used by function_16b90. The matching Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"

void function_17550(byte *context, word const *range);

// @retail 0x174d0
void function_174d0(byte *context, long group, long stage, long pass, long entry)
{
    byte *definition = *(byte **)(context + 0x0c);
    if (group < *(long *)(definition + 0x1c))
    {
        byte *record = *(byte **)(definition + 0x20) + group * 6;
        if (*(dword *)record & (1u << stage))
        {
            word range = *(word *)(record + 4);
            if (stage < (range >> 9))
            {
                range = (*(word **)(definition + 0x28))[(range & 0x01ff) + stage];
                if (pass < (range >> 9))
                {
                    range = (*(word **)(definition + 0x30))[(range & 0x01ff) + pass];
                    if (entry < (range >> 9))
                    {
                        word const *selected = (word *)(*(byte **)(definition + 0x38)
                            + ((range & 0x01ff) + entry) * 10);
                        function_17550(context, selected);
                    }
                }
            }
        }
    }
}
