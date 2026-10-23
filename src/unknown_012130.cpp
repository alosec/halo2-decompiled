// @flags /O2 /Gr
/* UNKNOWN_012130.CPP: the builds whose game states this one can load */

#include "unknown_11c920.h"
#include <string.h>

/* the build version (game_state_lifecycle.cpp) */
extern char g_450698[0x20];

struct s_compatible_version
{
	char const *version;
	long unknown04;
};

s_compatible_version g_43e86c[1] =
{
	{ g_450698, 0x34000000 },
};

// @retail 0x12130
bool __stdcall version_is_compatible(char const *version)
{
	bool result = false;

	for (dword i = 0; i < sizeof(g_43e86c) / sizeof(g_43e86c[0]); i++)
	{
		if (!strcmp(version, g_43e86c[i].version))
		{
			result = true;
			break;
		}
	}
	return result;
}
