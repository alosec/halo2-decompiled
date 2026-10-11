// Source-local process-lifetime storage for the recovered one-block sound
// codec allocator. Driver startup reserves and publishes the exact 0xcc-byte
// static block; the genuine XMem allocation/free path owns the in-use flag.
// This storage does not implement or bypass that allocator path.

#include "unknown_11c920.h"

void *g_510c3c;
bool g_510c40;
