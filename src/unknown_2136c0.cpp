// @flags /O2 /Oi /Gr
#include "unknown_11c920.h"
#include "language.h"
#include "unknown_120d80.h"
#include <string.h>

struct s_cache_file
{
    HANDLE handle;
    byte unknown04[0x800];
};

struct s_cache_copy
{
    char path[256];
    long last_used;
    long state;
};

struct s_cache_copy_request
{
    char map_name[256];
    long priority;
};

struct s_cache_copy_progress
{
    long completed;
    dword total;
};

struct s_copy_header_view;
extern s_cache_file g_557c90[6];
extern long g_55aca8;
extern s_cache_copy g_55acac[8];
extern s_copy_header_view g_55b4f0;
extern dword g_55bcf0;
extern long g_55bcf4;
extern s_cache_copy_progress g_55bcf8;
extern long g_55bd00;
extern long g_55bd04;
extern long g_55bd08;
extern bool g_55bd0c;
extern long g_55bd10;
extern s_file_handle g_55bd14;
extern long g_55bd18;
extern long g_55bd1c;
extern bool g_55bd20;
extern char g_55bd21[0x103];
extern s_cache_copy_request g_55be24[2];
extern dword g_55c02c;

void __stdcall function_2138f0(long arg_0, long arg_1);
void function_213970(void);

#pragma optimize("s", on)
// score 131 (was 145): retail clears two storage blocks; separate globals require separate clears and
// cache-load scheduling differs. Tried language dirty first 145; version dirty first 145; current version
// tail 152; reset enabled first 145; scalar reset phase 145; forced language inline 169.
// Without the size pragma 172; stores in address order 145 (184 without the pragma); chained
// zero/NONE stores 145; language read at each use 143 (199 without the pragma). The two blocks
// are 0x557c90..0x55b4ec and 0x55b4f0..0x55c030; their globals are defined in three other files.
// Native progress/handle/both reset records 174/168/187; reversed clear phases 145;
// reversed with native progress/handle/both 175/168/187; native records after buffers 183.
// Separate allocation still prevents the two retail-wide clears; original body restored.
// Round 184: unsigned/native-int/immutable-signed/immutable-unsigned language results 145 each;
// query before all resets 142 (three-point gain dropped), after first bulk clears 144;
// requested-value-first comparisons and explicit void tail return 145/145. Original body restored;
// the separate allocations still prevent both complete bulk clears.
// Round 193: Native nonzero reset ownership at cache/copy/enabled phases, seven combinations 145 each;
// complete payload clears before scalar publication 131 kept, plain confirmed at 229 bytes. Two retail-wide
// blocks and expanded language query still differ.
// Round 193: On complete payloads: seven nonzero ownership combinations 131 each; NONE/zero/both chains 131
// each; query before all resets 128 (three-point gain dropped), between payloads and state 131. Retain only
// the complete-payload form at 131.
// @retail 0x2136c0
void function_2136c0(void)
{
    memset(g_557c90, 0, sizeof(g_557c90));
    memset(g_55acac, 0, sizeof(g_55acac));
    memset(&g_55b4f0, 0, 0x800);
    memset(g_55bd21, 0, sizeof(g_55bd21));
    memset(g_55be24, 0, sizeof(g_55be24));
    g_55bcf0 = 0;
    g_55bcf4 = 0;
    g_55bcf8.completed = 0;
    g_55bcf8.total = 0;
    g_55bd04 = 0;
    g_55bd08 = 0;
    g_55bd14.handle = NULL;
    g_55bd18 = 0;
    g_55bd1c = 0;
    g_55bd20 = false;
    g_55c02c = 0;
    g_55aca8 = NONE;
    g_55bd0c = true;
    g_55bd00 = NONE;
    g_55bd10 = NONE;
    long local_0 = get_current_language();
    if (global_preferences_globals.current.unknown18 != local_0)
    {
        function_2138f0(0, 6);
        global_preferences_globals.current.unknown18 = local_0;
        global_preferences_globals.dirty = true;
    }
    if (global_preferences_globals.current.unknown1e8 != 0x2651)
    {
        function_2138f0(0, 6);
        global_preferences_globals.current.unknown1e8 = 0x2651;
        global_preferences_globals.dirty = true;
    }
    function_213970();
}
#pragma optimize("", on)
