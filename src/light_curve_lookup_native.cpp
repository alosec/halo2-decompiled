#include "unknown_11c920.h"

#if defined(H2_NATIVE_PORT)

// The recovered lookup table is initialized by the normal tag-function-table
// lifecycle.  Owning this evaluator does not claim that mutable table ready.
extern byte g_46bd48[16][1024];

static long light_curve_round_nearest_even(real value)
{
    long truncated = (long)value;
    real remainder = value - (real)truncated;
    if (remainder > 0.5f || (remainder == 0.5f && (truncated & 1)))
        return truncated + 1;
    if (remainder < -0.5f || (remainder == -0.5f && (truncated & 1)))
        return truncated - 1;
    return truncated;
}

// @retail 0x17ca10
real function_17ca10(real x, short curve)
{
    if (0.0f > x)
        x = 0.0f;
    else if (x > 1.0f)
        x = 1.0f;

    if (curve)
    {
        byte const *table = g_46bd48[curve];
        real scaled = x * 1023.0f;
        long whole = (long)scaled;
        real fraction = scaled - (real)whole;
        short index = (short)light_curve_round_nearest_even(scaled - 0.1f);

        if (index == 0x3ff)
            x = table[0x3ff] * (1.0f / 255.0f);
        else
        {
            real a = table[index] * (1.0f / 255.0f);
            real b = table[index + 1] * (1.0f / 255.0f);
            x = a * (1.0f - fraction) + b * fraction;
        }

        if (0.0f > x)
            x = 0.0f;
        else if (x > 1.0f)
            x = 1.0f;
    }
    return x;
}

#endif
