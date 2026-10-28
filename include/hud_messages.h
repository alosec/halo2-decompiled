/* HUD_MESSAGES.H: the one-time hud messages a script shows by index
   (src/hud_messages.cpp) */

#ifndef HUD_MESSAGES_H
#define HUD_MESSAGES_H

#include "globals.h"

/* whether each message has been shown (unknown_29f5b0.cpp) */
extern bool g_4f55dc[16];

/* whether the message has been shown; outside game state 1 none has */
inline bool function_2ac270_get(long index)
{
	if (index >= 0 && index < 15 && g_4e6948->state == 1)
		return g_4f55dc[index];
	return false;
}

#endif
