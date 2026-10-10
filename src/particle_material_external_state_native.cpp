// Source-local native reconstruction of external material state refresh.
// The Xbox body remains in unknown_0167a0.cpp.

#include "unknown_11c920.h"

struct vector3f
{
    real i, j, k;
};

real function_336f0(long selector);
void function_33980(long selector, vector3f *out);
long function_34060(long mode, dword flags);

// @retail 0x1ae70
void function_1ae70(byte *state)
{
    *(dword *)(state + 0x58) = 0;
    dword pending[5];
    for (long index = 0; index < 5; ++index)
        pending[index] = ((dword *)(state + 0x30))[index] &
            ((dword *)(state + 0x44))[index];

    for (long base = 0; base < 64; base += 32)
    {
        dword remaining = pending[base >> 5];
        while (remaining)
        {
            long bit = (long)__builtin_ctzl(remaining);
            long index = base + bit;
            ((real *)(state + 0x5c))[index] = function_336f0(index);
            remaining &= ~(1u << bit);
            ((dword *)(state + 0x30))[index >> 5] &= ~(1u << (index & 31));
        }
    }

    for (long base = 0; base < 64; base += 32)
    {
        dword remaining = pending[2 + (base >> 5)];
        while (remaining)
        {
            long bit = (long)__builtin_ctzl(remaining);
            long index = base + bit;
            function_33980(index, (vector3f *)(state + 0x108 + index * 12));
            remaining &= ~(1u << bit);
            ((dword *)(state + 0x38))[index >> 5] &= ~(1u << (index & 31));
        }
    }

    dword remaining = pending[4];
    while (remaining)
    {
        long bit = (long)__builtin_ctzl(remaining);
        ((long *)(state + 0x300))[bit] = function_34060(
            bit, *(dword *)(*(byte **)(state + 0x14) + 2));
        remaining &= ~(1u << bit);
        ((dword *)(state + 0x40))[bit >> 5] &= ~(1u << (bit & 31));
        ((dword *)(state + 0x58))[bit >> 5] |= 1u << (bit & 31);
    }
}
