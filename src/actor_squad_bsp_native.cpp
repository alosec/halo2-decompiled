#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include <stddef.h>

#if H2_NATIVE_PORT

static_assert(sizeof(s_squad_datum) == 0x98, "squad datum layout");
static_assert(offsetof(s_squad_datum, actor_count) == 8, "squad actor count layout");
static_assert(offsetof(s_squad_datum, first_actor_index) == 0x68, "squad actor list layout");
static_assert(sizeof(s_actor_datum) == 0x888, "actor datum layout");
static_assert(offsetof(s_actor_datum, next_actor_index) == 0x20, "actor link layout");
static_assert(offsetof(s_actor_datum, squad_index) == 0x30, "actor squad layout");

bool function_1e13f0(long actor_index);
bool function_1e11b0(long actor_index, bool active);
void function_1e1150(long actor_index, short team);
void function_1e22d0(long actor_index, bool conditional);
void function_203ed0(long actor_index, bool keep_count);
void function_204d30(s_squad_actor_iterator *iterator, long squad_index);

// @retail 0x203f60
void function_203f60(long actor_index)
{
    if (g_4f55d0->active)
    {
        s_actor_datum *actor = actor_datum_get(actor_index);
        actor->next_actor_index = g_4f55d0->unknown14;
        g_4f55d0->unknown14 = actor_index;
        actor->flag00a = true;
        *(long *)((byte *)actor + 0x14) = g_510c54->game_time;
        function_1e22d0(actor_index, false);
    }
}

// @retail 0x203bf0
bool function_203bf0(long squad_index)
{
    s_squad_datum *squad = squad_get(squad_index);
    if (*(short *)((byte *)squad + 0x7e) == g_4686c4)
    {
        if (!(((byte *)squad)[2] & 0x80))
        {
            s_squad_actor_iterator iterator;
            function_204d30(&iterator, squad_index);
            while (g_4f55d0->active && iterator.next_actor_index != NONE)
            {
                long actor_index = iterator.next_actor_index;
                s_actor_datum *actor = actor_datum_get(actor_index);
                iterator.next_actor_index = actor->next_actor_index;
                if (!function_1e11b0(actor_index, true))
                {
                    *(long *)((byte *)actor + 0x34) = squad_index;
                    function_203ed0(actor_index, false);
                    function_203f60(actor_index);
                }
            }
        }
        ((byte *)squad)[2] |= 0x80;
        *(long *)((byte *)squad + 0x78) = g_510c54->game_time;
    }
    return (bool)(((dword)((byte *)squad)[2] & 0x80) >> 7);
}

// @retail 0x203d70
void function_203d70(long actor_index, short squad_index, bool keep_team)
{
    if (!g_4f55d0->active)
        return;

    s_actor_datum *actor = actor_datum_get(actor_index);
    s_squad_datum *squad = squad_get((word)squad_index);
    if (function_1e13f0(actor_index))
    {
        if (*(short *)((byte *)squad_get((word)squad_index) + 0x7e) != g_4686c4)
        {
            s_squad_actor_iterator iterator;
            function_204d30(&iterator, squad_index);
            while (g_4f55d0->active && iterator.next_actor_index != NONE)
            {
                long previous_index = iterator.next_actor_index;
                s_actor_datum *previous = actor_datum_get(previous_index);
                iterator.next_actor_index = previous->next_actor_index;
                *(long *)((byte *)previous + 0x34) = squad_index;
                function_203ed0(previous_index, false);
                function_203f60(previous_index);
            }
            *(short *)((byte *)squad_get((word)squad_index) + 0x7e) = g_4686c4;
        }
        if (*((byte *)actor + 9) && !(((byte *)squad)[2] & 0x80))
            function_203bf0(squad_index);
    }

    *(long *)((byte *)actor + 0x34) = NONE;
    actor->next_actor_index = squad->first_actor_index;
    squad->first_actor_index = actor_index;
    actor->squad_index = squad_index;
    function_1e11b0(actor_index, (bool)((((dword)((byte *)squad)[2]) >> 7) & 1));
    short team = (signed char)((byte *)squad)[0x76];
    if (*(short *)((byte *)actor + 0x24) != team && !keep_team)
        function_1e1150(actor_index, team);
    squad->actor_count++;
    ((byte *)squad)[3] |= 1;
}

#endif
