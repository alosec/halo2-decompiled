#include "unknown_11c920.h"

// @flags /O2 /Gr

// @retail 0x2fc1a0
long function_2fc1a0(void *object)
{
    return *(long *)((byte *)object + 0x10);
}

// @retail 0x2fc1b0
long function_2fc1b0(void *object, long index)
{
    index++;
    return index < *(long *)((byte *)object + 0x10) ? index : NONE;
}

// @retail 0x2fc1d0
long function_2fc1d0(void *object, long index, long)
{
    return *(long *)(*(byte **)((byte *)object + 0x0c) + index * 8);
}

// @retail 0x2fc1e0
long function_2fc1e0(void *object, long index)
{
    return *(long *)(*(byte **)((byte *)object + 0x0c) + index * 8 + 4);
}

// @retail 0x2fc1f0
long function_2fc1f0(void *object, long index)
{
    return *(long *)(*(byte **)((byte *)object + 0x0c) + index * 8);
}
