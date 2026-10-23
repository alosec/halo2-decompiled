// @flags /O2 /GL-
// Havok (third-party physics library): copies a block of bytes.
#include <string.h>
#include "growable_array_2d91d0.h"

// @retail 0x2d8db0
void __cdecl function_2d8db0(void *destination, void const *source, long count)
{
    memcpy(destination, source, count);
}
