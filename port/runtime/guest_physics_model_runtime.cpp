#include "unknown_11c920.h"

/* Leaf providers used only by complete ARM-native physics-model tables.
 * Cache-owned objects are never freed, and no retail x86 code address is
 * installed in a native object. */
extern "C" void *h2_native_physics_delete(void *object, dword)
{
    return object;
}

extern "C" void h2_native_physics_idle(void *)
{
}

extern "C" long h2_native_physics_zero(void *)
{
    return 0;
}

extern "C" long h2_native_physics_none(void *)
{
    return NONE;
}

extern "C" long h2_native_physics_kind_2(void *) { return 2; }
extern "C" long h2_native_physics_kind_6(void *) { return 6; }
extern "C" long h2_native_physics_kind_7(void *) { return 7; }
extern "C" long h2_native_physics_kind_8(void *) { return 8; }
extern "C" long h2_native_physics_kind_10(void *) { return 10; }
extern "C" long h2_native_physics_kind_22(void *) { return 22; }

extern "C" void h2_native_physics_first_vertex(void *object, float *result)
{
    float const *source = (float const *)((byte *)object + 0x20);
    result[0] = source[0];
    result[1] = source[1];
    result[2] = source[2];
    result[3] = source[3];
}
