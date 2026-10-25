#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19d220.h"
#include <string.h>
#include <wchar.h>

// @flags /O2 /Ob1 /Gr

long function_215c00(long arg_0, wchar_t const *arg_1);
void __stdcall function_215e60(long arg_0);
void function_1a0180(long arg_0, long arg_1, word *arg_2);
bool function_19d650(s_game_variant *arg_0);
bool function_212ec0(s_game_variant *arg_0, long arg_1);

// score 386 (was 388): frame 0x464 and default/copy/text homes agree;
// inlined flag/value store scheduling and the shared tail differ. Inline depths
// one/two 388; global optimisation off 9513; size mode 1035; case nine first 389;
// terminator before copy 388; terminator via the copy result 398.
// Cases two/seven/eight first 414/407/409; zero inline depth 2361;
// separate save-assigned result 454 (fast).
// Case four first 388; five last 407; cases two/five adjacent 386;
// eight last 301 (executable sequence farther); high cases after one 423 (fast).
// @retail 0x212380
long function_212380(long arg_0, long arg_1, byte *arg_2)
{
    long const *local_0 = &arg_1;
    long local_2 = function_215c00(arg_0, (wchar_t const *)arg_2);
    if (local_2 != NONE)
    {
        s_game_variant local_6;
        switch (arg_0)
        {
        case 1:
            variant_defaults_initialize(&local_6, 0);
            break;
        case 2:
            variant_defaults_initialize(&local_6, 3);
            break;
        case 4:
            variant_defaults_initialize(&local_6, 1);
            break;
        case 5:
            variant_defaults_initialize(&local_6, 2);
            break;
        case 7:
            variant_defaults_initialize(&local_6, 4);
            break;
        case 8:
            variant_defaults_initialize(&local_6, 5);
            break;
        case 9:
            variant_defaults_initialize(&local_6, 6);
            break;
        default:
            __assume(0);
        }
        local_6.flags &= 0xfffe;
        function_19d650(&local_6);
        wcsncpy(local_6.name, (wchar_t const *)arg_2, 0x1f);
        local_6.name[0x1f] = 0;
        if (!function_212ec0(&local_6, local_2))
        {
            function_215e60(local_2);
            local_2 = NONE;
        }
    }
    return local_2;
}
