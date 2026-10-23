/* PLAYER_QUERIES.H: the players (g_4e8c24, 0x21c bytes each) and the unit,
   item and tag data player_queries.cpp reads through them. Only the fields
   it touches are named. */

#ifndef PLAYER_QUERIES_H
#define PLAYER_QUERIES_H

#include "unknown_11c920.h"
#include "data_array.h"
#include <stddef.h>

struct s_player_datum
{
	short identifier;
	byte flags;             /* players with flag 2 are skipped by player_iterator_next */
	byte unknown03[0x2c - 3];
	long unit_index;        /* NONE when the player has no unit */
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x21c - 0xc1];
};
typedef char assert_player_datum_size[(sizeof(s_player_datum) == 0x21c) ? 1 : -1];
typedef char assert_player_datum_unit_index[(offsetof(s_player_datum, unit_index) == 0x2c) ? 1 : -1];
typedef char assert_player_datum_team[(offsetof(s_player_datum, team) == 0xc0) ? 1 : -1];

/* an iterator over the players (its callers pass it as a pointer to longs);
   from +4 it is an s_record_pool_iterator */
struct s_player_iterator
{
	s_player_datum *player;
	s_record_pool *data;
	long datum_index;
	long index;
};

/* an element of the object headers (g_4e0300) */
struct s_player_object_header
{
	byte unknown00[8];
	byte *object;
};

/* a unit object: the items it carries at +0x218 */
struct s_player_unit
{
	byte unknown00[0x218];
	long items[4];
};

/* an item object: its tag first */
struct s_player_item
{
	long tag_index;
};

/* an item's tag: its type at +0x290 */
struct s_player_item_tag
{
	byte unknown00[0x290];
	short type;
};

#endif
