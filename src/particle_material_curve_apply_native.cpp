// Source-local native reconstruction of material curve-range application used
// beneath function_174d0. The Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"

struct s_1b230_function
{
    long size;
    byte *data;
};

real function_1b230(
    s_1b230_function const *definition, long input_index, long range_index, real period);

// @retail 0x17550
void function_17550(byte *context, word const *range)
{
    if ((*range >> 9) > 0)
    {
        byte *definition = *(byte **)(context + 0x0c);
        byte *entry = *(byte **)(definition + 0x58) + (*range & 0x01ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            definition = *(byte **)(context + 0x0c);
            word const *subrange = *(word **)(definition + 0x50) + entry[3];
            short const *item = *(short **)(definition + 0x48) + (*subrange & 0x01ff) * 2;
            for (long j = 0; j < (*subrange >> 9); ++j, item += 2)
            {
                byte *function = *(byte **)(*(byte **)(context + 0x0c) + 0x40)
                    + item[0] * 20;
                s_1b230_function const *curve =
                    (s_1b230_function const *)(function + 12);
                real value = function_1b230(
                    curve, *(long *)function, *(long *)(function + 4), *(real *)(function + 8));
                byte *data = curve->data;
                if (!(data[1] & 0xf0))
                {
                    real lower = *(real *)(data + 4);
                    real upper = *(real *)(data + 8);
                    if (0.0f > value) value = 0.0f;
                    else if (value > 1.0f) value = 1.0f;
                    value = (upper - lower) * value + lower;
                }
                ((real *)(context + 0x338))[entry[0] * 16 + item[1]] = value;
            }
        }
    }
}
