#include "unknown_11c920.h"
#include "globals.h"
#include "network_message_types.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

class c_class_6a600;
struct s_1870a0
{
    byte field_0[0x24];
};
struct s_1870a1
{
    byte field_0[8];
    long field_8;
};
void __stdcall function_186ab0(long arg_0, real arg_1, real arg_2, s_player_action *arg_3);
void function_69040(c_class_6a600 *arg_0, s_player_action *arg_1, dword arg_2);
long function_14de70(long local_player_index);

// @retail 0x1870a0
void __stdcall function_1870a0(real arg_0, real arg_1)
{
    s_player_action actions[4];
    dword mask = 0;

    for (long i = 0; i < 4; i++)
    {
        long player_index = function_14de70(i);
        if (player_index != NONE)
        {
            function_186ab0(player_index, arg_0, arg_1, &actions[i]);
            mask |= 1 << i;
        }
    }
    if (!g_4cf772 && ((s_1870a1 *)g_4cf77c)->field_8)
        function_69040((c_class_6a600 *)g_4cf77c, actions, mask);
}
