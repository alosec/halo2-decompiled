// Native scalar reconstruction of retail function_016ae0. The Xbox source
// uses cvttss2si followed by a negative-fraction correction, which is floor
// for every finite binary32 input used by the particle frame selector.

#include "unknown_11c920.h"

// @retail 0x016ae0
long function_016ae0(real value)
{
    long result = (long)value;
    if (value < (real)result)
        --result;
    return result;
}
