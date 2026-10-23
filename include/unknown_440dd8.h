#pragma once
#include "unknown_11c920.h"

typedef void (*t_subsystem_440dd8_callback)();
typedef void (__stdcall *t_subsystem_440dd8_callback_18)(short);
typedef void (__stdcall *t_subsystem_440dd8_callback_20)(dword const *, dword const *, long);

struct s_subsystem_440dd8
{
    t_subsystem_440dd8_callback field_0;
    t_subsystem_440dd8_callback field_4;
    t_subsystem_440dd8_callback field_8;
    t_subsystem_440dd8_callback field_c;
    t_subsystem_440dd8_callback field_10;
    t_subsystem_440dd8_callback field_14;
    t_subsystem_440dd8_callback_18 field_18;
    t_subsystem_440dd8_callback_20 visible_clusters_changed;
    t_subsystem_440dd8_callback_20 active_clusters_changed;
};
typedef char assert_subsystem_440dd8_size[(sizeof(s_subsystem_440dd8) == 0x24) ? 1 : -1];
extern s_subsystem_440dd8 const g_440dd8[68];
