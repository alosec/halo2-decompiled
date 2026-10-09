// Native source split of recovered AI actor unlink helpers at retail
// 0x203ed0 and 0x203fb0. Matching Xbox definitions remain in
// unknown_2003a0.cpp, whose unrelated AI routines use x86 inline assembly.

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"

struct s_squad_activity_flags_203cc0_native
{
    byte unknown00[2];
    word unused : 7;
    word active : 1;
    word remaining : 8;
};

bool function_1e13f0(long actor_index);

static s_actor_datum *next_actor_204390_native(s_squad_actor_iterator *iterator)
{
    if (*(volatile bool *)&g_4f55d0->active && iterator->next_actor_index != NONE)
    {
        s_actor_datum *actor = actor_datum_get(iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = actor->next_actor_index;
        return actor;
    }
    return 0;
}

bool function_204390(long squad_index)
{
    s_squad_actor_iterator iterator;
    if (g_4f55d0->active)
    {
        iterator.squad_index = squad_index;
        iterator.actor_index = NONE;
        iterator.next_actor_index = squad_index == NONE ?
            g_4f55d0->unknown14 : squad_get(squad_index)->first_actor_index;
    }
    while (next_actor_204390_native(&iterator))
    {
        if (function_1e13f0(iterator.actor_index))
            return true;
    }

    for (long index = g_4f55d0->unknown14; index != NONE; )
    {
        s_actor_datum *actor =
            &((s_actor_datum *)(*(byte *volatile *)&g_4f55f0->data))[index & 0xffff];
        long const source_squad = *(volatile long *)((byte *)actor + 0x34);
        if (source_squad == squad_index && function_1e13f0(index))
            return true;
        index = actor->next_actor_index;
    }
    return false;
}

static s_actor_datum *next_active_squad_actor_203cc0(s_squad_actor_iterator *iterator)
{
    while (g_4f55d0->active && iterator->next_actor_index != NONE)
    {
        long const current = iterator->next_actor_index;
        s_actor_datum *actor = actor_datum_get(current);
        iterator->actor_index = current;
        iterator->next_actor_index = actor->next_actor_index;
        if (*(bool *)((byte *)actor + 9))
            return actor;
    }
    return 0;
}

void function_203cc0(long squad_index)
{
    s_squad_activity_flags_203cc0_native *squad =
        (s_squad_activity_flags_203cc0_native *)squad_get(squad_index);
    squad->active = false;

    s_squad_actor_iterator iterator;
    if (g_4f55d0->active)
    {
        iterator.squad_index = squad_index;
        iterator.actor_index = NONE;
        iterator.next_actor_index = squad_get(squad_index)->first_actor_index;
    }
    s_actor_datum *actor;
    while ((actor = next_active_squad_actor_203cc0(&iterator)) != 0)
    {
        if (*(bool *)((byte *)actor + 9))
        {
            *(bool *)((byte *)actor + 9) = false;
            *(long *)((byte *)actor + 0x10) = g_510c54->game_time;
            (*(short *)((byte *)g_4f55d0 + 0x36a))--;
        }
    }
}

void function_203ed0(long actor_index, bool keep_count)
{
    if (!g_4f55d0->active)
        return;

    s_actor_datum *actor = actor_datum_get(actor_index);
    if (actor->squad_index == NONE)
        return;

    s_squad_datum *squad = squad_get(actor->squad_index);
    long *link = &squad->first_actor_index;
    long current = *link;
    while (current != actor_index)
    {
        link = &actor_datum_get(current)->next_actor_index;
        current = *link;
    }
    *link = actor->next_actor_index;
    if (!keep_count)
        squad->actor_count--;
    actor->next_actor_index = NONE;
    actor->squad_index = NONE;
    ((byte *)squad)[3] |= 1;
}

void function_203fb0(long actor_index)
{
    if (!g_4f55d0->active)
        return;

    s_actor_datum *actor = actor_datum_get(actor_index);
    long *link = &g_4f55d0->unknown14;
    long current = *link;
    while (current != actor_index)
    {
        link = &actor_datum_get(current)->next_actor_index;
        current = *link;
    }
    *link = actor->next_actor_index;
    actor->flag00a = false;
    actor->next_actor_index = NONE;
    actor->flag00b = false;
}
