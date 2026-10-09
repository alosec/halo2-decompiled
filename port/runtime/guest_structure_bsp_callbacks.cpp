#include "unknown_11c920.h"

void arena_initialize_for_new_structure_bsp(void);
void function_b6bb0(void);
void function_226a0(void);
void function_183ee0(void);
void function_16f120(void);
void decals_update_locations(void);
void contrails_update_locations(void);
void function_175d70(void);
void function_03f2b0(void);
void sound_update_locations(void);
void object_looping_sounds_initialize_for_new_structure_bsp(void);
void function_d4e30(void);
void function_1c7bb0(void);
void function_1c2690(void);
void function_14b6a0(void);
void function_210df0(void);

extern "C" void h2_native_structure_bsp_idle(void)
{
}

extern "C" void (*const h2_native_structure_bsp_switch_callbacks[68])(void) =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    arena_initialize_for_new_structure_bsp,
    NULL, NULL, h2_native_structure_bsp_idle,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    function_b6bb0,
    NULL, NULL, NULL,
    function_226a0,
    NULL, NULL,
    function_183ee0,
    NULL,
    function_16f120,
    decals_update_locations,
    NULL,
    contrails_update_locations,
    function_175d70,
    function_03f2b0,
    NULL, NULL,
    sound_update_locations,
    NULL,
    object_looping_sounds_initialize_for_new_structure_bsp,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    function_d4e30,
    function_1c7bb0,
    function_1c2690,
    function_14b6a0,
    h2_native_structure_bsp_idle,
    h2_native_structure_bsp_idle,
    function_210df0,
    NULL,
    h2_native_structure_bsp_idle,
    NULL,
    h2_native_structure_bsp_idle,
};

static_assert(sizeof(h2_native_structure_bsp_switch_callbacks) == 68 * sizeof(dword),
    "structure-BSP callback table layout");
