#pragma once

#include "unknown_19d220.h"

struct s_54e49c
{
	long index;
	union
	{
		s_game_variant variant;
		byte variant_bytes[sizeof(s_game_variant)];
	};
};

extern s_54e49c g_54e49c_storage;

#define g_54e49c (g_54e49c_storage.index)
#define g_54e4a0 (g_54e49c_storage.variant)
