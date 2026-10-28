/* RENDER_MEMORY.H: the frame's render memory (g_487b18), 0x27000 bytes handed
   out in dword-rounded pieces, each known by a handle */

#ifndef RENDER_MEMORY_H
#define RENDER_MEMORY_H

#include "unknown_11c920.h"

struct s_matrix_workspace
{
    long size;
    byte data[0x27000];
};

extern s_matrix_workspace g_487b18;

/* the handle of `bytes` (rounded up to dwords) of the frame's render memory, or
   NONE when it is full (src/unknown_03f2b0.cpp, which defines g_487b18, still
   has its own copy of this helper) */
static inline long render_memory_allocate(long bytes)
{
    long result = NONE;
    long whole_size = bytes;
    long remainder = bytes % 4;
    if (remainder)
        whole_size = bytes - remainder + 4;
    long size = g_487b18.size + whole_size;
    if (size <= 0x27000)
    {
        result = g_487b18.size & 0x0fffffff;
        g_487b18.size = size;
    }
    return result;
}

#endif
