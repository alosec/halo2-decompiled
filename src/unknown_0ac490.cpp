// @flags /O2 /Gr
/* UNKNOWN_0AC490.CPP: the network message codecs of the discovery (ping,
   pong, broadcast) and connection (connect-*) families, and the functions
   that register them */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "network_message_types.h"

struct s_message_ping
{
	word id;
	byte unknown02[2];
	long timestamp;
	bool flag;
};

struct s_message_pong
{
	word id;
	byte unknown02[2];
	long timestamp;
	long state;
};

struct s_message_broadcast_search
{
	word id;
	byte unknown02[2];
	byte nonce[8];
};

struct s_message_broadcast_reply
{
	word id;
	byte unknown02[2];
	byte nonce[8];
	byte session[0x714];
};

struct s_message_connect_request
{
	dword identifier;
	long channel;
};

struct s_message_connect_refuse
{
	dword identifier;
	long reason;
};

struct s_message_connect_establish
{
	dword identifier;
	dword remote_identifier;
};

struct s_message_connect_closed
{
	dword identifier;
	dword remote_identifier;
	long reason;
};

// @retail 0x000ac490
void __stdcall message_ping_encode(s_bitstream *stream, long size, s_message_ping *message)
{
	stream_write_checked(stream, message->id, 16);
	function_195720(stream, message->timestamp, 32);
	stream_write_bit(stream, message->flag);
}

// @retail 0x000ac530
bool __stdcall message_ping_decode(s_bitstream *stream, long size, s_message_ping *message)
{
	message->id = (word)function_1959c0(stream, 16);
	message->timestamp = function_1959c0(stream, 32);
	message->flag = function_1957d0(stream);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0x000ac580
void __stdcall message_pong_encode(s_bitstream *stream, long size, s_message_pong *message)
{
	stream_write_checked(stream, message->id, 16);
	function_195720(stream, message->timestamp, 32);
	stream_write_checked(stream, message->state, 2);
}

// @retail 0x000ac610
bool __stdcall message_pong_decode(s_bitstream *stream, long size, s_message_pong *message)
{
	message->id = (word)function_1959c0(stream, 16);
	message->timestamp = function_1959c0(stream, 32);
	message->state = function_1959c0(stream, 2);
	if (stream_overflowed(stream) || message->state < 0 || message->state >= 3)
		return false;
	return true;
}

// @retail 0x000ac670
void __stdcall message_broadcast_search_encode(s_bitstream *stream, long size, s_message_broadcast_search *message)
{
	stream_write_checked(stream, message->id, 16);
	function_1955d0(stream, &message->nonce, 64);
}

// @retail 0x000ac6e0
bool __stdcall message_broadcast_search_decode(s_bitstream *stream, long size, s_message_broadcast_search *message)
{
	message->id = (word)function_1959c0(stream, 16);
	function_195820(stream, &message->nonce, 64);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0x000ac730
void __stdcall message_broadcast_reply_encode(s_bitstream *stream, long size, s_message_broadcast_reply *message)
{
	stream_write_checked(stream, message->id, 16);
	function_1955d0(stream, &message->nonce, 64);
	session_description_encode(stream, &message->session);
}

// @retail 0x000ac7a0
bool __stdcall message_broadcast_reply_decode(s_bitstream *stream, long size, s_message_broadcast_reply *message)
{
	message->id = (word)function_1959c0(stream, 16);
	function_195820(stream, &message->nonce, 64);
	if (session_description_decode(stream, &message->session) && !stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0x000ac800
void network_message_types_register_discovery(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_ping, "ping", sizeof(s_message_ping), message_ping_encode, message_ping_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_pong, "pong", sizeof(s_message_pong), message_pong_encode, message_pong_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_broadcast_search, "broadcast-search", sizeof(s_message_broadcast_search), message_broadcast_search_encode, message_broadcast_search_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_broadcast_reply, "broadcast-reply", sizeof(s_message_broadcast_reply), message_broadcast_reply_encode, message_broadcast_reply_decode);
}

// @retail 0x000ac8a0
void __stdcall message_connect_request_encode(s_bitstream *stream, long size, s_message_connect_request *message)
{
	function_195720(stream, message->identifier, 32);
	stream_write_checked(stream, message->channel, 8);
}

// @retail 0x000ac900
bool __stdcall message_connect_request_decode(s_bitstream *stream, long size, s_message_connect_request *message)
{
	message->identifier = function_1959c0(stream, 32);
	message->channel = function_1959c0(stream, 8);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0x000ac940
void __stdcall message_connect_refuse_encode(s_bitstream *stream, long size, s_message_connect_refuse *message)
{
	function_195720(stream, message->identifier, 32);
	stream_write_checked(stream, message->reason, 3);
}

// @retail 0x000ac9a0
bool __stdcall message_connect_refuse_decode(s_bitstream *stream, long size, s_message_connect_refuse *message)
{
	message->identifier = function_1959c0(stream, 32);
	message->reason = function_1959c0(stream, 3);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0x000ac9e0
void __stdcall message_connect_establish_encode(s_bitstream *stream, long size, s_message_connect_establish *message)
{
	function_195720(stream, message->identifier, 32);
	function_195720(stream, message->remote_identifier, 32);
}

// @retail 0x000aca00
bool __stdcall message_connect_establish_decode(s_bitstream *stream, long size, s_message_connect_establish *message)
{
	message->identifier = function_1959c0(stream, 32);
	message->remote_identifier = function_1959c0(stream, 32);
	return stream_overflowed(stream) ? false : true;
}

// @retail 0x000aca40
void __stdcall message_connect_closed_encode(s_bitstream *stream, long size, s_message_connect_closed *message)
{
	function_195720(stream, message->identifier, 32);
	function_195720(stream, message->remote_identifier, 32);
	stream_write_checked(stream, message->reason, 5);
}

// @retail 0x000acab0
bool __stdcall message_connect_closed_decode(s_bitstream *stream, long size, s_message_connect_closed *message)
{
	message->identifier = function_1959c0(stream, 32);
	message->remote_identifier = function_1959c0(stream, 32);
	message->reason = function_1959c0(stream, 5);
	if (stream_overflowed(stream) || message->reason < 0 || message->reason >= 18)
		return false;
	return true;
}

// @retail 0x000acb10
void network_message_types_register_connection(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_connect_request, "connect-request", sizeof(s_message_connect_request), message_connect_request_encode, message_connect_request_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_connect_refuse, "connect-refuse", sizeof(s_message_connect_refuse), message_connect_refuse_encode, message_connect_refuse_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_connect_establish, "connect-establish", sizeof(s_message_connect_establish), message_connect_establish_encode, message_connect_establish_decode);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_connect_closed, "connect-closed", sizeof(s_message_connect_closed), message_connect_closed_encode, message_connect_closed_decode);
}
