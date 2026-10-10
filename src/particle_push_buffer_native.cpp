// Source-local native reconstruction of retail push-buffer reservation. This
// preserves the genuine Xbox submission boundary instead of discarding the
// packet or fabricating host-owned push memory.

#include "unknown_11c920.h"

extern "C" dword *__stdcall D3DDevice_BeginPush(dword dword_count);

// @retail 0x015d00
bool function_015d00(long count, dword **out)
{
    if (count >= 0x800)
    {
        *out = NULL;
        return false;
    }
    *out = D3DDevice_BeginPush(count);
    return true;
}
