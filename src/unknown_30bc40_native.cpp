#ifdef H2_NATIVE_PORT

#include "unknown_1cec30.h"

#include <stddef.h>

void __cdecl function_2d91d0(void *array, long element_size);
bool __cdecl function_279740(long value);

struct s_havok_simulation_island_active_view
{
    byte unknown00[0x24];
    long storage_index;
    byte unknown28;
    byte active;
    byte unknown2a[0x30 - 0x2a];
    byte dispatching;
    byte unknown31[3];
    long state34;
    long state38;
    hkEntity **entities;
    long entity_count;
};

template<typename T>
struct s_havok_array_view
{
    T *data;
    long size;
    dword capacity_and_flags;
};

class c_havok_world_activation_listener
{
public:
    virtual void function_0(void) = 0;
    virtual void simulation_island_activated(c_havok_body_group *island) = 0;
};

class c_havok_entity_activation_listener
{
public:
    virtual void function_0(void) = 0;
    virtual void function_1(void) = 0;
    virtual void entity_activated(hkEntity *entity) = 0;
};

class c_havok_cache_owned_reference
{
public:
    virtual void deleting_destructor(dword flags) = 0;
};

struct s_havok_entity_activation_view
{
    byte unknown00[0x70];
    s_havok_array_view<c_havok_entity_activation_listener *> listeners;
};

struct s_havok_world_activation_view
{
    byte unknown00[8];
    s_havok_array_view<c_havok_body_group *> active_islands;
    s_havok_array_view<c_havok_body_group *> inactive_islands;
    byte unknown20[0x78 - 0x20];
    s_havok_array_view<c_havok_world_activation_listener *> listeners;
    byte unknown84[0x108 - 0x84];
    s_havok_array_view<hkRigidBody *> pending_bodies;
};

struct s_havok_rigid_body_reference_view
{
    byte unknown00[6];
    short reference_count;
};

static_assert(offsetof(hkRigidBody, m_world) == 8,
    "hkRigidBody world offset must match retail");
static_assert(offsetof(hkRigidBody, m_simulation_island) == 0x44,
    "hkRigidBody simulation-island offset must match retail");
static_assert(offsetof(s_havok_rigid_body_reference_view, reference_count) == 6,
    "Havok reference-count offset must match retail");
static_assert(offsetof(s_havok_simulation_island_active_view, storage_index) == 0x24,
    "Havok simulation-island storage index must match retail");
static_assert(offsetof(s_havok_simulation_island_active_view, active) == 0x29,
    "Havok simulation-island active offset must match retail");
static_assert(offsetof(s_havok_simulation_island_active_view, dispatching) == 0x30,
    "Havok simulation-island dispatch flag must match retail");
static_assert(offsetof(s_havok_simulation_island_active_view, entities) == 0x3c,
    "Havok simulation-island entity array must match retail");
static_assert(offsetof(s_havok_entity_activation_view, listeners) == 0x70,
    "Havok entity-listener array must match retail");
static_assert(offsetof(s_havok_world_activation_view, active_islands) == 8,
    "Havok active-island array must match retail");
static_assert(offsetof(s_havok_world_activation_view, inactive_islands) == 0x14,
    "Havok inactive-island array must match retail");
static_assert(offsetof(s_havok_world_activation_view, listeners) == 0x78,
    "Havok world-listener array must match retail");
static_assert(offsetof(s_havok_world_activation_view, pending_bodies) == 0x108,
    "Havok pending-body array must match retail");

template<typename T>
static void compact_null_listeners(s_havok_array_view<T *> *listeners)
{
    for (long i = listeners->size - 1; i >= 0; --i)
    {
        if (!listeners->data[i])
        {
            --listeners->size;
            for (long j = i; j < listeners->size; ++j)
                listeners->data[j] = listeners->data[j + 1];
        }
    }
}

// @retail 0x318540
static void havok_world_activation_notify(
    s_havok_world_activation_view *world,
    s_havok_simulation_island_active_view *island)
{
    for (long i = world->listeners.size - 1; i >= 0; --i)
    {
        c_havok_world_activation_listener *listener = world->listeners.data[i];
        if (listener)
            listener->simulation_island_activated((c_havok_body_group *)island);
    }
    compact_null_listeners(&world->listeners);

    for (long i = 0; i < island->entity_count; ++i)
    {
        hkEntity *entity = island->entities[i];
        s_havok_array_view<c_havok_entity_activation_listener *> *listeners =
            &((s_havok_entity_activation_view *)entity)->listeners;
        for (long j = listeners->size - 1; j >= 0; --j)
        {
            c_havok_entity_activation_listener *listener = listeners->data[j];
            if (listener)
                listener->entity_activated(entity);
        }
        compact_null_listeners(listeners);
    }
}

// @retail 0x30c560
static void havok_world_pending_body_remove(
    s_havok_world_activation_view *world,
    hkRigidBody *body)
{
    if (world->pending_bodies.size > 0 && !world->pending_bodies.data)
        return;

    for (long i = 0; i < world->pending_bodies.size; ++i)
    {
        if (world->pending_bodies.data[i] == body)
        {
            s_havok_rigid_body_reference_view *reference =
                (s_havok_rigid_body_reference_view *)body;
            if (reference->reference_count <= 0)
                return;

            long found = NONE;
            for (long j = 0; j < world->pending_bodies.size; ++j)
            {
                if (world->pending_bodies.data[j] == body)
                {
                    found = j;
                    break;
                }
            }
            if (found == NONE)
                return;

            --world->pending_bodies.size;
            world->pending_bodies.data[found] =
                world->pending_bodies.data[world->pending_bodies.size];

            --reference->reference_count;
            if (reference->reference_count == 0)
            {
                if (function_279740((long)body))
                    reference->reference_count = 1;
                else
                    ((c_havok_cache_owned_reference *)body)->deleting_destructor(1);
            }
        }
    }
}

// @retail 0x30ccd0
static bool havok_world_simulation_island_activate(
    s_havok_world_activation_view *world,
    s_havok_simulation_island_active_view *island)
{
    long inactive_count = world->inactive_islands.size;
    long storage_index = island->storage_index;
    if (inactive_count <= 0 || storage_index < 0 || storage_index >= inactive_count ||
        world->active_islands.size < 0 || world->listeners.size < 0 ||
        island->entity_count < 0 || !world->inactive_islands.data ||
        !world->inactive_islands.data[inactive_count - 1] ||
        (world->active_islands.size > 0 && !world->active_islands.data) ||
        (world->listeners.size > 0 && !world->listeners.data) ||
        (island->entity_count > 0 && !island->entities))
        return false;
    for (long i = 0; i < island->entity_count; ++i)
    {
        s_havok_entity_activation_view *entity =
            (s_havok_entity_activation_view *)island->entities[i];
        if (!entity || entity->listeners.size < 0 ||
            (entity->listeners.size > 0 && !entity->listeners.data))
            return false;
    }

    if (world->active_islands.size ==
        (long)(world->active_islands.capacity_and_flags & 0x7fffffffU))
        function_2d91d0(&world->active_islands, sizeof(c_havok_body_group *));
    if (!world->active_islands.data)
        return false;

    world->active_islands.data[world->active_islands.size++] =
        (c_havok_body_group *)island;

    c_havok_body_group *moved = world->inactive_islands.data[inactive_count - 1];
    world->inactive_islands.data[storage_index] = moved;
    ((s_havok_simulation_island_active_view *)moved)->storage_index = storage_index;
    --world->inactive_islands.size;

    island->storage_index = world->active_islands.size - 1;
    island->active = 1;
    island->state34 = 0;
    island->state38 = 0;
    havok_world_activation_notify(world, island);
    island->dispatching = 0;
    return true;
}

// @retail 0x30bc40
hkBool hkRigidBody::body_is_simulating(void) const
{
    hkBool result;
    result.m_bool = m_simulation_island
        ? ((s_havok_simulation_island_active_view *)m_simulation_island)->active
        : 0;
    return result;
}

// @retail 0x30bc60
void hkRigidBody::activate(void)
{
    if (!m_world)
        return;

    s_havok_world_activation_view *world =
        (s_havok_world_activation_view *)m_world;
    havok_world_pending_body_remove(world, this);

    s_havok_simulation_island_active_view *island =
        (s_havok_simulation_island_active_view *)m_simulation_island;
    if (island && !island->active && island->storage_index != NONE)
        havok_world_simulation_island_activate(world, island);
}

#endif
