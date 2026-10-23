// @flags /O1 /Ob1 /Gr
/* UNKNOWN_2B01A2.CPP: a widget item helper that retail keeps out of line
   in all its callers (its own file, with /Ob1) */

#include "unknown_11c920.h"
#include "screen_widgets.h"

long ui_definition_find_range(long value);
void function_2b01b5(s_widget_item *item, short value);

/* sets the item's range: the user interface globals' range that holds the
   value */
// @retail 0x2b01a2
void function_2b01a2(long value, s_widget_item *item)
{
	function_2b01b5(item, (short)ui_definition_find_range(value));
}
