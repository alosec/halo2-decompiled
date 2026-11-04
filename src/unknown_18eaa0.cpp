// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_058ee0.h"
#include <xtl.h>

struct s_18eaa0
{
	byte field_0[0x1c];
	char field_1c[1];
};

bool g_4e9bbc;
dword g_4e9bc0;
bool g_547f29;
extern bool g_51ea00;
extern bool g_4ed39d;
extern bool g_4ed39e;
extern dword g_4ed3a0;
long map_location_get(char const *arg_0);
bool __stdcall function_18f100(char const *arg_0);
void function_18ec20(bool arg_0);
bool cache_files_load_map(char const *arg_0);
void function_11c840(void);
void function_18ec60(bool arg_0, char const *arg_1);
void function_18ed00(bool arg_0);
void function_593e0(void);
#if H2_WHOLE_IMAGE
extern "C" void h2_probe(char const *, unsigned, unsigned, unsigned);
#endif

#pragma inline_depth(0)
// @retail 0x18eaa0
bool function_18eaa0(s_18eaa0 const *arg_0)
{
	volatile bool local_0 = false;
	char const *local_1 = arg_0->field_1c;
	switch (map_location_get(local_1))
	{
	case 0:
		if (!function_18f100(local_1)) goto local_3;
		goto local_2;
	case 1:
		if (!function_18f100(local_1)) goto local_3;
		goto local_2;
	case 2:
		if (!function_18f100(local_1)) goto local_3;
	case 3:
	local_2:
		g_4e9bbc = true;
		g_4e9bc0 = GetTickCount();
		function_18ec20(false);
		if (cache_files_load_map(local_1))
		{
			function_11c840();
			function_18ec60(false, local_1);
			local_0 = true;
			g_4e9bbc = false;
			return true;
		}
		else
			function_18ed00(false);
		g_4e9bbc = false;
		break;
	case 4:
	local_3:
#if H2_WHOLE_IMAGE
		/* port: name the fatal "problem with the disc" screen's cause */
		h2_probe("h2-fatal map load failed (0x18eaa0), location %u", (unsigned)map_location_get(local_1), 0, 0);
#endif
		g_51ea00 = true;
		g_547f29 = true;
		g_4ed39e = true;
		g_4ed39d = true;
		g_4ed3a0 = GetTickCount();
		if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
			function_593e0();
		break;
	default:
		__assume(0);
	}
	return local_0;
}
#pragma inline_depth(255)
