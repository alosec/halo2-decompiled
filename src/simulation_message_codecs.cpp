// @flags /O2 /Gr
/* SIMULATION_MESSAGE_CODECS.CPP: the view establishment (view-establishment,
   player-acknowledge), synchronous (synchronous-update, -actions, -join,
   -gamestate), game results and test message codecs, and the functions
   that register them. A decoder returns whether the message it read is
   valid; the synchronous update and actions messages also have a comparison */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "network_message_types.h"
#include <stddef.h>

struct s_view_establishment_message
{
	long mode;
	long value;
};
typedef char assert_view_establishment_message_size[(sizeof(s_view_establishment_message) == 8) ? 1 : -1];

struct s_player_acknowledge_message
{
	dword mask;
	dword acknowledged;
	byte entries[16][12];
};
typedef char assert_player_acknowledge_message_size[(sizeof(s_player_acknowledge_message) == 0xc8) ? 1 : -1];

struct s_synchronous_actions_message
{
	long value;           /* not negative */
	long optional_value;  /* NONE or more, sent plus one */
	bool flag;
	byte unknown09[3];
	dword mask;
	s_player_action actions[4];
};
typedef char assert_synchronous_actions_message_size[(sizeof(s_synchronous_actions_message) == 0x180) ? 1 : -1];

struct s_synchronous_join_message
{
	long value;  /* not negative */
};
typedef char assert_synchronous_join_message_size[(sizeof(s_synchronous_join_message) == 4) ? 1 : -1];

/* a variable size message: size bytes of data follow the header */
struct s_synchronous_gamestate_message
{
	long value;  /* 23 bits, not negative */
	long size;
	byte data[1];
};
typedef char assert_synchronous_gamestate_message_data[(offsetof(s_synchronous_gamestate_message, data) == 8) ? 1 : -1];

struct s_synchronous_update_message
{
	byte unknown00[0x4048];
};
typedef char assert_synchronous_update_message_size[(sizeof(s_synchronous_update_message) == 0x4048) ? 1 : -1];

struct s_game_results_message
{
	dword words[2];  /* sent as they are */
	byte results[0x4fb0];
};
typedef char assert_game_results_message_size[(sizeof(s_game_results_message) == 0x4fb8) ? 1 : -1];

struct s_test_message
{
	bool flag;
	byte unknown01[3];
	long value;
};
typedef char assert_test_message_size[(sizeof(s_test_message) == 8) ? 1 : -1];

/* writes view-establishment */
// @retail 0xb2440
void __stdcall message_view_establishment_encode(s_bitstream *stream, long size, s_view_establishment_message *message)
{
	stream_write_checked(stream, message->mode, 3);
	stream_write_bit(stream, message->value != NONE);
	if (message->value != NONE)
		function_195720(stream, message->value, 32);
}

/* reads view-establishment */
// @retail 0xb24e0
bool __stdcall message_view_establishment_decode(s_bitstream *stream, long size, s_view_establishment_message *message)
{
	message->mode = function_1959c0(stream, 3);
	if (function_1957d0(stream))
		message->value = function_1959c0(stream, 32);
	else
		message->value = NONE;
	if (!stream_overflowed(stream) && message->mode >= 0 && message->mode < 6 && (message->value == NONE || message->value >= 0))
		return true;
	return false;
}

/* writes player-acknowledge: the entries of the players in its mask */
// @retail 0xb2550
void __stdcall message_player_acknowledge_encode(s_bitstream *stream, long size, s_player_acknowledge_message *message)
{
	stream_write_checked(stream, message->mask, 16);
	stream_write_checked(stream, message->acknowledged, 16);
	for (long i = 0; i < 16; i++)
	{
		if (message->mask & (1 << i))
			function_1955d0(stream, message->entries[i], 0x60);
	}
}

/* reads player-acknowledge; only players in the mask may be acknowledged */
// @retail 0xb2600
bool __stdcall message_player_acknowledge_decode(s_bitstream *stream, long size, s_player_acknowledge_message *message)
{
	message->mask = function_1959c0(stream, 16);
	message->acknowledged = function_1959c0(stream, 16);
	for (long i = 0; i < 16; i++)
	{
		if (message->mask & (1 << i))
			function_195820(stream, message->entries[i], 0x60);
	}
	if (!stream_overflowed(stream) && (message->acknowledged & ~message->mask) == 0)
		return true;
	return false;
}

/* registers the view establishment message types */
// @retail 0xb2680
void network_message_types_register_view_establishment(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_view_establishment, "view-establishment", 8, message_view_establishment_encode, message_view_establishment_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_acknowledge, "player-acknowledge", 0xc8, message_player_acknowledge_encode, message_player_acknowledge_decode);
}

/* writes synchronous-update */
// @retail 0xb2710
void __stdcall message_synchronous_update_encode(s_bitstream *stream, long size, s_synchronous_update_message *message)
{
	function_87d00(stream, message);
}

/* reads synchronous-update */
// @retail 0xb2730
bool __stdcall message_synchronous_update_decode(s_bitstream *stream, long size, s_synchronous_update_message *message)
{
	bool result = function_87e90(stream, message);
	if (!stream_overflowed(stream) && result)
		return true;
	return false;
}

/* compares two synchronous-update messages */
// @retail 0xb2770
bool __stdcall message_synchronous_update_compare(long size, s_synchronous_update_message *a, s_synchronous_update_message *b)
{
	return function_88060(a, b);
}

/* writes synchronous-actions: the actions of the players in its mask */
// @retail 0xb2790
void __stdcall message_synchronous_actions_encode(s_bitstream *stream, long size, s_synchronous_actions_message *message)
{
	function_195720(stream, message->value, 32);
	function_195720(stream, message->optional_value + 1, 32);
	stream_write_bit(stream, message->flag);
	stream_write_checked(stream, message->mask, 4);
	for (long i = 0; i < 4; i++)
	{
		if (message->mask & (1 << i))
			function_86f90(stream, &message->actions[i]);
	}
}

/* reads synchronous-actions */
// @retail 0xb2860
bool __stdcall message_synchronous_actions_decode(s_bitstream *stream, long size, s_synchronous_actions_message *message)
{
	bool result = true;
	message->value = function_1959c0(stream, 32);
	message->optional_value = function_1959c0(stream, 32) - 1;
	message->flag = function_1957d0(stream);
	message->mask = function_1959c0(stream, 4);
	for (long i = 0; i < 4; i++)
	{
		if (message->mask & (1 << i))
			result = result && function_874c0(stream, &message->actions[i]);
	}
	if (result && !stream_overflowed(stream) && message->value >= 0 && message->optional_value >= NONE)
		return true;
	return false;
}

/* compares two synchronous-actions messages: the same mask and the same actions */
// @retail 0xb2920
bool __stdcall message_synchronous_actions_compare(long size, s_synchronous_actions_message *a, s_synchronous_actions_message *b)
{
	bool result = true;
	if (a->mask == b->mask)
	{
		for (long i = 0; i < 4; i++)
		{
			if (a->mask & (1 << i))
				result = result && function_87830(&a->actions[i], &b->actions[i]);
		}
	}
	else
		result = false;
	return result;
}

/* writes synchronous-join */
// @retail 0xb2980
void __stdcall message_synchronous_join_encode(s_bitstream *stream, long size, s_synchronous_join_message *message)
{
	function_195720(stream, message->value, 32);
}

/* reads synchronous-join */
// @retail 0xb29a0
bool __stdcall message_synchronous_join_decode(s_bitstream *stream, long size, s_synchronous_join_message *message)
{
	message->value = function_1959c0(stream, 32);
	if (!stream_overflowed(stream) && message->value >= 0)
		return true;
	return false;
}

/* writes synchronous-gamestate */
// @retail 0xb29e0
void __stdcall message_synchronous_gamestate_encode(s_bitstream *stream, long size, s_synchronous_gamestate_message *message)
{
	byte *data = message->data;
	stream_write_checked(stream, message->value, 23);
	stream_write_checked(stream, message->size, 13);
	if (message->size > 0)
		function_1955d0(stream, data, message->size * 8);
}

/* reads synchronous-gamestate; its data must fill the message's size */
// @retail 0xb2a90
bool __stdcall message_synchronous_gamestate_decode(s_bitstream *stream, long size, s_synchronous_gamestate_message *message)
{
	byte *data = message->data;
	message->value = function_1959c0(stream, 23);
	message->size = function_1959c0(stream, 13);
	bool valid = !stream_overflowed(stream) && message->value >= 0 && message->size >= 0 && message->size <= 0xffff && size == message->size + 8;
	if (valid && message->size > 0)
	{
		function_195820(stream, data, message->size * 8);
		if (stream_overflowed(stream))
			valid = false;
		else
			valid = true;
	}
	return valid;
}

/* registers the synchronous message types */
// @retail 0xb2b30
void network_message_types_register_synchronous(c_type_659ceb *collection)
{
	collection->function_x5c51c9(_network_message_type_synchronous_update, "synchronous-update", 0, 0x4048, 0x4048,
		(t_message_encode)message_synchronous_update_encode, (t_message_decode)message_synchronous_update_decode, (t_message_compare)message_synchronous_update_compare);
	collection->function_x5c51c9(_network_message_type_synchronous_actions, "synchronous-actions", 0, 0x180, 0x180,
		(t_message_encode)message_synchronous_actions_encode, (t_message_decode)message_synchronous_actions_decode, (t_message_compare)message_synchronous_actions_compare);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_synchronous_join, "synchronous-join", 4, message_synchronous_join_encode, message_synchronous_join_decode);
	collection->function_x5c51c9(_network_message_type_synchronous_gamestate, "synchronous-gamestate", 1, 8, 0xffff,
		(t_message_encode)message_synchronous_gamestate_encode, (t_message_decode)message_synchronous_gamestate_decode, NULL);
}

/* writes game-results */
// @retail 0xb2c50
void __stdcall message_game_results_encode(s_bitstream *stream, long size, s_game_results_message *message)
{
	function_1955d0(stream, &message->words[0], 32);
	function_1955d0(stream, &message->words[1], 32);
	function_197680(stream, message->results);
}

/* reads game-results */
// @retail 0xb2c80
bool __stdcall message_game_results_decode(s_bitstream *stream, long size, s_game_results_message *message)
{
	function_195820(stream, &message->words[0], 32);
	function_195820(stream, &message->words[1], 32);
	if (function_197d80(stream, message->results))
		return true;
	return false;
}

/* registers the game results message type */
// @retail 0xb2cc0
void network_message_types_register_game_results(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_game_results, "game-results", 0x4fb8, message_game_results_encode, message_game_results_decode);
}

/* writes test */
// @retail 0xb2d10
void __stdcall message_test_encode(s_bitstream *stream, long size, s_test_message *message)
{
	stream_write_bit(stream, message->flag);
	stream_write_checked(stream, message->value, 16);
}

/* reads test */
// @retail 0xb2da0
bool __stdcall message_test_decode(s_bitstream *stream, long size, s_test_message *message)
{
	message->flag = function_1957d0(stream);
	message->value = function_1959c0(stream, 16);
	bool ok = !stream_overflowed(stream);
	return ok;
}

__declspec(noinline) void network_message_types_register_test(c_type_659ceb *collection);

/* registers the test message type */
// @retail 0xb2de0
void network_message_types_register_test(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_test, "test", 8, message_test_encode, message_test_decode);
}
