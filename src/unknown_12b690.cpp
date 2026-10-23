#include "unknown_440dd8.h"
// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "main_globals.h"

void function_12b070(void);
void function_137c20(void);
void function_12b450(void);
void function_18eb80(void);
void effects_dispose(void);
void function_067eb0(void);
void object_list_dispose(void);
void function_10a9d0(void);
void function_123bf0(void);
void function_125600(void);
void function_1320b0(void);
void scenario_ramps_dispose(void);
void function_141c0(void);
void function_14783f(void);
void function_14b560(void);
void function_155ed0(void);
void function_17b850(void);
void function_17d2a0(void);
void function_1c2600(void);
void function_1c79c0(void);
void function_1eb5e0(void);
void function_1ec470(void);
void function_20af50(void);
void function_215730(void);
void function_217c20(void);
void function_2218f0(void);
void function_2b5c0(void);
void function_8dd70(void);
void function_b68c0(void);
void level_handle_tables_dispose(void);
void object_looping_sounds_dispose(void);
void script_nodes_dispose(void);
void transport_dispose(void);

PRIVATE void function_12b691(void)
{
}

struct s_440ddc
{
    void (*field_0)(void);
    dword field_4[8];
};

#define g_440ddc (*( s_440ddc const (*)[68])((byte const *)g_440dd8 + 4))

#pragma inline_depth(0)
// @retail 0x12b690
void function_12b690(void)
{
    main_globals.unknown28 = 1;
    function_12b070();
    function_137c20();
    while (!main_globals.unknown2a[1])
        function_12b450();
    function_18eb80();
    g_4e6948 = NULL;
    s_440ddc const *local_0 = &g_440ddc[67];
    do
    {
        local_0->field_0();
        local_0--;
    }
    while ((long)local_0 >= (long)g_440ddc);
}
#pragma inline_depth(255)
