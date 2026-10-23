// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_2accd0.h"

struct s_saved_game_file;
struct s_saved_game_file_location;
extern void *g_51ea14;
bool function_216800(long arg_1, void *arg_0);
bool function_216060(long arg_0, word const *arg_1);
void __stdcall function_217370(long arg_0, bool **arg_1);
s_saved_game_file *saved_game_file_new(s_saved_game_file *arg_0, long arg_1, s_saved_game_file_location const *arg_2);
s_saved_game_file *saved_game_file_new_from_id(s_saved_game_file *arg_0, long arg_1, wchar_t const *arg_2, s_saved_game_file_location const *arg_3, void *arg_4, bool *arg_5);
bool saved_game_file_copy_begin(void *arg_0, dword arg_1, bool arg_2, s_saved_game_file_task *arg_3);

struct s_2173d0
{
    char field_0[0x14];
    word field_14[0x12];
    long field_38;
    byte field_3c[4];
};

#pragma inline_depth(1)
// @retail 0x2173d0
bool function_2173d0(long arg_0, wchar_t const *arg_1, void *arg_2, dword arg_3, s_saved_game_file_task *arg_4)
{
    (void)&arg_2;
    (void)&arg_3;
    (void)&arg_4;
    bool local_0 = false;
    s_saved_game_file_task *local_2 = arg_4;
    s_2173d0 local_1;
    local_1.field_0[0] = 0;
    local_1.field_14[0] = 0;
    if (function_216800(arg_0, &local_1))
    {
        s_saved_game_file *local_5;
        if (function_216060(arg_0, (word const *)arg_1))
        {
            if (!g_51ea14)
                goto local_4;
            bool *local_3;
            function_217370(arg_0, &local_3);
            s_saved_game_file *local_6 = (s_saved_game_file *)((byte *)local_2 + 8);
            if (!local_6)
                goto local_4;
            local_5 = saved_game_file_new_from_id(local_6, arg_0, arg_1, (s_saved_game_file_location const *)&local_1, (byte *)g_51ea14 + 0x4bf00, local_3);
        }
        else
        {
            s_saved_game_file *local_6 = (s_saved_game_file *)((byte *)local_2 + 8);
            if (!local_6)
                goto local_4;
            local_5 = saved_game_file_new(local_6, arg_0, (s_saved_game_file_location const *)&local_1);
        }
        if (local_5)
            local_0 = saved_game_file_copy_begin(arg_2, arg_3, false, local_2);
    }
local_4:
    return local_0;
}
#pragma inline_depth(255)
