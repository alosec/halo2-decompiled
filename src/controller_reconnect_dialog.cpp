// @flags /O1 /Gr
/* CONTROLLER_RECONNECT_DIALOG.CPP: whether a controller's reconnect dialog
   may show (asked by the controller check of 0x18f9be) */

#include "unknown_11c920.h"
#include "globals.h"

bool function_13cb40(void);
bool game_loop_running();

/* true unless the flag 0x13cb40 reads is set, and always while the game
   loop runs the main menu (game options state 3) */
// @retail 0x2365f7
bool controller_reconnect_dialog_allowed(void)
{
	bool result = true;

	if (function_13cb40())
	{
		result = false;
	}
	if (game_loop_running() && g_4e6948->state == 3)
	{
		result = true;
	}
	return result;
}
