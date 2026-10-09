#include "unknown_11c920.h"
#include "globals.h"

/* Native specialization of the six populated retail function_1ea9c0 branches.
 * This unit is compiled independently but is not linked into production until
 * the complete provider closure is linked and the read-only 00a preflight has
 * succeeded. The four families proven empty in authentic 00a are omitted. */

struct s_shape_block_table;
void *function_1eaf30(short type, s_shape_block_table *table, short index);

extern "C" dword h2_native_physics_model_vtable_00455500[];
extern "C" dword h2_native_physics_model_vtable_00455ef0[];
extern "C" dword h2_native_physics_model_vtable_00455498[];
extern "C" dword h2_native_physics_model_vtable_00456008[];
extern "C" dword h2_native_physics_model_vtable_00455fc0[];
extern "C" dword h2_native_physics_model_vtable_00455f30[];
extern "C" dword h2_native_physics_model_vtable_00455f8c[];

static long g_native_physics_model_written_objects;
static long g_native_physics_model_written_resolutions;

static void h2_native_physics_install(byte *object, dword *vtable)
{
    *(dword *)(object + 0x00) = (dword)vtable;
    *(word *)(object + 0x06) = 0x80;
    *(dword *)(object + 0x08) = 0;
    g_native_physics_model_written_objects++;
}

static void *h2_native_physics_resolve(byte *definition, dword key)
{
    g_native_physics_model_written_resolutions++;
    return function_1eaf30((short)key, (s_shape_block_table *)definition,
        (short)(key >> 16));
}

extern "C" void h2_native_physics_model_write_reset()
{
    g_native_physics_model_written_objects = 0;
    g_native_physics_model_written_resolutions = 0;
}

extern "C" int h2_native_physics_model_write_status()
{
    return (g_native_physics_model_written_resolutions << 16) |
        (g_native_physics_model_written_objects & 0xffff);
}

extern "C" void h2_native_physics_model_initialize(long tag_index)
{
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;

    long count = *(long *)(definition + 0x38);
    byte *entries = *(byte **)(definition + 0x3c);
    for (long i = 0; i < count; i++)
    {
        byte *entry = entries + i * 0x90;
        *(void **)(entry + 0x38) = h2_native_physics_resolve(definition,
            *(dword *)(entry + 0x38));
    }

    count = *(long *)(definition + 0x58);
    entries = *(byte **)(definition + 0x5c);
    for (long i = 0; i < count; i++)
        h2_native_physics_install(entries + i * 0x50 + 0x20,
            h2_native_physics_model_vtable_00455500);

    count = *(long *)(definition + 0x60);
    entries = *(byte **)(definition + 0x64);
    for (long i = 0; i < count; i++)
    {
        byte *entry = entries + i * 0x90;
        byte *child = entry + 0x20;
        byte *parent = entry + 0x40;
        h2_native_physics_install(child,
            h2_native_physics_model_vtable_00455ef0);
        *(dword *)(child + 0x08) = (dword)entry;
        h2_native_physics_install(parent,
            h2_native_physics_model_vtable_00455498);
        *(dword *)(parent + 0x0c) = (dword)child;
    }

    count = *(long *)(definition + 0x70);
    entries = *(byte **)(definition + 0x74);
    byte *vertex_storage = *(byte **)(definition + 0x7c);
    byte *value_storage = *(byte **)(definition + 0x84);
    long external_vertices = 0;
    long external_values = 0;
    for (long i = 0; i < count; i++)
    {
        byte *entry = entries + i * 0x100;
        byte *object = entry + 0x20;
        long vertex_count = *(long *)(object + 0x34);
        long vertex_capacity = *(long *)(object + 0x38) & 0x7fffffff;
        long value_count = *(long *)(object + 0xd8);
        byte *vertices = vertex_capacity <= 3 ? object + 0x40 : vertex_storage;
        if (vertex_capacity > 3 && vertices)
            vertices += external_vertices * 0x30;
        byte *values = value_storage;
        if (values)
            values += external_values * 0x10;
        h2_native_physics_install(object,
            h2_native_physics_model_vtable_00456008);
        *(dword *)(object + 0x08) = (dword)entry;
        *(dword *)(object + 0x30) = (dword)vertices;
        *(dword *)(object + 0xd4) = (dword)values;
        if (vertex_capacity > 3)
            external_vertices += vertex_count;
        external_values += value_count;
    }

    count = *(long *)(definition + 0x90);
    entries = *(byte **)(definition + 0x94);
    byte *child_storage = *(byte **)(definition + 0x9c);
    long external_children = 0;
    for (long i = 0; i < count; i++)
    {
        byte *object = entries + i * 0x38;
        long child_count = *(long *)(object + 0x10);
        long child_capacity = *(long *)(object + 0x14) & 0x7fffffff;
        byte *children = child_capacity <= 4 ? object + 0x18 : child_storage;
        if (child_capacity > 4 && children)
            children += external_children * 8;
        h2_native_physics_install(object,
            h2_native_physics_model_vtable_00455fc0);
        *(dword *)(object + 0x0c) = (dword)children;
        for (long child = 0; child < child_count; child++)
        {
            byte *item = children + child * 8;
            *(void **)item = h2_native_physics_resolve(definition,
                *(dword *)item);
            *(dword *)(item + 4) = 0;
        }
        if (child_capacity > 4)
            external_children += child_count;
    }

    count = *(long *)(definition + 0x108);
    entries = *(byte **)(definition + 0x10c);
    for (long i = 0; i < count; i++)
    {
        byte *parent = entries + i * 0x20;
        byte *child = parent + 0x14;
        dword child_value = *(dword *)(child + 0x08);
        void *resolved = h2_native_physics_resolve(definition,
            *(dword *)(parent + 0x0c));
        h2_native_physics_install(parent,
            h2_native_physics_model_vtable_00455f30);
        *(dword *)(parent + 0x0c) = (dword)resolved;
        *(dword *)(parent + 0x10) = (dword)child;
        h2_native_physics_install(child,
            h2_native_physics_model_vtable_00455f8c);
        *(dword *)(child + 0x08) = child_value;
    }
}
