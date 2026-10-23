// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>

class c_class_93590;
void network_link_receive(c_class_93590 *link);
void function_8df50(void);
void function_8dfc0(void);
void main_time_wait_for_vblank(void);
void function_13f10(long a, long b);
extern bool g_4d8ba0;
extern c_class_93590 *g_510560;

typedef void (__stdcall *t_aux_view_callback)(void *context);
void function_2ca00(point3f const *position, t_aux_view_callback callback, void *context);
void __stdcall function_2233a0(real const *arg_0);

static __forceinline void draw_progress_frame(point3f const *color, real progress)
{
    function_2ca00(color, (t_aux_view_callback)function_2233a0, &progress);
}

/* keeps the network and the screen alive while showing a progress value */
// @retail 0x12b6f0
void function_12b6f0(real progress)
{
    if (g_4d8ba0)
    {
        g_510548 = 1;
        g_51054c = GetTickCount();
        network_link_receive(g_510560);
        g_510548 = 0;
        g_51054c = GetTickCount();
    }
    function_8df50();
    function_8dfc0();
    point3f color;
    color.x = 12.0f / 255.0f;
    color.y = 17.0f / 255.0f;
    color.z = 32.0f / 255.0f;
    draw_progress_frame(&color, progress);
    main_time_wait_for_vblank();
    function_13f10(0, 0);
}
