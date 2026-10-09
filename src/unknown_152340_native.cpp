// Native source split of recovered player census refresh at retail 0x152340.
// The matching Xbox definition remains in unknown_1502e0.cpp, whose
// unrelated target-selection code uses x87 inline assembly.

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>

struct s_tail_player_152340_native
{
    short salt;
    word flags;
    byte unknown04[0x2c - 4];
    long unit_index;
};

struct s_player_census_152340_native
{
    byte unknown00[4];
    bool all_without_units;
    bool any_available;
};

static_assert(offsetof(s_tail_player_152340_native, unit_index) == 0x2c, "player unit offset");
static_assert(offsetof(s_player_census_152340_native, all_without_units) == 4, "census empty flag offset");
static_assert(offsetof(s_player_census_152340_native, any_available) == 5, "census available flag offset");

extern void *g_4e8c20;
extern s_record_pool *g_4e8c24;

void function_152340(void)
{
    s_player_census_152340_native *census = (s_player_census_152340_native *)g_4e8c20;
    census->all_without_units = true;
    census->any_available = false;

    s_record_pool_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    s_tail_player_152340_native *player;
    while ((player = (s_tail_player_152340_native *)data_iterator_next_inlined(&iterator)) != 0)
    {
        if (player->unit_index != NONE || (player->flags & 8))
            census->all_without_units = false;
        else
            census->any_available = true;
    }
}
