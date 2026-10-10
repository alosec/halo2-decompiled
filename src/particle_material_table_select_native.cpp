// Source-local native reconstruction of material pass table selection.
// The Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"

void function_176a0(byte *state, word const *range);
void function_17960(byte *state, word const *range);
void function_17b60(byte *state, word const *range);
void function_18560(byte *state, word const *range);

// @retail 0x17420
void function_17420(byte *state, long stage, long group, long pass, long variant)
{
    byte const *definition = *(byte **)(state + 0x0c);
    if (stage < *(long const *)(definition + 0x1c))
    {
        byte const *stage_entry = *(byte **)(definition + 0x20) + stage * 6;
        if ((*(dword const *)stage_entry & (1u << group)) &&
            group < (*(word const *)(stage_entry + 4) >> 9))
        {
            word packed = ((word const *)*(byte **)(definition + 0x28))[
                (*(word const *)(stage_entry + 4) & 0x01ff) + group];
            if (pass < (packed >> 9))
            {
                packed = ((word const *)*(byte **)(definition + 0x30))[
                    (packed & 0x01ff) + pass];
                if (variant < (packed >> 9))
                {
                    word const *entry = (word const *)(*(byte **)(definition + 0x38) +
                        ((packed & 0x01ff) + variant) * 10);
                    function_176a0(state, entry + 3);
                    function_17960(state, entry + 4);
                    function_17b60(state, entry + 1);
                    function_18560(state, entry + 2);
                }
            }
        }
    }
}
