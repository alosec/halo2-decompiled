#ifndef MAIN_MENU_H
#define MAIN_MENU_H
/* MAIN_MENU.H: the data the main menu screen (src/main_menu.cpp) builds */

#include "unknown_11c920.h"

/* a button press the main menu makes for a controller to finish an Xbox Live
   choice the screen was waiting for: an input event (s_widget_event) with its
   source */
struct s_main_menu_selection
{
	long type;
	long controller_index;
	long param;
	short source;
};
typedef char assert_main_menu_selection_size[(sizeof(s_main_menu_selection) == 0x10) ? 1 : -1];

#endif
