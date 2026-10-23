#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include "unknown_19c1d0.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xtl.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_game_options;
struct s_clc_source;
struct s_packed_clc;
struct s_player_settings_snapshot;
struct s_tail_transfer_options;
struct s_18e960
{
    long field_0;
    byte field_4[4];
    double field_8;
    long field_10;
    long field_14;
    long field_18;
    char field_1c[0x104];
    byte field_120[0xd];
    bool field_12d;
    byte field_12e[0x1118 - 0x12e];
};
struct s_18e961
{
    byte field_0[0x340];
};
struct s_18e962
{
    union
    {
        s_18e960 field_0;
        double field_8;
    };
};
char const g_453060[] = "application/x-halo2campaign";
void packed_clc_write(s_clc_source const *arg_0, s_packed_clc *arg_1);
bool __stdcall function_b3ed0(s_packed_clc const *arg_0, long arg_1, char const *arg_2);
void function_152450(s_player_settings_snapshot *arg_0);
void function_152660(s_tail_transfer_options *arg_0);
bool network_session_interface_set_value4d08(long arg_0, long arg_1, char const *arg_2);
void function_18e790(s_game_options const *arg_0);
void __stdcall function_18f1c0(long arg_0);

#pragma inline_depth(0)
// @retail 0x18e960
void function_18e960(void)
{
    s_18e961 local_0;
    s_18e962 local_1;
    local_1.field_0 = *(s_18e960 *)((byte *)g_4e6948 + 8);
    if (g_467214 != NONE)
    {
        switch (online_task_get_logon_status(g_467214))
        {
        case 1:
            packed_clc_write((s_clc_source const *)&local_1.field_0, (s_packed_clc *)&local_0);
            function_b3ed0((s_packed_clc const *)&local_0, 0x33b, g_453060);
            break;
        }
    }
    if (local_1.field_0.field_14 != NONE)
    {
        long local_2 = function_19c440(local_1.field_0.field_14, local_1.field_0.field_18);
        if (local_2 != NONE)
        {
            s_entry_a *local_3 = function_19c270(local_1.field_0.field_14, local_2);
            if (local_3)
            {
                char const *local_4 = local_3->name;
                if (local_4)
                {
                    local_1.field_0.field_18 = local_2;
                    strncpy(local_1.field_0.field_1c, local_4, sizeof(local_1.field_0.field_1c));
                    local_1.field_0.field_1c[sizeof(local_1.field_0.field_1c) - 1] = 0;
                    local_1.field_0.field_12d = false;
                    long local_5 = time(NULL);
                    long local_6 = rand() ^ GetTickCount();
                    local_6 ^= local_5;
                    local_1.field_0.field_10 = local_6;
                    function_152450((s_player_settings_snapshot *)&local_1.field_0);
                    function_152660((s_tail_transfer_options *)&local_1.field_0);
                    if (!network_session_interface_set_value4d08(local_1.field_0.field_14, local_2, local_4))
                        function_18e790((s_game_options const *)&local_1.field_0);
                    return;
                }
            }
        }
    }
    function_18f1c0(7);
}
#pragma inline_depth(255)
