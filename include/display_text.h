/* DISPLAY_TEXT.H: the text 0x13cf00 shows from the scenario's string list
   (the state at +0x18 of g_510c50), as display_text_get_alpha (0x13cbb0)
   reads it */
#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

#include <stddef.h>
#include "unknown_11c920.h"

/* the shown string, and how long it has been shown (it fades in over the
   first 1.5) */
struct s_display_text
{
	byte unknown00[4];
	real age;
};
typedef char assert_display_text_age[(offsetof(s_display_text, age) == 0x4) ? 1 : -1];

#endif
