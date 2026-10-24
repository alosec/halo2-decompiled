// @flags /O2 /Ob1 /Gr
/* the game results' start (retail 0x195a40, called from 0x67ee0 when the
   simulation starts): the 0xdcd8-byte results object at 0x510cb0 is
   cleared, and when the game has an engine it takes the game options' map,
   variant and player records, the teams of the players, and whether every
   machine of the results is the host (and connected); a game that is not a
   film playback (mode 4) then starts recording its results.

   Follows the draft parked in src/unknown_1967d0.cpp (its 0x195a40 takes the
   results as an argument; retail writes the global storage directly). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_4cf790.h"
#include <string.h>
#include <time.h>

extern dword g_510ca4;
extern dword g_510ca8;
extern byte g_510ca1;

struct s_surface_description;
struct s_simulation_world_owner;
struct s_machine_address;
struct s_session_machine_address;
struct s_network_observer;
struct s_address_table;

s_surface_description *function_192e60(long index);
void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count);
long function_1991f0(s_address_table *table, byte const *address);
long function_199250(s_address_table *table, byte const *address);
long simulation_watcher_find_machine(const s_simulation_world_owner *watcher, const s_machine_address *address);
long network_observer_find_channel_by_machine(s_network_observer *observer, const s_session_machine_address *address, long volatile owner);
void function_196470(void);
void function_196780(void);

/* whether the machine of an address has its observer channel up (state 7) */
static bool results_machine_connected(byte const *address)
{
	s_network_observer *observer = *(s_network_observer **)((byte *)g_4cf780 + 8);
	if (!observer || simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
		(s_machine_address const *)address) == NONE)
		return false;
	long channel = network_observer_find_channel_by_machine(observer, (s_session_machine_address const *)address, 3);
	return channel != NONE && *(long *)((byte *)observer + channel * 0x528 + 0xa8) == 7;
}

/* whether an address is the session's host machine */
static bool results_machine_is_host(byte const *address)
{
	byte *session = *(byte **)((byte *)g_4cf780 + 0xc);
	if (!session || simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
		(s_machine_address const *)address) == NONE)
		return false;
	return !memcmp(session + 0x62 + *(long *)(session + 0x40) * 0x10c, address, 6);
}

// @retail 0x195a40
void function_195a40(void)
{
	byte *results = &g_510cb0;

	memset(results, 0, 0xdcd8);
	g_510ca4 = 0;
	g_510ca8 = (dword)time(NULL);
	if (!g_55e4d0[g_4e9ae8->engine_index])
		return;

	byte *options = (byte *)g_4e6948;
	*(long *)(results + 0x130) = *(long *)(options + 0x10);
	*(long *)(results + 0x134) = *(long *)(options + 0x14);
	*(long *)(results + 4) = *(long *)(options + 0x138);
	if (*(long *)(options + 0x138) != NONE)
	{
		byte *variant = (byte *)function_192e60(*(long *)(options + 0x138));
		if (variant)
		{
			long mode = *(long *)variant;
			results[0x12a] = mode == 5;
			results[0x128] = mode == 5 || mode == 2 || mode == 4;
			results[0x129] = mode == 5 || mode == 3 || mode == 4;
			for (long i = 0; i < 9; ++i)
				utf8_string_to_utf16_string((char const *)(variant + 0xc + i * 0x10),
					(word *)(results + 8 + i * 0x20), 0x10);
		}
	}
	*(long *)(results + 0x130) = *(long *)(options + 0x10);
	*(long *)(results + 0x134) = *(long *)(options + 0x14);
	memcpy(results + 0x138, options + 0x13c, 0x130);
	*(long *)(results + 0x268) = *(long *)(options + 0x20);
	memcpy(results + 0x26c, options + 0x24, 0x104);

	/* the sixteen player records (0xa4 bytes at 0x511034) from the options'
	   players (0xe4 bytes at +0x2dc) */
	for (long i = 0; i < 16; ++i)
	{
		byte *source = options + 0x2dc + i * 0xe4;
		byte *record = results + 0x384 + i * 0xa4;
		record[0] = source[0];
		if (record[0])
		{
			memcpy(record + 2, source + 0xe, 12);
			memcpy(record + 0x10, source + 0x1c, 0x90);
			record[1] = (byte)function_199250((s_address_table *)results, source + 8);
			if (record[1] == 0xff)
				record[1] = (byte)function_1991f0((s_address_table *)results, source + 8);
		}
	}

	/* team games: every team with a player gets its entry (0x18 bytes at
	   0x511a74) */
	results[0x12b] = results[0x180] & 1;
	if (results[0x12b])
	{
		for (long i = 0; i < 16; ++i)
		{
			byte *record = results + 0x384 + i * 0xa4;
			long team = *(signed char *)(record + 0x8c);
			if (!record[0] || team == NONE)
				continue;
			byte *entry = results + 0xdc4 + team * 0x18;
			if (entry[0])
				continue;
			entry[0] = 1;
			entry[1] = 0xff;
			*(short *)(entry + 2) = 0;
			memset(entry + 0xc, 0, 12);
			*(long *)(entry + 8) = NONE;
			*(short *)(entry + 4) = NONE;
			if (results[0x12a] && memcmp(record + 0x80, g_440070, 12))
			{
				memcpy(entry + 0xc, record + 0x80, 12);
				*(long *)(entry + 8) = *(long *)(record + 0x9c);
				*(short *)(entry + 4) = *(short *)(record + 0x9a);
			}
		}
	}

	/* the machines (11 bytes each at 0x51e8d4): whether each is the host, and
	   whether it is connected when either end is the host */
	bool local_host = results_machine_is_host(g_4cf7cc);
	for (long i = 0; i < 16; ++i)
	{
		byte *address = results + 0xdc24 + i * 11;
		if (!address[6])
			continue;
		bool remote_host = results_machine_is_host(address);
		address[8] = remote_host;
		address[9] = remote_host;
		if (local_host)
			address[7] = remote_host ? true : results_machine_connected(address);
		else
			address[7] = remote_host ? results_machine_connected(address) : false;
	}

	results[0] = 1;
	if (g_4e6948->mode != 4)
	{
		g_510ca0 = true;
		g_510ca1 = false;
		function_196470();
		function_196780();
	}
}
