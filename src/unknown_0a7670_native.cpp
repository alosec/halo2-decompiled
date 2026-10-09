// Native source split of recovered simulation-object presence query at retail
// 0xa7670. The matching Xbox definition remains in unknown_0a58d0.cpp,
// whose broader entity database has unrelated portability dependencies.

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"

bool function_a7670(long object_index)
{
    if (g_4e6948->mode != 4)
        return false;
    s_object_header *header = &((s_object_header *)g_4e0300->data)[object_index & 0xffff];
    return header->object->field_d4 != NONE;
}
