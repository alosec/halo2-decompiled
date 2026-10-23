// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_157450.h"
#include "unknown_07f720.h"

// @retail 0x15f330
bool function_15f330(s_player_appearance const *appearance, short team_index, color3f *colors)
{
	long color_team = NONE;
	bool use_default = team_index == NONE;
	bool result = false;

	if (game_engine_get() || (((byte const *)appearance)[7] & 8))
	{
		if (function_x340af0() && team_index >= 0 && team_index < 8)
		{
			color_team = team_index;
		}
		result = true;
	}
	function_7f790(color_team, use_default, appearance, colors);
	return result;
}
