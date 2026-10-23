// @flags /O1 /Oi /Ob1 /Gr
/* SCREEN_PARAMETERS.CPP: filling in what a screen is loaded with
   (s_screen_parameters, screen_widgets.h) */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_19b516.h"
#include "screen_widgets.h"

/* fills in a screen's parameters: its type, the controllers it is for, the
   window channel and index it opens in and its load procedure. Given an id
   buffer, the parameters' id is copied out to it; otherwise the id is set
   to all NONE. */
// @retail 0x149f49
void function_149f49(s_message *message, word type, dword *id, short user_flags, long channel, long window_slot, long load_proc)
{
	s_screen_parameters *parameters = (s_screen_parameters *)message;

	parameters->type = type;
	parameters->user_flags = user_flags;
	parameters->a = channel;
	parameters->b = window_slot;

	if (id)
	{
		memcpy(id, parameters->id, sizeof(parameters->id));
	}
	else
	{
		memset(parameters->id, 0xff, sizeof(parameters->id));
	}

	parameters->load = (screen_load_proc)load_proc;
}
