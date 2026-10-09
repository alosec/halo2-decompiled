// Native source split of recovered netgame-entry detach at retail 0x15b220.
// The matching Xbox definition remains in unknown_157450.cpp, whose broader
// game-engine translation unit has unrelated x87 code.

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_157450.h"
#include <stddef.h>

struct s_engine_object_15b220_native
{
    byte unknown00[0xaf];
    char netgame_entry;
};

struct s_engine_object_header_15b220_native
{
    byte unknown00[8];
    s_engine_object_15b220_native *object;
};

static_assert(offsetof(s_engine_object_15b220_native, netgame_entry) == 0xaf,
    "netgame entry offset");
static_assert(offsetof(s_engine_object_header_15b220_native, object) == 8,
    "engine object pointer offset");

void function_15b220(long object_index, long entry_index)
{
    s_engine_object_15b220_native *object =
        ((s_engine_object_header_15b220_native *)g_4e0300->data)[object_index & 0xffff].object;
    if (g_4e6948->mode != 4)
        function_xaee93d()->netgame_entries[entry_index].index = NONE;
    object->netgame_entry = NONE;
}
