// @flags /O2 /Gr
/* UNKNOWN_15D770.CPP: whether a dead player may respawn yet: the game's
   minimum count of living players (g_4e6948 +0x1b4) against the players
   alive and the players waiting ahead of this one */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include "engine_peer.h"

/* the players (0x21c bytes each), as the respawn checks see them */
struct s_respawn_player
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x1ac - 0xc1];
	short s1ac;
	byte unknown1ae[0x1b4 - 0x1ae];
	long respawn_time;
	byte unknown1b8[0x21c - 0x1b8];
};

/* the iterator of 0x19f240 */
struct s_respawn_player_iterator
{
	s_respawn_player *player;
	s_record_pool *data;
	long datum_index;
	long index;
};

bool player_iterator_next(long *iterator);

static inline s_respawn_player *respawn_player_get(long player_index)
{
	return (s_respawn_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_respawn_player));
}

static inline bool function_x340af0()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	return result;
}

// @retail 0x15db30
bool function_15db30(long player_index)
{
	bool result = false;

	if (g_4e6948->value1b8 > 0)
	{
		s_respawn_player *player = respawn_player_get(player_index);

		if (player->unit_index == NONE && player->s1ac == 0)
			result = true;
	}
	return result;
}

// @retail 0x15d770
bool function_15d770(long player_index)
{
	s_record_pool *players = g_4e8c24;
	long absolute_index = player_index & 0xffff;
	s_respawn_player *player = (s_respawn_player *)(players->data + (player_index & 0xffff) * sizeof(s_respawn_player));
	bool result = false;

	if (g_4e6948->value1b4 > 0 && player->unit_index == NONE)
	{
		s_respawn_player_iterator iterator;
		long living_count = 0;

		result = true;

		iterator.data = players;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			s_respawn_player *other = iterator.player;

			if ((!function_x340af0() || player->team == other->team) &&
				other->unit_index != NONE && other != player)
			{
				living_count++;
			}
		}

		if (g_4e6948->value1b4 > living_count)
		{
			long needed_count = g_4e6948->value1b4 - living_count;
			long waiting_count = 0;

			iterator.data = g_4e8c24;
			iterator.index = NONE;
			iterator.datum_index = NONE;
			while (player_iterator_next((long *)&iterator))
			{
				s_respawn_player *other = iterator.player;

				if ((!function_x340af0() || player->team == other->team) &&
					other->unit_index == NONE && other != player &&
					!function_15db30(iterator.datum_index) &&
					other->respawn_time <= player->respawn_time &&
					(other->respawn_time != player->respawn_time || absolute_index >= (iterator.datum_index & 0xffff)))
				{
					waiting_count++;
				}
			}

			if (waiting_count < needed_count)
				result = false;
		}
	}
	return result;
}

struct s_respawn_options
{
	byte unknown00[0x184];
	dword flags;
};

bool function_161e10(long team);

// @retail 0x15d920
bool function_15d920(long player_index)
{
	s_record_pool *players = g_4e8c24;
	long index = player_index & 0xffff;
	s_respawn_player *player = (s_respawn_player *)(players->data + (player_index & 0xffff) * sizeof(s_respawn_player));
	bool result = false;
	if ((bool)((((s_respawn_options *)g_4e6948)->flags >> 14) & 1))
	{
		if (function_x340af0())
		{
			if (player->unit_index == NONE)
			{
				result = true;
				char counts[8];
				memset(counts, 0, sizeof(counts));
				s_respawn_player_iterator iterator;
				iterator.data = players;
				iterator.index = NONE;
				iterator.datum_index = NONE;
				while (player_iterator_next((long *)&iterator))
				{
					s_respawn_player *other = iterator.player;
					if (other->team != NONE && (other->unit_index != NONE ||
						!function_15db30(iterator.datum_index) && !function_15d770(iterator.datum_index)))
						counts[other->team]++;
				}
				long minimum = 16;
				for (long team = 0; team < 8; team++)
				{
					if (function_161e10(team) && minimum > counts[team])
						minimum = counts[team];
				}
				if (minimum <= 2)
					minimum = 2;
				if (minimum < counts[player->team])
				{
					long living_count = 0;
					long waiting_count = 0;
					iterator.data = g_4e8c24;
					iterator.index = NONE;
					iterator.datum_index = NONE;
					while (player_iterator_next((long *)&iterator))
					{
						s_respawn_player *other = iterator.player;
						if (player->team == other->team && other != player)
						{
							if (other->unit_index == NONE && function_15db30(iterator.datum_index))
								continue;
							if (other->unit_index != NONE)
								living_count++;
							else if (other->respawn_time <= player->respawn_time &&
								(other->respawn_time != player->respawn_time || index >= (iterator.datum_index & 0xffff)))
								waiting_count++;
						}
					}
					long needed = minimum - living_count;
					if (needed > 0 && waiting_count < needed)
						result = false;
				}
				else
					result = false;
			}
		}
	}
	return result;
}
