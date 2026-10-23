#include "unknown_11c920.h"
#include "globals.h"
// @flags /O2 /Gr
#include "launch_stack_storage.h"

class c_class_6a600;
struct s_18eb80
{
    byte field_0[0x2f];
    bool field_2f;
};
extern byte g_4cf77a;
extern bool g_4ed39c;
extern long g_4ed294;
void __stdcall function_693a0(c_class_6a600 *arg_0);
bool __stdcall function_11c1b0(short arg_0, bool arg_1);
void function_137d00(void);
void function_18edb0(void);
void function_11be50(void);
void texture_cache_dispose_from_old_map(void);

// @retail 0x18eb80
void function_18eb80(void)
{
    if (g_4e6948 && g_4e6948->flag1120)
    {
        if (g_4cf770 && ((s_18eb80 *)g_4cf77c)->field_2f && !g_4cf77a && !g_4ed39c)
        {
            s_18eb80 *local_0 = (s_18eb80 *)g_4cf77c;
            function_693a0((c_class_6a600 *)g_4cf77c);
            local_0->field_2f = false;
        }
        function_11c1b0(NONE, true);
        function_137d00();
    }
    switch (g_4ed294)
    {
    case 0:
        return;
    case 2:
        function_18edb0();
        function_11be50();
        break;
    case 4:
        function_18edb0();
        function_11be50();
        break;
    case 5:
        texture_cache_dispose_from_old_map();
        g_4e6420--;
        g_4ed294 = 0;
        return;
    default:
        __assume(0);
    }
    g_4e6420--;
    g_4ed294 = 0;
}
