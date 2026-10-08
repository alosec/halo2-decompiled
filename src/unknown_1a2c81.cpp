// @flags /O1 /Gr
/* UNKNOWN_1A2C81.CPP: the controller a widget belongs to */

#include "unknown_11c920.h"
#include "screen_widgets.h"

/* inline (kept out of line): retail's callers treat ecx and edx as
   clobbered across the call (0x23296e) */
// @retail 0x1a2c81
#ifndef H2_NATIVE_PORT
inline
#endif
long c_class_1a2c81::get_controller_index()
{
	word flags = user_flags;

	if (flags & 1)
	{
		return 0;
	}
	if (flags & 2)
	{
		return 1;
	}
	if (flags & 4)
	{
		return 2;
	}
	return (flags & 8) ? 3 : NONE;
}
