// @flags /O2 /Gr
/* UNKNOWN_214F10.CPP: the game state's memory and its two copies on the
   utility drive: unknown_123b30.cpp allocates the memory here, and saves it to
   (and reads it back from) the cache file of one of two slots, through a
   block of the texture cache when one is free. Decompiled by lane L for
   unknown_123b30.cpp (0x123b30..0x123e20). */

#include "unknown_11c920.h"
#include "async.h"
#include "unknown_12b400.h"
#include "unknown_123b30.h"
#include "unknown_5020dc_storage.h"
#include <xtl.h>
#include <string.h>

char *function_11c9c0(char *buffer, long size, const char *format, ...);
long function_12d400(long type, long size, long user_data, long update, long release);
void function_12d520(long address);
void function_125d60(void);

/* the name of a slot's cache file */
char const *g_470050;

bool g_5020d8;
s_game_state_cache g_game_state_cache;

// @retail 0x214f10
void *game_state_cache_allocate(long cpu_size, long gpu_size)
{
	long size = cpu_size + gpu_size;
	void *memory = physical_memory_malloc_fixed(size, PAGE_READWRITE);

	g_5020dc = (dword)memory;
	g_5020e0 = size;
	g_5020d8 = true;
	return memory;
}

long __stdcall function_1a08f0(s_async_task *task);

PRIVATE __forceinline long function_214f83(char const *arg_0, s_file_handle *arg_1, bool volatile *arg_2)
{
    s_async_task local_0;
    memset(&local_0, 0, sizeof(local_0));
    local_0.create_file.path = arg_0;
    local_0.create_file.access = GENERIC_READ | GENERIC_WRITE;
    local_0.create_file.share_mode = 0;
    local_0.create_file.creation_disposition = OPEN_ALWAYS;
    local_0.create_file.flags = 0;
    local_0.create_file.file = arg_1;
    local_0.create_file.create_path = false;
    return function_120ba0(6, &local_0, 8, function_1a08f0, arg_2);
}

// score 42 (was 42): retail tests the handle in ECX and copies it to EAX for the by-value
// argument, builds the success byte with `xor edx, edx; setne dl`, stores it and re-reads it from
// memory before the test, as a volatile flag would; ours tests a register copy. Only a volatile
// success byte (not kept) gives the store and re-read: with the array element passed directly and
// the handle test in an inline helper returning bool (or a long cast) it then matches; without
// it every form forwards the stored value. Forms tried: full-width open result 42; first/second/
// both waits and shared opener/combined helpers 55; branched success 50; local role names 55;
// native/early/named result and late success 42; typed open outcome 42; typed success
// update/complete outcomes 44/42; early outcome home 42; positive outcome/byte admission 43/55;
// initialized success output/byte declaration order 83/42; long ordinal 111; word/byte ordinals
// 42/42; path/status lifetimes 42/42; long ordinal with all workspaces/failure continue 111/111;
// short endpoint equality 42; handle test and call on the array element with the long result 44;
// element copy inside the branch 42; element pointer for open/test/call with the long result 44,
// with a bool result 55; handle tested again at the branch 53; resize inline/outline/both
// 42/42/42; single open/resize/both wait admission 60/52/63; native handle value and branch-set
// full-width open result 42/42; opener void/native result 42/42; ordinary inline 229; task
// aggregate 84; const/early file 42/42; unsigned/word status 42/42; existing wait helper for both
// waits 42 (kept); plain bool success 55; success set inside a test 53; ternary/initialized
// declaration/array element 55/55/55; the plain `success = handle != INVALID; if (success)` 55;
// success at function scope 55; `if (!success) continue;` 55; long result tested through success
// 55; success cleared then set inside the open branch 53; memset-owned share/flags/both/
// create-path/share+path/flags+path/all zeros 40/38/107/36/105/107/112; 64-bit outcome 50;
// signed-byte/word/unsigned/64-bit outcomes on omitted flags 69/38/38/71, on omitted create-path
// 67/36/36/69. Round 96: success from the array element 55, through an inline bool helper 55,
// tested through a resize helper's pointer parameter 55; `success = false; if (valid) success =
// true;` 50 (store sunk into the branch) or with the file copy 53; `success = opened` from a
// long or a long cast 55/55.
// Round 158 (all 55 at 351 bytes, register test): an inline bool handle helper by pointer/by
// value; char success with a bool pointer argument; a long result stored with a bool cast.
// Diagnostic only: with a volatile success flag everything after the open matches but retail's
// `xor edx, edx` (48).
// Round 100: the opener helper also waiting and writing success through a pointer parameter 49
// (still forwarded); success from the array element before the file copy 55; `!(handle ==
// INVALID)` 55; `if (opened != 0)` 42 (identical); the handle compared again at the branch 53.
// @retail 0x214f80
void game_state_cache_files_open(void)
{
	for (short i = 0; i < 2; i++)
	{
		char path[200];
		bool volatile done;
		bool success;

		function_11c9c0(path, sizeof(path), g_470050, i);
		function_214f83(path, &g_5020f8[i], &done);
		function_120d50(&done, false);
		s_file_handle file = g_5020f8[i];

		long opened = file.handle != INVALID_HANDLE_VALUE;
		success = opened != 0;
		if (opened)
		{
			function_1a1310(file, 0x40b000, 8, 6, &success, &done);
			function_120d50(&done, false);
		}
	}
	g_5020f4 = true;
}

// @retail 0x2150f0
void game_state_cache_files_close(void)
{
	for (short i = 0; i < 2; i++)
	{
		bool volatile done;

		function_1a1550(g_5020f8[i], 8, 6, &done);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
	}
	g_5020f4 = false;
}

/* the write from the texture cache block has finished: give the block back */
static __forceinline void game_state_cache_write_finish(void)
{
	g_5020f2[g_5020f0] = g_5020ec == g_5020e0;
	function_12d520(g_5020e4);
	g_5020e4 = 0;
}

// @retail 0x215140
void __stdcall game_state_cache_lock_update(void *address, long user_data)
{
	if (g_5020e8)
	{
		game_state_cache_write_finish();
	}
}

// @retail 0x215180
void __stdcall game_state_cache_lock_release(void *address, long user_data)
{
	while (g_5020e4)
	{
		if (g_5020e8)
		{
			game_state_cache_write_finish();
		}
		else
		{
			SwitchToThread();
		}
	}
}


// @retail 0x2151f0
void game_state_cache_write(short slot)
{
	void const *source;
	long priority;

	if (g_5020e4)
	{
		game_state_cache_lock_release((void *)g_5020e4, 0);
	}
	g_5020f2[slot] = false;
	g_5020e4 = function_12d400(0, g_5020e0, 0, (long)game_state_cache_lock_update, (long)game_state_cache_lock_release);
	g_5020f0 = slot;
	source = (void const *)g_5020dc;
	priority = 3;
	if (g_5020e4)
	{
		memcpy((void *)g_5020e4, (void const *)g_5020dc, g_5020e0);
		source = (void const *)g_5020e4;
		priority = 2;
	}
	s_file_handle file = g_5020f8[slot];
	s_file_write_flags flags(0);
	function_1a1050(file, source, g_5020e0, 0, flags, 8, priority, &g_5020ec, &g_5020e8);
	if (!g_5020e4)
	{
		if (!g_5020e8)
		{
			while (!g_5020e8)
			{
				SwitchToThread();
				function_125d60();
			}
		}
		g_5020f2[slot] = g_5020ec == g_5020e0;
	}
}

// @retail 0x2152e0
bool game_state_cache_read(short slot)
{
	bool result = false;

	if (g_5020e4)
	{
		game_state_cache_lock_release((void *)g_5020e4, 0);
	}
	if (g_5020f2[slot])
	{
		dword bytes_read;
		bool volatile done;

		function_1a0f10(g_5020f8[slot], (void *)g_5020dc, g_5020e0, 0, 8, 6, &bytes_read, &done);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
		result = bytes_read == g_5020e0;
	}
	return result;
}
