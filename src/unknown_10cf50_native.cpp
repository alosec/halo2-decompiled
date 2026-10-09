// Native source split of recovered item-state query at retail 0x10cf50.
// The matching Xbox definition remains in items.cpp, whose unrelated item
// update code uses x87 inline assembly.

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>

struct s_item_10cf50_native
{
    byte unknown000[0x12c];
    byte flags_12c;
};

struct s_item_header_10cf50_native
{
    byte unknown00[8];
    s_item_10cf50_native *item;
};

static_assert(offsetof(s_item_10cf50_native, flags_12c) == 0x12c, "item state flags offset");
static_assert(offsetof(s_item_header_10cf50_native, item) == 8, "item header pointer offset");

extern s_record_pool *g_4e0300;

bool function_10cf50(long item_index)
{
    s_item_10cf50_native *item =
        ((s_item_header_10cf50_native *)g_4e0300->data)[item_index & 0xffff].item;
    return (item->flags_12c & 1) != 0 && (item->flags_12c & 2) == 0;
}
