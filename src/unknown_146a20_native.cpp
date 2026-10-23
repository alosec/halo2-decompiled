#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_146a20.h"
#include <new.h>

#ifdef H2_NATIVE_PORT
extern void *g_479888;
extern byte *g_510c44;
extern bool g_510c48;
extern bool g_479880;
extern long g_479884;
extern long g_479890;
extern c_havok_abstract_allocator *g_479894;
extern c_havok_abstract_allocator *g_479898;
extern long g_4798b4;
extern "C" c_havok_fixed_memory *h2_native_havok_fixed_memory_construct(
    void *storage, void *buffer, long size);
extern "C" c_havok_page_pool_allocator *h2_native_havok_pool_memory_construct(void *storage);

// Portable construction of the exact recovered allocator objects at 0x146a20.
void function_146a20(void)
{
    g_479884 = NONE;
    g_479890 = 1;
    g_479880 = true;
    g_479894 = h2_native_havok_fixed_memory_construct(
        g_479888, (byte *)g_479888 + 0x30, 0x15000);
    g_479898 = h2_native_havok_pool_memory_construct((byte *)g_479888 + 0x15040);
    g_4798b4++;
}

// Portable cache-owned teardown of the recovered scratch allocator at 0x146de0.
// c_havok_abstract_allocator's destructor is empty and the next placement construction replaces
// the table pointer, so native ownership only clears the live pointer.
void function_146de0(void)
{
    if (g_47989c)
        g_47989c = 0;
}

// Portable construction of the exact recovered scratch allocator at 0x146b80.
void function_146b80(void)
{
    byte *buffer = g_510c44;
    long size = 0x452e8;

    g_510c48 = true;
    if ((dword)buffer & 0xf)
    {
        long adjustment = -(long)buffer & 0xf;
        buffer += adjustment;
        size -= adjustment;
    }
    g_47989c = h2_native_havok_fixed_memory_construct(
        (byte *)g_479888 + 0x105410, buffer, size);
}
#endif
