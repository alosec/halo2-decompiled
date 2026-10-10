// Native scalar reconstruction of retail periodic curve evaluation at
// 0x17c900. The Xbox source uses fmod followed by x87 fistp after removing
// the fraction; for finite binary32 inputs the remaining value is integral,
// so truncation produces the same index without x87 inline assembly.

#include "unknown_11c920.h"

extern byte const g_468d48[12][1024];

// @retail 0x17c900
real function_17c900(short function_type, real input)
{
    if (function_type == 0)
        return 1.0f;

    real scaled = input * 36.57143f;
    long const whole = (long)scaled;
    real const fraction = scaled - (real)whole;
    long const index = whole & 0x3ff;
    byte const *table = g_468d48[function_type];
    real const a = table[index] * (1.0f / 255.0f);
    real b = table[(index + 1) & 0x3ff] * (1.0f / 255.0f);

    if ((1 << function_type) & 0xc0)
    {
        if (a > 0.75f && 0.25f > b)
            b += 1.0f;
        real result = (1.0f - fraction) * a + b * fraction;
        if (result > 1.0f)
            result -= 1.0f;
        return result;
    }

    return (1.0f - fraction) * a + b * fraction;
}
