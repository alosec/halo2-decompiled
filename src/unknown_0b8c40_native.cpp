#include "unknown_11c920.h"
#include "globals.h"

// Native-port source split of retail 0xb8c40. The matching Xbox definition
// remains in unknown_0b7740.cpp, whose unrelated code requires x86 intrinsics.
struct s_object_transform_view_b8c40
{
    long definition_index;
};

struct s_object_transform_header_b8c40
{
    byte unknown00[8];
    s_object_transform_view_b8c40 *object;
};

struct s_object_named_value_b8c40
{
    byte unknown00[8];
    long value;
    byte unknown0c[0x18 - 0xc];
};

struct s_object_named_values_b8c40
{
    byte unknown00[0x94];
    long count;
    s_object_named_value_b8c40 *entries;
};

// @retail 0xb8c40
long function_b8c40(long object_index, short entry_index)
{
    long result = NONE;
    s_object_transform_view_b8c40 *object =
        ((s_object_transform_header_b8c40 *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_named_values_b8c40 *definition =
        (s_object_named_values_b8c40 *)g_4e3b44[object->definition_index & 0xffff].bytes;
    if (entry_index >= 0 && entry_index < definition->count)
        result = definition->entries[entry_index].value;
    return result;
}
