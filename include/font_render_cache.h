#pragma once

#include "unknown_11c920.h"

struct s_font_render_entry
{
	long character_index;
	union
	{
		long unknown04;
		struct { short x, y; } position;
	};
};

#include "glyphs_storage.h"
