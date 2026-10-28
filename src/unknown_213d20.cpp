// @flags /O2 /Gr
/* UNKNOWN_213D20.CPP (part): which map file a map name stands for and
   where it is: open as a cache file, copied to the utility drive, or being
   copied (0x213d20..0x214e40). Decompiled by lane T for the map loading code
   of 0x163680..0x163b60. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>

char *csprintf_256(char *buffer, const char *format, ...);
int function_11c920(char const *s1, char const *s2);

/* the open cache files (0x804 bytes each, unknown_213760.cpp); the name of
   each file's map is at +0x24 */
struct s_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

extern s_cache_file g_557c90[3];

#define MAXIMUM_CACHE_FILES 6
#define cache_file_map_name(index) ((char const *)&g_557c90[index] + 0x24)

/* the maps copied to the utility drive (0x108 bytes each) */
struct s_cache_copy
{
	char path[256];
	long last_used;
	long state;
};

#define MAXIMUM_CACHE_COPIES 8

s_cache_copy g_55acac[MAXIMUM_CACHE_COPIES];

/* the map being copied and the two maps waiting to be */
struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

extern long g_55bd04;
extern long g_55bd08;
bool g_55bd0c;
char g_55bd21[0x103];
s_cache_copy_request g_55be24[2];

enum
{
	_map_location_none = 0,
	_map_location_queued,
	_map_location_copying,
	_map_location_open,
	_map_location_copied
};

__declspec(noinline) void cache_copy_hurry(void);

// @retail 0x214d40
void map_name_from_path(char const *path, char *name)
{
	char *extension;
	char *c;
	dword count;

	strncpy(name, path, 0x100);
	name[0xff] = 0;
	extension = name + strspn(name, ".");
	if (*extension)
	{
		extension = strpbrk(extension, ".");
		if (extension)
		{
			*extension = 0;
		}
	}
	for (count = 0x100, c = name; *c && count-- > 0; c++)
	{
		char character = *c;

		if (character >= 'A' && character <= 'Z')
		{
			character += 'a' - 'A';
		}
		*c = character;
	}
}

// @retail 0x214db0
void map_file_path_get(char const *map_name, char *path)
{
	char name[256];

	if (map_name[0] == 't' && map_name[1] == ':' && map_name[2] == '\\' ||
		map_name[0] == 'w' && map_name[1] == ':' && map_name[2] == '\\')
	{
		map_name_from_path(map_name, name);
		csprintf_256(path, "%s.map", name);
	}
	else
	{
		char const *file_name = strrchr(map_name, '\\');

		if (file_name)
		{
			file_name++;
		}
		else
		{
			file_name = map_name;
		}
		map_name_from_path(file_name, name);
		csprintf_256(path, "d:\\maps\\%s.map", name);
	}
}

// @retail 0x214a60
#ifdef H2_NATIVE_PORT
long cache_file_find(char const *map_name);
#else
long cache_file_find(char const *map_name)
{
	char path[256];
	long result = NONE;
	long index;

	path[0] = 0;
	map_file_path_get(map_name, path);
	for (index = 0; index < MAXIMUM_CACHE_FILES; index++)
	{
		if (!function_11c920(path, cache_file_map_name(index)))
		{
			result = index;
			break;
		}
	}
	return result;
}
#endif

// @retail 0x214060
s_cache_copy *cache_copy_find(char const *map_name)
{
	s_cache_copy *volatile result;
	char path[256];
	dword index;

	path[0] = 0;
	result = NULL;
	map_file_path_get(map_name, path);
	for (index = 0; index < MAXIMUM_CACHE_COPIES; index++)
	{
		s_cache_copy *copy = &g_55acac[index];

		if (!strcmp(path, copy->path))
		{
			return copy;
		}
	}
	return result;
}

// @retail 0x2141b0
void cache_copy_forget(char const *map_name)
{
	s_cache_copy *copy = cache_copy_find(map_name);

	if (copy)
	{
		memset(copy, 0, sizeof(*copy));
	}
}

// score 6 (was 6): post-path EBX reuse remains. selected index 76; oldest timestamp 40;
// selected-record arms 6; age operands 9; entry selection 6; short index 45;
// whole-path destination 6; no-selection exit 6.
// post-normalization lookup reuses EBX; retail forms the path address again.
// Fast: empty path value 59; normalizer-owned path 49; record-pointer iterator 56;
// signed record index 56; older record before empty 14; retained-record continue 6;
// state reset before copy 8; guaranteed selected record 37.
// Path helper with an inline definition, with and without a noinline declaration, 6/6; path
// helpers defined after their callers 6; path helper alone in an /Ob1 file 6; print helper
// inline with a noinline declaration 6. Retail also forms 0x2141f0's EDI constant again after
// the call: retail callers act as if they had no register summary for the path helper.
// @retail 0x214100
void __stdcall cache_copy_record_use(char const *map_name)
{
	char path[256];
	s_cache_copy *copy;

	path[0] = 0;
	map_file_path_get(map_name, path);
	copy = cache_copy_find(path);
	if (!copy)
	{
		dword index;

		for (index = 0; index < MAXIMUM_CACHE_COPIES; index++)
		{
			s_cache_copy *candidate = &g_55acac[index];

			if (!candidate->path[0])
			{
				copy = candidate;
				break;
			}
			if (!copy || candidate->last_used < copy->last_used)
			{
				copy = candidate;
			}
		}
		if (copy)
		{
			memcpy(copy->path, path, sizeof(path));
			copy->state = 0;
		}
	}
	if (copy)
	{
		copy->last_used = GetTickCount();
		copy->state++;
		cache_copy_find(map_name);
	}
}

// @retail 0x2141d0
bool cache_copy_complete(char const *map_name)
{
	s_cache_copy *copy = cache_copy_find(map_name);
	long state = 0;

	if (copy)
	{
		state = copy->state;
	}
	return state >= 3 ? true : false;
}

long g_55bd18;
long g_55bd1c;
bool g_55bd20;

long function_12d400(long type, long size, long user_data, long update, long release);
void function_12d520(long address);
bool function_2141f0(void);

// @retail 0x214030
void cache_copy_buffer_release(void)
{
	if (g_55bd18)
	{
		function_12d520(g_55bd18);
		g_55bd18 = 0;
		g_55bd1c = 0;
	}
}

// @retail 0x213fa0
void __stdcall cache_copy_buffer_reclaim(long address, long user_data)
{
#if H2_PORT_SHIMS
	/* The async worker sets g_55bd0c on another thread; read it afresh each
	   pass, as the retail loop does, so the wait cannot be hoisted. */
	while (*(long volatile *)&g_55bd18)
	{
		g_55bd20 = true;
		if (*(bool volatile *)&g_55bd0c)
		{
			function_2141f0();
		}
	}
#else
	while (g_55bd18)
	{
		g_55bd20 = true;
		if (g_55bd0c)
		{
			function_2141f0();
		}
	}
#endif
}

// @retail 0x213fe0
void cache_copy_buffer_resize(long size)
{
	if (g_55bd1c != size)
	{
		cache_copy_buffer_release();
		g_55bd18 = function_12d400(0, size, 0, 0, (long)cache_copy_buffer_reclaim);
		if (g_55bd18)
		{
			g_55bd1c = size;
		}
	}
}

// score 5 (was 5): second buffer address reused after call;
// buffer home names 5; result declared last 5; comparison array initialized 56; equality inputs reversed 14;
// mismatch early return 24; inline comparison declaration 5; noinline comparison declaration 5; noinline
// path declaration 5.
// the second buffer address is reused instead of recomputed after the call.
// Inline declaration 5; noinline declaration 5; byte memset 5; both clears first 25; selected result 24;
// later result 13; both buffers initialized 81; priority-first result 31.
// 0x214db0 forms (callee still matching): returning the printed buffer 5; drive-path test in a bool 5;
// file name from a separator ternary 5; one shared print call 19; path-first parameter order 9; the
// helper in its own file sorted after or before this one 5/5.
// two-buffer record 5; result between buffers 5; result initialized between calls 10;
// unsigned result 5; named equality 5; combined noinline declarations 5;
// combined inline declarations 5; force-inline comparison declaration 5.
// @retail 0x213e30
long cache_copy_current_priority(char const *map_name)
{
	long result = NONE;
	char path[256];
	char current_path[256];

	path[0] = 0;
	map_file_path_get(map_name, path);
	current_path[0] = 0;
	map_file_path_get(g_55bd21, current_path);
	if (!function_11c920(path, current_path))
	{
		result = g_55be24[0].priority;
	}
	return result;
}

// score 5 (was 5): second buffer address reused after call;
// buffer home names 5; result declared last 5; comparison array initialized 56; equality inputs reversed 14;
// mismatch early return 24; inline comparison declaration 5; noinline comparison declaration 5; noinline
// path declaration 5; reversed buffer name order 5.
// the second buffer address is reused instead of recomputed after the call.
// Inline declaration 5; noinline declaration 5; byte memset 5; both clears first 25;
// selected result 24; later result 13; both buffers initialized 81; priority-first result 31.
// 0x214db0 forms (callee still matching): returning the printed buffer 5; drive-path test in a bool 5;
// file name from a separator ternary 5; one shared print call 19; path-first parameter order 9; the
// helper in its own file sorted after or before this one 5/5.
// Queued record pointer 5; delayed/scoped comparison buffer 5/5; interleaved result 5;
// path matrix 15; two-arm result 24; named comparison 5; unsigned result 5.
// @retail 0x213e90
long cache_copy_queued_priority(char const *map_name)
{
	long result = NONE;
	char path[256];
	char queued_path[256];

	path[0] = 0;
	map_file_path_get(map_name, path);
	queued_path[0] = 0;
	map_file_path_get(g_55be24[1].map_name, queued_path);
	if (!function_11c920(path, queued_path))
	{
		result = g_55be24[1].priority;
	}
	return result;
}

/* retail calls this out of line from its one caller, 0x163680 */
__declspec(noinline) long cache_copy_priority(char const *map_name);

// @retail 0x213ef0
long cache_copy_priority(char const *map_name)
{
	long result = cache_copy_current_priority(map_name);

	if (result == NONE)
	{
		result = cache_copy_queued_priority(map_name);
	}
	return result;
}

// score 7 (was 7): post-path EBX reuse remains. shared completion 7; state value 7;
// entry-result single exit 57; short result 55; inline query 7;
// outlined inline query 7; inline completion 7; open success last 37.
// post-normalization open lookup reuses EBX; retail forms the path address again.
// Fast: empty path value 67; normalizer-owned path 47; open-path scope 7; nested copy state 7;
// copy at first use 7; queued result domain 21; open-index observation 7;
// active-priority observation 7.
// Path helper with an inline definition, with and without a noinline declaration, 7/7; path
// helpers defined after their callers 7; path helper alone in an /Ob1 file 7; print helper
// inline with a noinline declaration 7. Retail also forms 0x2141f0's EDI constant again after
// the call: retail callers act as if they had no register summary for the path helper.
// @retail 0x213f10
long map_location_get(char const *map_name)
{
#ifdef H2_NATIVE_PORT
	/* The native host has one synchronously owned cache and no Xbox utility
	 * drive or asynchronous copy queue. Report only that exact ownership. */
	return cache_file_find(map_name) != NONE ? _map_location_open : _map_location_none;
#else
	char path[256];
	s_cache_copy *copy;

	path[0] = 0;
	map_file_path_get(map_name, path);
	if (cache_file_find(path) != NONE)
	{
		return _map_location_open;
	}
	copy = cache_copy_find(map_name);
	if (copy && copy->state >= 3)
	{
		return _map_location_copied;
	}
	if (cache_copy_current_priority(map_name) != NONE)
	{
		return _map_location_copying;
	}
	return cache_copy_queued_priority(map_name) != NONE;
#endif
}

// score 3 (was 3): Frame agrees; the second path address reuses EBX instead of being formed again from ESP
// (retail keeps EDI, the map name, across 0x214db0 but treats its EBX buffer argument as clobbered).
// Tried buffer declarations 3; first buffer name 3; second buffer name 3; result lifetime 3; second clear
// lifetime 7; test result 36; two result paths 36; integral result 52; path helper reverse 5; path helper
// cdecl 5; path helper stdcall 3; noinline declaration 3; inline declaration 3; first buffer initialized
// 65; second initialized 68; byte memset 3; empty strncpy 81; both buffers initialized 94; shared byte
// clear 5; symmetric comparison 39; named comparison 3; named symmetric comparison 39; path array loop
// with per-path clear 48, preclear 59; path pointer loop with per-path clear 69, preclear 102; path array
// while traversal 48; later element address 3; whole-array pointer 3; two-dimensional paths 3; call
// element address 3; call-output pointer 3; later comparison pointer 3; later whole-array comparison
// pointer 3; one-member buffer structs (object address to the call, field to the compare) 3.
// 0x214db0 forms (callee still matching): returning the printed buffer 3; drive-path test in a bool 3;
// file name from a separator ternary 3; one shared print call 45 (callee 39); path-first parameter
// order 5; the helper in its own file sorted after or before this one 3/3.
// @retail 0x214e40
bool map_names_equal(char const *map_name, char const *other_map_name)
{
	bool result = false;
	char path[256];
	char other_path[256];

	path[0] = 0;
	other_path[0] = 0;
	map_file_path_get(map_name, path);
	map_file_path_get(other_map_name, other_path);
	if (!strcmp(path, other_path))
	{
		result = true;
	}
	return result;
}

// @retail 0x214990
void cache_copy_hurry(void)
{
	if (g_55bd04 && g_55bd04 < 0x11)
	{
		g_55bd08 = 2;
		g_55bd04 = 0x11;
	}
}

// @retail 0x213d20
bool map_copy_request(char const *map_name, long priority)
{
	bool result = false;
	long location = map_location_get(map_name);
	char path[256];

	path[0] = 0;
	map_file_path_get(map_name, path);
	if (location == _map_location_none)
	{
		if (!g_55be24[1].map_name[0] || priority >= g_55be24[1].priority)
		{
			memset(&g_55be24[1], 0, sizeof(g_55be24[1]));
			strncpy(g_55be24[1].map_name, path, 0x100);
			g_55be24[1].map_name[0xff] = 0;
			g_55be24[1].priority = priority;
			result = true;
		}
	}
	else if (location == _map_location_copying)
	{
		if (map_names_equal(path, g_55be24[0].map_name))
		{
			g_55be24[0].priority = priority;
		}
		result = true;
	}
	else if (location == _map_location_queued)
	{
		if (map_names_equal(path, g_55be24[1].map_name))
		{
			g_55be24[1].priority = priority;
		}
		result = true;
	}
	else if (location == _map_location_open)
	{
		result = true;
	}
	if (g_55be24[0].map_name[0] && g_55be24[1].map_name[0] && g_55be24[1].priority >= g_55be24[0].priority)
	{
		cache_copy_hurry();
	}
	return result;
}
