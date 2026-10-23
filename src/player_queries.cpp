/* PLAYER_QUERIES.CPP: steps through the players (g_4e8c24), finds the item
   of a type a player's unit carries, and announces how far a player is
   from the score to win. */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "slot_owner.h"
#include "game_engine_events.h"
#include "player_queries.h"

// @flags /O2 /Gr

/* record_pool_iterator_step, as these iterators inline it: once with its call to
   function_16bc00, then with that inlined too */
static inline byte *player_iterator_first(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	return result;
}

/* steps the iterator to the next player whose flag 2 (at +2) is clear;
   false when there are no more */
// @retail 0x19f240
bool player_iterator_next(long *iterator_)
{
	s_player_iterator *iterator = (s_player_iterator *)iterator_;
	s_record_pool_iterator *data_iterator = (s_record_pool_iterator *)&iterator->data;

	iterator->player = (s_player_datum *)player_iterator_first(data_iterator);
	while (iterator->player && (iterator->player->flags & 2))
		iterator->player = (s_player_datum *)data_iterator_next_inlined(data_iterator);

	return iterator->player != 0;
}

/* steps the iterator to the next player that has a unit; false when there
   are no more */
// @retail 0x19f300
bool player_iterator_next_with_unit(long *iterator_)
{
	s_player_iterator *iterator = (s_player_iterator *)iterator_;
	s_record_pool_iterator *data_iterator = (s_record_pool_iterator *)&iterator->data;

	iterator->player = (s_player_datum *)player_iterator_first(data_iterator);
	while (iterator->player && iterator->player->unit_index == NONE)
		iterator->player = (s_player_datum *)data_iterator_next_inlined(data_iterator);

	return iterator->player != 0;
}

/* the first of the four items the player's unit carries whose item tag
   has the given type, or NONE */
// @retail 0x19f3c0
long player_find_unit_item_of_type(long player_index, long type)
{
	s_player_datum *player = (s_player_datum *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_player_datum));
	long result = NONE;

	if (player->unit_index != NONE)
	{
		s_player_object_header *headers = (s_player_object_header *)g_4e0300->data;
		s_player_unit *unit = (s_player_unit *)headers[player->unit_index & 0xffff].object;
		long i;

		for (i = 0; i < 4; i++)
		{
			long item_index = unit->items[i];

			if (item_index != NONE)
			{
				s_player_item *item = (s_player_item *)headers[item_index & 0xffff].object;
				s_player_item_tag *tag = (s_player_item_tag *)g_4e3b44[item->tag_index & 0xffff].bytes;

				if (tag->type == type)
				{
					result = item_index;
					break;
				}
			}
		}
	}

	return result;
}

bool function_15eaf0();

/* the event initialize and set cause player functions of unknown_19de80.cpp,
   which retail inlines here */
static inline void score_event_initialize(s_event *event, long type, long subtype)
{
	event->type = type;
	event->subtype = subtype;
	event->a = NONE;
	event->cause_player_index = NONE;
	event->cause_team = NONE;
	event->effect_player_index = NONE;
	event->effect_team = NONE;
	event->f = 0;
	event->g = NONE;
}

static inline void score_event_set_cause_player(s_event *event, long player_index)
{
	event->cause_player_index = player_index;
	event->cause_team = ((s_player_datum *)g_4e8c24->data)[player_index & 0xffff].team;
}

/* announces that the player is 10, 30 or 60 points from the score to win
   (function_15eaf0 picks between two events for each) */
// @retail 0x19f470
void player_announce_points_to_win(long player_index, long score)
{
	if (g_4e6948->score_to_win - score == 10)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 0x30 : 0x2f);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
	if (g_4e6948->score_to_win - score == 30)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 10 : 9);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
	if (g_4e6948->score_to_win - score == 60)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 8 : 7);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
}
