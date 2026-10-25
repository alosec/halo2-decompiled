// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_138180.CPP: game session options (validate, compare, initialize) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#define MAXIMUM_CONTROLLERS 4

dword g_4e61cc[MAXIMUM_CONTROLLERS];
byte g_440070[12];

static inline dword string_length_bounded(const char *string, dword maximum_length)
{
	const char *end = string;
	dword length = 0;
	do
	{
		if (!*end++)
		{
			break;
		}
		length++;
	} while (length < maximum_length);
	return length;
}

// score 185 (was 187): frame 0x18 agrees. Retail compares i and count signed
// (long), stores count = 0 before the first loop and counts a player before
// its uniqueness test. Registers still rotate: retail has options in edi
// (reloaded after the inner loop), i in ebp, mask in esi with home [esp+0x1c];
// we have options in esi, i in edi (spilled), mask in ebp.
// Retail's tail (one shared local_machine_valid test ending in mov al, 1, and
// a separate false return after count <= 2) comes out exactly with
// memcmp(&players[i].machine, &machines[m], ...) and a switch of
// ok = ok && count > 1 && count <= 2 (12c) / ok = ok && count == 1, then
// local_1 = ok && options->local_machine_valid; case 2/3 as one && each;
// but that scores 199: the m loop tests the mask in memory (one instruction
// where retail loads it into ecx) so the tail sits one line off.
// Tried: count declared at the top (201), i per loop (202 with the switch form), direct returns in
// case 1 (238), else branch first (208), machine_mask read directly or
// mask & (1 << m) (no change), ?: or if for local_machine_valid (193, 298).
// Long validation result 275; long player uniqueness 270; byte uniqueness 187;
// short membership result 190; word active count 195.
// score 174 (was 185): Frame 0x14 versus 0x18 and index/options/mask homes still differ.
// Membership arm first 226; final validity merged 185; uniqueness merged 185; both merged 185;
// success-only stores 300; mask range 230; signed-byte found index 175 kept; membership first plus
// shared uniqueness 226. With byte index, role name 175; long uniqueness 238; count bounds 173
// dropped for only two further points.
// score 174 (was 174): frame 0x14 versus 0x18; options/index/mask homes still differ.
// Machine/duplicate/both bit cursors 270/285/312; membership bit 245; outer plus
// membership bits 235; all bit cursors 295; case orders 2,1,3 and 3,1,2 198/224.
// First bit cursor recovers frame/options register but loses entry zero and adds 12 bytes.
// frame and options/index/mask homes still differ.
// native membership cursor 233f; byte membership cursor 269f.
// machine array base 175f; player array base 242f.
// native array bases 242f; external options record 175f.
// search cursor machine base 231f; byte search player base 272f.
// frame 0x14 versus 0x18 and options/index/mask homes remain;
// fast bounded scan indexed 239; direct terminator exit 175; signed result/count/bound 175/175/175;
// signed count and result 175; indexed terminator exit 239; scan cursor/count names 175.
// Frame 0x14/0x18 and options/index/mask registers still differ.
// shared duplicate index 175; shared candidate index 175.
// short duplicate indices 308; byte duplicate indices 294.
// short membership index 234; byte membership index 234.
// long final result 220; short final result 235.
// score 174 (was 174): frame 0x14/0x18 and options/index/mask homes remain.
// Fast: native machine 266; native identifier 295; native player machine 282; identifier and machine 287;
// native player 288; both machines 313; identifier library copy 295; machine library copy 266.
// score 171 (was 177): frame 0x14/0x18 and options/index/mask homes remain.
// Byte-frame equality input order: machine pair 175f; empty identifier 175f;
// duplicate identifier 177f; local machine 176f; membership 173f/175p;
// machine and membership 173f; player comparisons 178f; all comparisons 176f.
// Two-point plain membership gain rejected; original comparison inputs retained.
// frame 0x14/0x18 and options/index/mask homes remain;
// session type/option kind/limit/index values 175/175/175/175; name length 175;
// local-machine validity value 263; uniqueness/found entry lifetimes 175/175.
// @retail 0x138180
bool __stdcall function_138180(const s_session_options *options)
{
	bool local_1 = false;
	{
		long i = 0;
		if (options->type < 0 || options->type >= 6)
		{
			local_1 = false;
			goto local_2;
		}
		if (options->unknown4 < 0 || options->unknown4 >= 6)
		{
			local_1 = false;
			goto local_2;
		}
		if (options->unknown6 <= 0 || options->unknown6 > 300)
		{
			local_1 = false;
			goto local_2;
		}
		if (string_length_bounded(options->name, sizeof(options->name)) <= 0)
		{
			local_1 = false;
			goto local_2;
		}
		if (options->unknown120 < 0 || options->unknown120 >= 16)
		{
			local_1 = false;
			goto local_2;
		}

		long count = 0;
		dword mask = options->machine_mask;
		bool ok = (mask & 0xffff0000) == 0;
		for (; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if ((1 << i) & mask)
			{
				for (long j = i + 1; j < MAXIMUM_PLAYERS_PER_SESSION; j++)
				{
					if ((1 << j) & mask)
					{
						ok = ok && memcmp(&options->machines[i], &options->machines[j], sizeof(s_session_machine)) != 0;
					}
				}
			}
		}

		for (i = 0; i < MAXIMUM_PLAYERS_PER_SESSION; i++)
		{
			if (options->players[i].active)
			{
				count++;
				bool unique = ok && options->players[i].index >= 0 && options->players[i].index < MAXIMUM_CONTROLLERS && memcmp(&options->players[i].id, g_440070, sizeof(options->players[i].id)) != 0;
				for (long j = i + 1; j < MAXIMUM_PLAYERS_PER_SESSION; j++)
				{
					if (options->players[j].active)
					{
						unique = unique && memcmp(&options->players[i].id, &options->players[j].id, sizeof(options->players[i].id)) != 0;
					}
				}
				if (options->players[i].flag1)
				{
					ok = unique && memcmp(&options->players[i].machine, g_440070, sizeof(options->players[i].machine)) == 0;
				}
				else
				{
					signed char found = NONE;
					for (long m = 0; m < MAXIMUM_PLAYERS_PER_SESSION; m++)
					{
						if (((1 << m) & mask) && memcmp(&options->machines[m], &options->players[i].machine, sizeof(s_session_machine)) == 0)
						{
							found = m;
							break;
						}
					}
					ok = unique && found != NONE;
				}
			}
		}

		switch (options->type)
		{
		case 1:
			ok = ok && options->unknown12a >= 0 && options->unknown12a < 4;
			if (options->unknown12c)
			{
				if (!ok)
				{
					local_1 = false;
					goto local_2;
				}
				if (count >= 2 && count <= 2)
				{
					local_1 = options->local_machine_valid != 0;
					goto local_2;
				}
				local_1 = false;
				goto local_2;
			}
			else
			{
				if (!ok || count != 1)
				{
					local_1 = false;
					goto local_2;
				}
				local_1 = options->local_machine_valid != 0;
				goto local_2;

			}
		case 2:
			if (!ok)
			{
				local_1 = false;
				goto local_2;
			}
			if (count >= 1 && count <= 16)
			{
				local_1 = options->local_machine_valid != 0;
				goto local_2;
			}
			local_1 = false;
			goto local_2;
		case 3:
			if (!ok)
			{
				local_1 = false;
				goto local_2;
			}
			if (count >= 1)
			{
				local_1 = options->local_machine_valid != 0;
				goto local_2;

			}
			local_1 = false;
			goto local_2;
		case 4:
			local_1 = ok;
			goto local_2;
		case 5:
			local_1 = ok;
			goto local_2;
		default:
			local_1 = false;
			goto local_2;
		}
	}
local_2:
	return local_1;
}

// @retail 0x1384a0
bool __stdcall function_1384a0(const s_session_options *a, const s_session_options *b)
{
	if (strcmp(a->name, b->name) == 0 && a->type == b->type && a->unknown6 == b->unknown6)
	{
		dword masks[2];
		for (long k = 0; k < 2; k++)
		{
			const s_session_options *options = k == 0 ? a : b;
			masks[k] = 0;
			if (options->local_machine_valid)
			{
				for (long j = 0; j < MAXIMUM_PLAYERS_PER_SESSION; j++)
				{
					const s_session_player *player = &options->players[j];
					if (player->active && !player->flag1 && memcmp(&player->machine, &options->local_machine, sizeof(s_session_machine)) == 0)
					{
						masks[k] |= 1 << (byte)player->index;
					}
				}
			}
		}
		if (masks[0] == masks[1])
		{
			bool equal = true;
			switch (a->type)
			{
			case 1:
				equal = a->unknown12c == b->unknown12c && a->unknown12a == b->unknown12a;
				break;
			case 2:
				equal = memcmp(a->unknown134, b->unknown134, sizeof(a->unknown134)) == 0;
				break;
			}
			return equal;
		}
	}
	return false;
}

// @retail 0x138600
int __cdecl function_138600(const void *a, const void *b)
{
	short x = *(const short *)a;
	short y = *(const short *)b;
	if (x == NONE && y != NONE)
	{
		return 1;
	}
	if (y == NONE && x != NONE)
	{
		return -1;
	}
	if (x > y)
	{
		return 1;
	}
	return x < y ? -1 : 0;
}

static inline long controller_index_next(long index)
{
	long next = NONE;
	if (index >= 0 && index < MAXIMUM_CONTROLLERS - 1)
	{
		next = index + 1;
	}
	return next;
}

// @retail 0x138640
void __stdcall function_138640(long count, s_session_options *options)
{
	options->machine_mask = 1;
	memset(&options->machines[0], 0, sizeof(s_session_machine));
	options->local_machine = options->machines[0];
	options->local_machine_valid = 1;

	long order[MAXIMUM_CONTROLLERS];
	bool used[MAXIMUM_CONTROLLERS];
	long index;
	for (index = 0; index != NONE; index = controller_index_next(index))
	{
		used[index] = false;
		order[index] = NONE;
	}

	for (long i = 0; i < count; i++)
	{
		long controller = NONE;
		for (index = 0; index != NONE; index = controller_index_next(index))
		{
			if (g_4e61cc[(short)index] && !used[index])
			{
				controller = index;
				break;
			}
		}
		if (controller == NONE)
		{
			controller = 0;
		}
		if (used[controller])
		{
			for (index = 0; index != NONE; index = controller_index_next(index))
			{
				if (!used[index])
				{
					controller = index;
					break;
				}
			}
		}
		order[i] = controller;
		used[controller] = true;
	}
	qsort(order, MAXIMUM_CONTROLLERS, sizeof(long), function_138600);

	memset(options->players, 0, sizeof(options->players));
	for (long i = 0; i < count; i++)
	{
		options->players[i].active = 1;
		options->players[i].flag1 = 0;
		options->players[i].machine = options->local_machine;
		options->players[i].controller = order[i];
		options->players[i].index = (short)i;
		memset(&options->players[i].id, 0, 12);
		options->players[i].id.index = (byte)i;
		options->players[i].id.unknown1 = 0xff;
		options->players[i].id.unknown2 = 0xff;
		options->players[i].id.unknown3 = 0xff;
		wcsncpy(options->players[i].name, L"", 0x1f);
		options->players[i].name[0x1f] = 0;
		memset(options->players[i].unknown5c, 0, sizeof(options->players[i].unknown5c));
		if (options->type == 2)
		{
			options->players[i].flag98 = (byte)i;
		}
		else
		{
			options->players[i].flag98 = 1;
		}
	}
}
