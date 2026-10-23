// @flags /O2 /GL-
// Havok (third-party physics library): fills a buffer with one byte value.
#include <string.h>

// @retail 0x2d8de0
void __cdecl function_2d8de0(void *buffer, int value, unsigned long size)
{
    memset(buffer, value, size);
}
