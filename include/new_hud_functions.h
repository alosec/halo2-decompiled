#ifndef NEW_HUD_FUNCTIONS_H
#define NEW_HUD_FUNCTIONS_H

/* the values the hud's widgets and shaders read by name
   (src/user_interface_hud_functions.cpp) */

#include <stddef.h>
#include "unknown_11c920.h"
#include "unknown_07f720.h"

/* the new hud globals (*g_510c4c), as the function evaluator reads them:
   the player the hud shows, the appearance it falls back on when there is
   none, and a value kept in 64ths */
struct s_new_hud_function_inputs
{
	byte unknown00[0x1b4];
	long player_index;
	byte unknown1b8[0x1c0 - 0x1b8];
	s_player_appearance appearance;
	short quantity;
};

typedef char assert_new_hud_player_offset[(offsetof(s_new_hud_function_inputs, player_index) == 0x1b4) ? 1 : -1];
typedef char assert_new_hud_appearance_offset[(offsetof(s_new_hud_function_inputs, appearance) == 0x1c0) ? 1 : -1];
typedef char assert_new_hud_quantity_offset[(offsetof(s_new_hud_function_inputs, quantity) == 0x1d0) ? 1 : -1];

real __stdcall new_hud_function_evaluate(long context, long name);
void new_hud_function_context_initialize(void);

#endif
