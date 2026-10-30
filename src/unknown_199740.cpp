// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_199740.CPP: unpacking a compressed data block (0x199740); its own
   file because retail compares floats with fcomi here (/arch:SSE) */

#include "unknown_11c920.h"
#include "physical_memory.h"
#include "loop_allocator.h"
#include "unknown_080e10.h"
#include <xtl.h>

__declspec(noinline) dword function_199560(void *data, dword size);

void function_12d520(long address);
void *__stdcall loop_allocator_zalloc(void *opaque, long count, long size);
void __stdcall loop_allocator_zfree(void *opaque, void *address);
typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size, block_allocate allocate, block_free release, void *opaque);

// score 3 (was 3): only the first timer-zero dword store precedes PUSH EDI and the result-byte
// store; retail puts it after both. The frame, timestamp pair, retry work and decompressor
// convention now agree. Earlier with the standard decompressor: real wrapper 64 (323/319 bytes),
// stdcall/cdecl stubs 64; field store order 66; one-exit result 64. Unsigned start 52;
// start before the result local 52; separate elapsed subtraction 52; plain-inline pool helper
// 301; retry counter before the test 52. Native unpack status at entry/after allocation
// 130/102; raw storage/unsigned bytes/both 52/52/52; entry status with raw storage/unsigned
// bytes 130/130; later status with both 102. Result after allocation 89; break/continue retry
// loop 52; allocator local before pool setup 57; elapsed read then -= start 52; timestamp
// before the result default 52.
// Round 202 with the recovered callee contract: byte/signed-byte/short success 105/105/106;
// direct test result 54; success from pool admission 66; inverse failure result 120;
// result assigned in both allocation outcomes 124; entry allocator-output workspace 22.
// All result forms and workspace lifetimes restored; shared helper bodies and qualifiers unchanged.
// @retail 0x199740
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size)
{
    bool result = false;
    s_loop_allocator *pool = (s_loop_allocator *)function_80e14(0x10000, 0, 0, 0, 90, 1.0f);
    if (pool)
    {
        function_18e250(pool, 0xffb0, "zlib pool", NULL);
        pool->field3c = true;
        pool->field3d = true;
        s_loop_allocator *allocator = pool;
        *decompressed_size = function_199560(buffer, size);
        if (function_2cb5b0(destination, decompressed_size, buffer + 4, size, loop_allocator_zalloc, loop_allocator_zfree, &allocator) == 0)
            result = true;
        function_12d520((long)pool);
    }
    return result;
}
