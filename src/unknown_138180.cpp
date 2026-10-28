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

/* whether a session's options are consistent: the header fields in range,
   distinct machines, valid and distinct players on known machines, and a
   player count the session type allows */
// @retail 0x138180
bool __stdcall function_138180(const s_session_options *options)
{
	bool result;
	if (options->type >= 0 && options->type < 6 &&
		options->unknown4 >= 0 && options->unknown4 < 6 &&
		options->unknown6 > 0 && options->unknown6 <= 300 &&
		string_length_bounded(options->name, sizeof(options->name)) > 0 &&
		options->unknown120 >= 0 && options->unknown120 < 16)
	{
		long count = 0;
		dword mask = options->machine_mask;
		bool ok = (mask & 0xffff0000) == 0;
		for (long machine_index = 0; machine_index < MAXIMUM_PLAYERS_PER_SESSION; machine_index++)
		{
			if ((1 << machine_index) & mask)
			{
				for (long j = machine_index + 1; j < MAXIMUM_PLAYERS_PER_SESSION; j++)
				{
					if ((1 << j) & mask)
					{
						ok = ok && memcmp(&options->machines[machine_index], &options->machines[j], sizeof(s_session_machine)) != 0;
					}
				}
			}
		}

		for (long player_index = 0; player_index < MAXIMUM_PLAYERS_PER_SESSION; player_index++)
		{
			if (options->players[player_index].active)
			{
				count++;
				bool unique = ok && options->players[player_index].index >= 0 && options->players[player_index].index < MAXIMUM_CONTROLLERS && memcmp(&options->players[player_index].id, g_440070, sizeof(options->players[player_index].id)) != 0;
				for (long j = player_index + 1; j < MAXIMUM_PLAYERS_PER_SESSION; j++)
				{
					if (options->players[j].active)
					{
						unique = unique && memcmp(&options->players[player_index].id, &options->players[j].id, sizeof(options->players[player_index].id)) != 0;
					}
				}
				if (options->players[player_index].flag1)
				{
					ok = unique && memcmp(&options->players[player_index].machine, g_440070, sizeof(options->players[player_index].machine)) == 0;
				}
				else
				{
					long found = NONE;
					for (long m = 0; m < MAXIMUM_PLAYERS_PER_SESSION; m++)
					{
						if (((1 << m) & mask) && memcmp(&options->players[player_index].machine, &options->machines[m], sizeof(s_session_machine)) == 0)
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
				ok = ok && count > 1 && count <= 2;
			}
			else
			{
				ok = ok && count == 1;
			}
			result = ok && options->local_machine_valid;
			break;
		case 2:
			result = ok && count >= 1 && count <= 16 && options->local_machine_valid;
			break;
		case 3:
			result = ok && count >= 1 && options->local_machine_valid;
			break;
		case 4:
		case 5:
			result = ok;
			break;
		default:
			result = false;
			break;
		}
	}
	else
	{
		result = false;
	}
	return result;
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
