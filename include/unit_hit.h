#pragma once
#include "unknown_11c920.h"

struct s_unit_melee_hit
{
	long object_index;
	long unknown04;
	long unknown08;
	long unknown0c;
	short material_index;
	byte unknown12[2];
	real unknown14;
	bool unknown18;
};

typedef char assert_unit_hit_size[(sizeof(s_unit_melee_hit) == 0x1c) ? 1 : -1];
