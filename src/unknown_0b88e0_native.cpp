// Native source split for the recovered object partition record refresh and
// its collision-filter flags. Matching Xbox definitions remain in the original
// sources, whose unrelated paths use x86-only intrinsics and assembly.

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

struct s_object_partition_record_ab
{
    short type;
    word flags;
    point3f centre;
    real radius;
};

struct s_object_partition_header_ab_native
{
    byte unknown00[8];
    byte *object;
};

static_assert(sizeof(s_object_partition_record_ab) == 0x14,
    "object partition record size");
static_assert(offsetof(s_object_partition_record_ab, centre) == 0x04,
    "object partition record centre offset");
static_assert(sizeof(s_object_partition_header_ab_native) == 0x0c,
    "object partition header size");
static_assert(offsetof(s_object_partition_header_ab_native, object) == 0x08,
    "object partition pointer offset");

// @retail 0x16b430
word collision_object_flags(long object_index)
{
    s_object_partition_header_ab_native *header =
        &((s_object_partition_header_ab_native *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;
    dword primary_flags = *(dword *)(object + 0x04);
    long type = (char)object[0xaa];
    word result = 0;

    if ((header->unknown00[2] & 0x10) || (primary_flags & 1))
        result |= 1u << 0;
    if (primary_flags & (1u << 20))
        result |= 1u << 1;
    if (!(primary_flags & (1u << 22)))
        result |= 1u << 2;
    if (!(primary_flags & (1u << 31)))
        result |= 1u << 3;
    if (type == 0)
    {
        if (*(word *)(object + 0x10a) & (1u << 2))
            result |= 1u << 4;
        if (*(word *)(object + 0x348) & (1u << 3))
            result |= 1u << 5;
    }
    else if (type == 0x0b && (*(dword *)(object + 0x12c) & (1u << 1)))
    {
        result |= 1u << 6;
    }
    return result;
}

// @retail 0xb88e0
void function_b88e0(long object_index, s_object_partition_record_ab *record)
{
    byte *object = ((s_object_partition_header_ab_native *)g_4e0300->data)
        [object_index & 0xffff].object;
    word flags = 0;
    if ((*(dword *)(object + 0x04) >> 9) & 1)
        flags = collision_object_flags(object_index);
    record->type = (char)object[0xaa];
    record->flags = flags;
    record->centre = *(point3f *)(object + 0x40);
    record->radius = *(real *)(object + 0x4c);
}
