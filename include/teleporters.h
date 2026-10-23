#ifndef TELEPORTERS_H
#define TELEPORTERS_H

/* TELEPORTERS.H: the data the multiplayer teleporters read. A teleporter is
   a pair of scenario markers: an entrance (marker type 7) and an exit (marker
   type 8) that share a key. The pairs live in the multiplayer globals
   (s_mp_globals::teleporters, s_teleporter in globals.h). */

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

/* the bits of s_teleporter_player::flags the teleporters use; the network
   code calls the first one "blocking-teleporter" */
enum e_teleporter_player_flags
{
	_teleporter_player_blocking_teleporter_bit = 2, /* stands on an exit someone tried to use */
	_teleporter_player_near_entrance_bit = 7,       /* stands near a teleporter's entrance this tick */
	_teleporter_player_teleported_bit = 8,          /* went through and has not left the entrance since */
};

/* a player (0x21c bytes, the g_4e8c24 data) as the teleporters see it */
struct s_teleporter_player
{
	short identifier;
	union
	{
		word flags; /* e_teleporter_player_flags */
		struct
		{
			word : 2;
			word blocking_teleporter : 1;
			word : 4;
			word near_entrance : 1;
			word has_teleported : 1;
			word : 7;
		};
	};
	byte unknown04[0x28 - 0x4];
	short local_index; /* the local player's index, NONE when remote */
	byte unknown2a[0x2c - 0x2a];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};
typedef char assert_teleporter_player_local_index[(offsetof(s_teleporter_player, local_index) == 0x28) ? 1 : -1];
typedef char assert_teleporter_player_unit_index[(offsetof(s_teleporter_player, unit_index) == 0x2c) ? 1 : -1];
typedef char assert_teleporter_player_size[(sizeof(s_teleporter_player) == 0x21c) ? 1 : -1];

/* the iterator of player_iterator_next_with_unit (the players with a unit) */
struct s_teleporter_player_iterator
{
	s_teleporter_player *player;
	s_record_pool *data;
	long datum_index;
	long index;
};
typedef char assert_teleporter_player_iterator_size[(sizeof(s_teleporter_player_iterator) == 0x10) ? 1 : -1];

/* an object (g_4e0300's object pointers) as the teleporters read it */
struct s_teleporter_object
{
	byte unknown00[0x14];
	long parent_index; /* NONE unless the object rides in or on another */
	byte unknown18[0x70 - 0x18];
	vector3f forward;
	byte unknown7c[0xaa - 0x7c];
	byte type; /* units are types 0 and 1 */
	byte unknownab[0x13c - 0xab];
	union
	{
		long player_index; /* the player controlling the unit */
		short player_index_short;
	};
};
typedef char assert_teleporter_object_parent[(offsetof(s_teleporter_object, parent_index) == 0x14) ? 1 : -1];
typedef char assert_teleporter_object_forward[(offsetof(s_teleporter_object, forward) == 0x70) ? 1 : -1];
typedef char assert_teleporter_object_type[(offsetof(s_teleporter_object, type) == 0xaa) ? 1 : -1];
typedef char assert_teleporter_object_player[(offsetof(s_teleporter_object, player_index) == 0x13c) ? 1 : -1];

/* an element (0xc bytes) of the object header data (g_4e0300) */
struct s_teleporter_object_header
{
	byte unknown00[8];
	s_teleporter_object *object;
};
typedef char assert_teleporter_object_header_size[(sizeof(s_teleporter_object_header) == 0xc) ? 1 : -1];

/* the deepest shape a point is in (s_shape_result, unknown_245bd0.cpp): the
   object it belongs to comes first */
struct s_teleporter_exit_shape
{
	long object_index;
	byte unknown04[0x30 - 0x4];
};
typedef char assert_teleporter_exit_shape_size[(sizeof(s_teleporter_exit_shape) == 0x30) ? 1 : -1];

/* the multiplayer constants the teleporters read */
struct s_teleporter_constants
{
	byte unknown00[4];
	real recharge_seconds; /* how long a teleporter rests after use */
	byte unknown08[0xf8 - 0x8];
	long blocked_message; /* string handle shown when the exit is blocked */
};
typedef char assert_teleporter_constants_recharge[(offsetof(s_teleporter_constants, recharge_seconds) == 0x4) ? 1 : -1];
typedef char assert_teleporter_constants_message[(offsetof(s_teleporter_constants, blocked_message) == 0xf8) ? 1 : -1];

/* an element of the multiplayer globals tag's second block */
struct s_teleporter_runtime
{
	byte unknown00[0x534];
	s_teleporter_constants *constants;
};
typedef char assert_teleporter_runtime_constants[(offsetof(s_teleporter_runtime, constants) == 0x534) ? 1 : -1];

/* the multiplayer globals tag (g_4e3b44[g_4e034c->index]) as the
   teleporters read it */
struct s_teleporter_globals_definition
{
	byte unknown00[8];
	long runtime_count;
	s_teleporter_runtime *runtime;
};
typedef char assert_teleporter_globals_definition_size[(sizeof(s_teleporter_globals_definition) == 0x10) ? 1 : -1];

#endif
