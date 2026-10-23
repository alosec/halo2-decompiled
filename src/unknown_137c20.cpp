// @flags /O2 /Gr
/* UNKNOWN_137C20.CPP: allocates the 0x1200-byte game globals from the game
   state, sets the floating-point control word, then runs every subsystem's
   first callback */

#include "unknown_11c920.h"
#include "globals.h"
#include "allocator_storage.h"
#include "crc.h"
#include "unknown_440dd8.h"
#include <string.h>
#include <float.h>

void function_123b30(void);

// @retail 0x137c20
void function_137c20(void)
{
    function_123b30();
    long size = 0x1200;
    byte *result = game_state_globals.base_address + game_state_globals.cpu_allocation_size;
    game_state_globals.cpu_allocation_size += size;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
    memset(result, 0, 0x1200);
    g_4e6948 = (s_game_options_view *)result;
    g_4e6948->index = NONE;
    _control87(0x9001f, 0xfffff);
    for (dword i = 0; i < 68; i++)
        g_440dd8[i].field_0();
}
