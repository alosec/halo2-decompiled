#include "unknown_11c920.h"

// @flags /O2 /Gr

#ifdef H2_NATIVE_PORT
/* The three type queries (retail 0x2fbb30, 0x2fdfe0, 0x2fbfe0) return the
   addresses of these static type objects. */
struct object_4803c4;
struct object_480504;
struct object_4803ec;
extern object_4803c4 g_4803c4;
extern object_480504 g_480504;
extern object_4803ec g_4803ec;

static void *h2_native_physics_delete(void *object, dword)
{
    /* All three objects are embedded in cache-owned tag memory. */
    return object;
}

static long h2_native_physics_query(void *, long, long)
{
    /* Native has no Xbox reflection/registration dictionary. */
    return 0;
}

static void h2_native_physics_enable(void *object, bool enable)
{
    word *flags = (word *)((byte *)object + 4);
    if (enable)
        *flags |= 0x8000;
    else
        *flags &= 0x7fff;
}

static void *h2_native_physics_root_type(void *)
{
    return &g_4803c4;
}

static void *h2_native_physics_nested_type(void *)
{
    return &g_480504;
}

static void *h2_native_physics_wrapper_type(void *)
{
    return &g_4803ec;
}

static void h2_native_physics_idle(void *)
{
}

typedef void (*h2_native_physics_method)();

static h2_native_physics_method const g_native_physics_runtime_vtables[3][5] =
{
    {
        (h2_native_physics_method)h2_native_physics_delete,
        (h2_native_physics_method)h2_native_physics_query,
        (h2_native_physics_method)h2_native_physics_enable,
        (h2_native_physics_method)h2_native_physics_root_type,
        (h2_native_physics_method)h2_native_physics_idle,
    },
    {
        (h2_native_physics_method)h2_native_physics_delete,
        (h2_native_physics_method)h2_native_physics_query,
        (h2_native_physics_method)h2_native_physics_enable,
        (h2_native_physics_method)h2_native_physics_nested_type,
        (h2_native_physics_method)h2_native_physics_idle,
    },
    {
        (h2_native_physics_method)h2_native_physics_delete,
        (h2_native_physics_method)h2_native_physics_query,
        (h2_native_physics_method)h2_native_physics_enable,
        (h2_native_physics_method)h2_native_physics_wrapper_type,
        (h2_native_physics_method)h2_native_physics_idle,
    },
};

static long g_native_physics_runtime_object_count;

extern "C" void h2_native_physics_runtime_reset()
{
    g_native_physics_runtime_object_count = 0;
}

extern "C" long h2_native_physics_runtime_object_count()
{
    return g_native_physics_runtime_object_count;
}

extern "C" dword h2_native_physics_runtime_vtable(long kind)
{
    if ((dword)kind >= 3)
        return 0;
    return (dword)g_native_physics_runtime_vtables[kind];
}
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
#ifdef H2_NATIVE_PORT
    g_native_physics_runtime_object_count++;
#endif

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
