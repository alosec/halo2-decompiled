// @flags /O1 /Oi /Gr
/* UI_DEFINITION_LOOKUPS.CPP: lookups in the user interface globals
   tag: a subtitle's text and the range that holds a value
   (ui_definition.h) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_234c64.h"
#include "ui_definition.h"

void function_1a0180(long tag_index, long string_handle, word *buffer);

/* a subtitle type and the handle of its string */
struct s_subtitle_string
{
	long type;
	long string_handle;
};

/* a subtitle's text, by type, from the user interface globals' string list */
// @retail 0x1496f6
void ui_definition_get_subtitle_text(long subtitle_type, word *buffer)
{
	s_type_954545 *ui_definition = ui_definition_get();
	s_subtitle_string strings[23] =
	{
		{ 0, 0 },
		{ 1, 0xf0006ce },
		{ 2, 0x110006cf },
		{ 3, 0x100006d0 },
		{ 4, 0x130006d1 },
		{ 5, 0x110006d2 },
		{ 6, 0xe0006d3 },
		{ 7, 0x180006d4 },
		{ 8, 0x90006d5 },
		{ 9, 0x80006d6 },
		{ 10, 0x1a0006d7 },
		{ 11, 0x180006d8 },
		{ 12, 0x80006d9 },
		{ 13, 0x80006da },
		{ 14, 0x230006db },
		{ 15, 0x250006dc },
		{ 16, 0x240006dd },
		{ 17, 0x1c0006de },
		{ 18, 0x230006df },
		{ 19, 0x1c0006e0 },
		{ 20, 0x1c0006e1 },
		{ 21, 0x180006e2 },
		{ 22, 0x40006e3 }
	};

	if (subtitle_type >= 0 && subtitle_type < 23 && ui_definition && ui_definition->subtitle_string_list_index != NONE)
	{
		function_1a0180(ui_definition->subtitle_string_list_index, strings[subtitle_type].string_handle, buffer);
	}
}

/* the user interface globals' range that holds a value */
// @retail 0x149ead
long ui_definition_find_range(long value)
{
	long result = NONE;
	s_type_954545 *ui_definition = ui_definition_get();

	if (ui_definition)
	{
		for (long index = 0; index < ui_definition->range_count; index++)
		{
			s_type_954545::s_user_interface_globals_range *range = &ui_definition->ranges[index];

			if ((value < range->lower ? range->lower : (value > range->upper ? range->upper : value)) == value)
			{
				result = index;
				break;
			}
		}
	}
	return result;
}
