#include "unknown_11c920.h"
#include "globals.h"
#include "main_globals.h"
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_session_options;
struct s_saved_game_header;
struct s_game_options;
struct s_18eaa0;
struct s_18f0a0;
struct s_input_state
{
    byte unknown00[0x10];
    byte values[0x38];
};
struct s_18e700
{
    long field_0;
    byte field_4[0x120 - 4];
    short field_120;
    byte field_122[0x264 - 0x122];
    long field_264;
    byte field_268[0x1118 - 0x268];
};
struct s_18e702
{
    union
    {
        s_18e700 field_0;
        double field_8;
    };
};
struct s_18e701
{
    byte field_0[0x2f];
    bool field_2f;
};
extern s_input_state g_4e61dc[3];
extern bool g_4ed39c;
extern bool g_4ed39d;
extern bool g_4ed39e;
extern long g_4ed294;
extern long g_51ea10;
extern s_game_options g_4ed3a8;
long g_4ed3a4;
bool g_55e757;

void function_18eb80(void);
void function_18ee60(void);
void function_18ef00(s_saved_game_header const *arg_0);
void function_18f170(s_game_options *arg_0, long arg_1);
void function_12bf00(void);
bool main_play_intro_movie(void);
bool window_manager_has_listed_dialog(void);
void function_14bac0(void);
bool __stdcall function_11c1b0(short arg_0, bool arg_1);
void function_137da0(void);
void function_137d00(void);
void __stdcall function_137ca0(s_game_options const *arg_0);
void function_137e40(void);
void function_d5560(bool arg_0);
void function_c09c0(long arg_0);
void function_158530(void);
void game_launch_file_wake_script(void);
void function_200d80(void);
bool function_18eaa0(s_18eaa0 const *arg_0);
void __stdcall function_18f0a0(s_18f0a0 const *arg_0);
void __stdcall function_1483c3(long arg_0);
void function_18efa0(void);
bool __stdcall function_18e8b0(s_session_options const *arg_0);

#pragma inline_depth(0)
// @retail 0x18f000
bool function_18f000(s_18e700 const *arg_0)
{
    bool local_0 = false;
    function_137ca0((s_game_options const *)arg_0);
    function_d5560(false);
    function_c09c0(g_4e6948->state);
    function_137e40();
    short local_1 = arg_0->field_120;
    if (function_11c1b0(local_1, true))
    {
        g_4e6948->flag1120 = true;
        if (g_4cf770 && !((s_18e701 *)g_4cf77c)->field_2f)
            ((s_18e701 *)g_4cf77c)->field_2f = true;
        function_158530();
        game_launch_file_wake_script();
        long local_2 = g_4e6948->mode;
        long local_3 = g_4e6948->state;
        if (local_2 != 4 && (local_3 == 1 || local_3 == 2))
            function_200d80();
        local_0 = true;
    }
    else
        function_137d00();
    return local_0;
}
#pragma inline_depth(255)

#pragma inline_depth(0)
// @retail 0x18efa0
void function_18efa0(void)
{
    s_18e700 local_0;
    function_18eb80();
    if (!g_55e757)
    {
        g_55e757 = true;
        function_18f170((s_game_options *)&local_0, 0);
        function_18e8b0((s_session_options *)&local_0);
        g_55e757 = false;
    }
    else
    {
        function_18ee60();
        function_12bf00();
    }
}
#pragma inline_depth(255)

#pragma inline_depth(0)
// @retail 0x18e8b0
bool __stdcall function_18e8b0(s_session_options const *arg_0)
{
    bool local_0 = false;
    s_session_options const *const *local_9 = &arg_0;
    function_18eb80();
    s_session_options const *local_1 = *(s_session_options const *volatile *)local_9;
    main_globals.switch_structure_bsp = false;
    main_globals.reset_map = false;
    main_globals.unknown6f = false;
    main_globals.unknown72 = false;
    main_globals.save_map = false;
    main_globals.quit_game = false;
    function_18ef00((s_saved_game_header const *)local_1);
    if (!local_1)
    {
        function_18ee60();
        local_0 = true;
        goto local_2;
    }
    memset(g_4e61dc, 0, 0x120);
    if (function_18eaa0((s_18eaa0 const *)local_1) && function_18f000((s_18e700 const *)local_1))
    {
        if (((s_18e700 const *)local_1)->field_0 == 1)
            function_18f0a0((s_18f0a0 const *)local_1);
        else if (((s_18e700 const *)local_1)->field_0 == 3 && !main_play_intro_movie())
            function_1483c3(((s_18e700 const *)local_1)->field_264);
        local_0 = true;
        goto local_2;
    }
    else
        function_18efa0();
local_2:
    return local_0;
}
#pragma inline_depth(255)

#pragma inline_depth(0)
// @retail 0x18e700
void function_18e700(void)
{
    s_18e702 local_0;
    g_4ed39c = true;
    function_14bac0();
    local_0.field_0 = *(s_18e700 *)((byte *)g_4e6948 + 8);
    if (local_0.field_0.field_120 == g_4686c4)
        function_137da0();
    else
        function_11c1b0(NONE, true);
    function_137d00();
    memset(g_4e61dc, 0, 0x120);
    bool local_1 = function_18f000(&local_0.field_0);
    g_4ed39c = false;
    if (!local_1)
        function_18efa0();
}
#pragma inline_depth(255)

#pragma inline_depth(0)
// @retail 0x18e810
void function_18e810(void)
{
    bool local_0 = g_4ed39d;
    if (g_4ed39d && !g_4ed39e && g_51ea10 > 0)
        local_0 = false;
    if (!window_manager_has_listed_dialog() && local_0)
    {
        if (g_4ed39e)
        {
            if (g_4ed294 != 5)
            {
                function_18eb80();
                main_globals.switch_structure_bsp = false;
                main_globals.reset_map = false;
                main_globals.unknown6f = false;
                main_globals.unknown72 = false;
                main_globals.save_map = false;
                main_globals.quit_game = false;
                function_18ef00(NULL);
                function_18ee60();
            }
        }
        else
            function_18e8b0((s_session_options const *)&g_4ed3a8);
        g_4ed39d = false;
        g_4ed39e = false;
        g_4ed3a4 = 0;
    }
}
#pragma inline_depth(255)
