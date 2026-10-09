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
extern hkMemory *g_479894;
extern hkMemory *g_479898;
extern long g_4798b4;
extern "C" c_havok_fixed_memory *h2_native_havok_fixed_memory_construct(
    void *storage, void *buffer, long size);

// Portable construction of the exact recovered allocator objects at 0x146a20.
void function_146a20(void)
{
    g_479884 = NONE;
    g_479890 = 1;
    g_479880 = true;
    g_479894 = new (g_479888) c_havok_fixed_memory((byte *)g_479888 + 0x30, 0x15000);
    g_479898 = new ((byte *)g_479888 + 0x15040) hkPoolMemory;
    g_4798b4++;
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
