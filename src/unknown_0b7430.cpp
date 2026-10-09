#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include <stddef.h>

// @flags /O2 /Gr /arch:SSE

struct s_location;
struct s_cluster_partition;

struct s_object_header_b7430
{
    short identifier;
    byte flags;
    byte type;
    byte unknown04[4];
    byte *object;
};
static_assert(sizeof(s_object_header_b7430) == 0x0c, "object header layout");

struct s_object_b7430
{
    long definition_index;
    dword flags;
    byte unknown08[0x14 - 0x08];
    long parent_index;
    byte unknown18[0x60 - 0x18];
    long first_cluster_reference;
    point3f position;
    vector3f forward;
    vector3f up;
    byte unknown88[0xd4 - 0x88];
    long simulation_entity_index;
};
static_assert(offsetof(s_object_b7430, parent_index) == 0x14, "object parent layout");
static_assert(offsetof(s_object_b7430, first_cluster_reference) == 0x60, "object cluster layout");
static_assert(offsetof(s_object_b7430, position) == 0x64, "object position layout");
static_assert(offsetof(s_object_b7430, forward) == 0x70, "object forward layout");
static_assert(offsetof(s_object_b7430, up) == 0x7c, "object up layout");
static_assert(offsetof(s_object_b7430, simulation_entity_index) == 0xd4, "object simulation layout");

extern void *g_4de2d4;
extern void *g_4de2e0;

void __stdcall function_bef30(long object_index, long remove, long add, long siblings, long own_flags);
void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference);
void function_b58c0(long index, dword mask);
void function_1c4a80(long object_index, long position_unchanged, long value);
void __stdcall function_bd020(long object_index);
void __stdcall function_b8600(long object_index, s_location const *location);

// @retail 0xb87b0
void __stdcall function_b87b0(long object_index)
{
    s_object_header_b7430 *headers = (s_object_header_b7430 *)g_4e0300->data;
    s_object_header_b7430 *header = &headers[object_index & 0xffff];
    s_object_b7430 *object = (s_object_b7430 *)header->object;

    function_bef30(object_index, true, false, false, false);
    s_cluster_partition *partition = (s_cluster_partition *)((object->flags & 0x200) ? g_4de2e0 : g_4de2d4);
    function_1cae40(partition, object_index, &object->first_cluster_reference);
    object->flags &= ~0x100u;
    header->flags &= ~0x40u;
}

// @retail 0xb7430
bool __stdcall function_b7430(long object_index, point3f const *position,
    vector3f const *forward, vector3f const *up, s_location const *location,
    bool update_physics, bool update_biped, bool biped_value, bool preserve_partition)
{
    s_object_header_b7430 *headers = (s_object_header_b7430 *)g_4e0300->data;
    s_object_header_b7430 *header = &headers[object_index & 0xffff];
    s_object_b7430 *object = (s_object_b7430 *)header->object;
    bool was_partitioned = (object->flags & 0x100) != 0;
    bool result = true;

    if (was_partitioned && !preserve_partition)
        function_b87b0(object_index);

    if (position)
    {
        if (object->parent_index == NONE &&
            (position->x != position->x || position->x < -32768.0f || position->x > 32768.0f ||
             position->y != position->y || position->y < -32768.0f || position->y > 32768.0f ||
             position->z != position->z || position->z < -32768.0f || position->z > 32768.0f))
        {
            result = false;
        }
        else
        {
            object->position = *position;
            if (object->simulation_entity_index != NONE)
                function_b58c0(object->simulation_entity_index, 2);
        }
    }

    if (forward)
    {
        object->forward = *forward;
        object->up = *up;
        if (object->simulation_entity_index != NONE)
            function_b58c0(object->simulation_entity_index, 4);
    }

    if (update_biped)
        function_1c4a80(object_index, position == NULL, biped_value);
    if (update_physics)
        function_bd020(object_index);
    if (was_partitioned && !preserve_partition)
        function_b8600(object_index, location);

    return result;
}
