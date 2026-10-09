#include "unknown_11c920.h"

// @flags /O2 /Gr

#ifdef H2_NATIVE_PORT
/*
 * The retail objects store x86 vtable addresses here.  A native caller must
 * provide complete Microsoft-ABI ARM tables for all three object kinds before
 * this constructor can be admitted to the guest image.
 */
extern "C" dword h2_native_physics_runtime_vtable(long kind);
#endif

static inline dword physics_runtime_vtable(long kind)
{
#ifdef H2_NATIVE_PORT
    return h2_native_physics_runtime_vtable(kind);
#else
    static dword const vtables[3] =
    {
        0x00456194,
        0x00455f58,
        0x00455540,
    };
    return vtables[kind];
#endif
}

static inline void write_dword(byte *block, long offset, dword value)
{
    *(dword *)(block + offset) = value;
}

static inline void write_word(byte *block, long offset, word value)
{
    *(word *)(block + offset) = value;
}

// @retail 0x246c60
void function_246c60(void *block, long unknown)
{
    byte *source = (byte *)block;
    byte *runtime = (byte *)unknown;
    if (*(long *)(source + 0x28) <= 0)
        return;

    dword nested_value = *(dword *)(runtime + 0x68);

    write_dword(runtime, 0x0c, (dword)source);
    write_dword(runtime, 0x08, 0);
    write_dword(runtime, 0x00, physics_runtime_vtable(0));
    write_word(runtime, 0x06, 0x80);

    byte *nested = runtime + 0x50;
    write_dword(nested, 0x08, 0);
    write_dword(nested, 0x0c, (dword)runtime);
    write_dword(nested, 0x10, nested_value);
    write_dword(nested, 0x00, physics_runtime_vtable(1));
    write_word(nested, 0x06, 0x80);

    byte *wrapper = runtime + 0x40;
    write_word(wrapper, 0x06, 1);
    write_dword(wrapper, 0x00, physics_runtime_vtable(2));
    write_dword(wrapper, 0x08, 0);
    write_dword(wrapper, 0x0c, (dword)nested);
    ++*(word *)(nested + 0x06);
    write_dword(wrapper, 0x00, physics_runtime_vtable(2));
    write_word(wrapper, 0x06, 0x80);
}
