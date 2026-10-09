#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT
struct s_native_physics_object_header
{
    short identifier;
    byte flags;
    byte type;
    short cluster_index;
    byte unknown06[2];
    byte *object;
};
static_assert(sizeof(s_native_physics_object_header) == 0x0c,
    "physics object header layout");

struct s_native_physics_object
{
    byte unknown000[0xb4];
    long havok_component_index;
};
static_assert(offsetof(s_native_physics_object, havok_component_index) == 0xb4,
    "physics object component offset");

struct s_native_havok_component_flags
{
    short identifier;
    byte unknown02[2];
    union
    {
        dword flags;
        struct
        {
            dword unknown0 : 5;
            dword bit5 : 1;
            dword unknown6 : 11;
            dword bit17 : 1;
            dword unknown18 : 14;
        };
    };
    long object_index;
    byte unknown0c[0xa0 - 0x0c];
};
static_assert(sizeof(s_native_havok_component_flags) == 0xa0,
    "Havok component flags layout");

void function_1d1260(s_havok_component *component);
void __stdcall function_1d01c0(s_havok_component *component);
void function_1cf120(long component_index);
void function_1d1540(s_havok_component *component);

static s_native_physics_object *native_physics_object_get(long object_index)
{
    s_native_physics_object_header *headers =
        (s_native_physics_object_header *)g_4e0300->data;
    return (s_native_physics_object *)headers[object_index & 0xffff].object;
}

static s_native_havok_component_flags *native_component_flags_get(long component_index)
{
    return &((s_native_havok_component_flags *)g_51e9b8->data)
        [component_index & 0xffff];
}

// Scalar native split of retail 0x1c3770. The active state is preserved
// across the exact remove, flag replacement, rebuild, and reactivation path.
void __stdcall function_1c3770(long object_index, dword flags)
{
    s_native_physics_object *object = native_physics_object_get(object_index);
    if (object->havok_component_index != NONE)
    {
        s_native_havok_component_flags *component =
            native_component_flags_get(object->havok_component_index);
        bool active = component->bit5;
        if (active)
            function_1d1260((s_havok_component *)component);
        function_1d01c0((s_havok_component *)component);
        component->flags = flags;
        function_1cf120(object->havok_component_index);
        if (active)
            function_1d1540((s_havok_component *)component);
    }
}

// @retail 0x1c3850
void function_1c3850(long object_index)
{
    s_native_physics_object *object = native_physics_object_get(object_index);
    if (object->havok_component_index != NONE &&
        !native_component_flags_get(object->havok_component_index)->bit17)
        function_1c3770(object_index, 0);
}
#endif
