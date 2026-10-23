// @flags /O2 /Ob1 /arch:SSE /Gr /GL-
/* UNKNOWN_059570.CPP: the session manager's state as the menus read it:
   the matchmaking state's own answer while matchmaking, 5 in the next
   state, 0 otherwise. */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"

long function_70570(c_session_state_matchmaking *state);

// @retail 0x59570
long function_59570(void)
{
	long result = 0;
	switch (g_527330.state)
	{
	case 6: return function_70570(&g_527330.state_matchmaking);
	case 7: result = 5; break;
	}
	return result;
}
