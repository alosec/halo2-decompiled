// @flags /O2 /Ob1 /arch:SSE /Gr
/* LINK_CONNECTION.CPP: the connections (0xf8 bytes each, at 0x4d87d4)
   between this machine and another over a link, and the two kinds of stream
   they own (lane D). A connection handshakes (connecting, establishing,
   established), hands each packet it reads to its clients, builds packets
   from what its clients write, and closes on a timeout or a client's
   request. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
#include "network_streams.h"
#include <xtl.h>
#include <string.h>

/* a free connection with no streams, clients or address */
// @retail 0x81390
s_link_connection::s_link_connection()
{
	sent_to_all_clients = false;
	read_by_all_clients = false;
	link_list = 0;
	link = 0;
	handler = 0;
	handler_count = 0;
	reliable_stream_index = NONE;
	stream_index = NONE;
	memset(handlers, 0, sizeof(handlers));
	local_sequence = NONE;
	remote_sequence = NONE;
	callback = 0;
	owner = 0;
	flags = 0;
	state = _link_connection_state_free;
	close_reason = 0;
	memset(&address, 0, sizeof(address));
	handshake_next_time = 0;
	link_connection_reset_timers(this);
}

bool g_4d8ba0;                  /* the connection pool exists */
s_connection_counter g_4e6398;  /* the counter every connection timer copies */

s_link g_528000;                /* the link new connections join */
byte g_528b28[4];               /* what new connections send out-of-band messages through */
byte g_529188[4];              /* the message handler new connections get */
s_connection_config g_4cf6d4;   /* the timeouts new connections use */

void function_094bf0(s_network_stream_header *stream);
void function_095cf0(s_network_stream_header *stream);

static inline long network_time_now(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

static inline long network_ticks_since(long time)
{
	return network_time_now() - time;
}

static inline long network_stream_window_space(s_network_stream_header *stream)
{
	s_network_stream_window *window = &stream->window;
	return (window->end - stream->window.next + 0x200) << 5;
}

static inline c_network_reliable_stream *link_connection_reliable_stream(s_link_connection *connection)
{
	return (c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index);
}

/* claims the first free reliable stream for an owner; NONE when all are in use */
// @retail 0x820f0
long network_reliable_stream_allocate(long owner)
{
	long result = NONE;

	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (!network_reliable_stream_get(i)->active)
			{
				s_network_stream_header *stream = network_reliable_stream_get(i);
				function_095cf0(stream);
				stream->owner = owner;
				stream->active = true;
				result = i;
				break;
			}
		}
	}
	return result;
}

/* claims the first free unreliable stream for an owner; NONE when all are in use */
// @retail 0x82150
long network_stream_allocate(long owner)
{
	long result = NONE;

	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (!network_stream_get(i)->active)
			{
				s_network_stream_header *stream = network_stream_get(i);
				function_094bf0(stream);
				stream->owner = owner;
				stream->active = true;
				stream->unknown0c = (void *)0x528588;
				result = i;
				break;
			}
		}
	}
	return result;
}

/* whether a set of connection flags is consistent */
// @retail 0x880b0
bool link_connection_flags_valid(dword flags)
{
	bool valid = !(flags & 0xffffff00) && (!(flags & 1) || !(flags & 2)) && (!(flags & 0x40) || !(flags & 0x80));
	if (flags & 4)
		valid = valid && !(flags & 8);
	if (!(flags & 8))
		valid = valid && !(flags & 0x30);
	if (!(flags & 0x10))
		valid = valid && !(flags & 0x20);
	return valid;
}

/* closes a connection, telling the other machine why unless the reason is 6 */
// @retail 0x88650
void link_connection_close(s_link_connection *connection, long reason)
{
	if (connection->state == _link_connection_state_established && reason != 6)
	{
		struct
		{
			long remote_sequence;
			long local_sequence;
			long reason;
		} message;

		message.local_sequence = connection->local_sequence;
		message.remote_sequence = connection->remote_sequence;
		message.reason = reason;
		function_07b140(connection->link, (long)&connection->address, 7, sizeof(message), &message);
	}
	if (connection->callback)
		connection->callback->function(connection->callback->context);
	link_remove_entry(connection->link_list, connection->id);
	connection->previous_address = connection->address;
	connection->state = _link_connection_state_closed;
	connection->close_reason = reason;
	connection->local_sequence = NONE;
	connection->remote_sequence = NONE;
}

/* closes a connection if it is open, then frees it and its streams */
// @retail 0x886e0
void link_connection_dispose(s_link_connection *connection)
{
	if (connection->state > _link_connection_state_closed)
		link_connection_close(connection, 3);
	connection->owner = 0;
	if (connection->stream_index != NONE)
	{
		s_network_stream_header *stream = network_stream_get(connection->stream_index);
		function_094bf0(stream);
		stream->active = false;
		stream->owner = 0;
		connection->stream_index = NONE;
	}
	if (connection->reliable_stream_index != NONE)
	{
		s_network_stream_header *stream = network_reliable_stream_get(connection->reliable_stream_index);
		stream->active = false;
		stream->owner = 0;
		connection->reliable_stream_index = NONE;
	}
	connection->state = _link_connection_state_free;
}

/* restarts every timer of a connection */
// @retail 0x88d20
void link_connection_reset_timers(s_link_connection *connection)
{
	for (long i = 0; i < k_link_connection_timer_count; i++)
	{
		connection->timers[i].time = network_time_now();
		connection->timers[i].counter = g_4e6398;
	}
}

/* restarts one timer of a connection */
// @retail 0x88d70
void link_connection_reset_timer(s_link_connection *connection, long index)
{
	connection->timers[index].time = network_time_now();
	connection->timers[index].counter = g_4e6398;
}

/* the bytes the reliable stream has sent and not had acknowledged */
// @retail 0x88fd0
long link_connection_get_reliable_bytes(s_link_connection *connection)
{
	long result = 0;

	if (connection->state == _link_connection_state_established && (connection->flags & _link_connection_reliable_stream_flag))
		result = link_connection_reliable_stream(connection)->m_bytes;
	return result;
}

/* the reliable messages not yet acknowledged */
// @retail 0x89000
long link_connection_get_reliable_pending(s_link_connection *connection)
{
	long result = 0;

	if (connection->state == _link_connection_state_established && (connection->flags & _link_connection_reliable_stream_flag))
	{
		c_network_reliable_stream *stream = link_connection_reliable_stream(connection);
		result = stream->m_message_window.newest - stream->m_next_sequence;
	}
	return result;
}

/* the reliable messages that may still be sent before the window fills */
// @retail 0x89030
long link_connection_get_reliable_window(s_link_connection *connection)
{
	long result = 0;

	if (connection->state == _link_connection_state_established && (connection->flags & _link_connection_reliable_stream_flag))
	{
		c_network_reliable_stream *stream = link_connection_reliable_stream(connection);
		result = stream->m_last_sequence - stream->m_message_window.newest + 0x80;
	}
	return result;
}

/* the bits the unreliable stream may still send */
// @retail 0x89070
long link_connection_send_capacity(s_link_connection *connection)
{
	long result = 0;

	if (connection->state == _link_connection_state_established && (connection->flags & _link_connection_unreliable_stream_flag))
	{
		s_network_stream_window *window = &network_stream_get(connection->stream_index)->window;
		result = (window->end - window->next + 0x200) << 5;
	}
	return result;
}

/* times a handshake out, or resends it once its retry interval has passed */
// @retail 0x890b0
void link_connection_update_handshake(s_link_connection *connection)
{
	if (network_ticks_since(connection->handshake_time) >= connection->config->connect_timeout)
	{
		link_connection_close(connection, 4);
		return;
	}
	if (network_ticks_since(connection->handshake_next_time) >= 0 && connection->handshake_count < connection->config->retry_count)
	{
		struct
		{
			long sequence;
			dword flags;
		} message;

		message.flags = connection->flags;
		message.sequence = connection->local_sequence;
		function_07b140(connection->link, (long)&connection->address, 4, sizeof(message), &message);
		connection->handshake_next_time = network_time_now() + connection->config->retry_interval;
	}
}

/* acknowledges the other machine's handshake, through the unreliable stream when there is one */
// @retail 0x89180
void link_connection_send_acknowledge(s_link_connection *connection, bool reliable)
{
	struct
	{
		long remote_sequence;
		long local_sequence;
	} message;

	message.local_sequence = connection->local_sequence;
	message.remote_sequence = connection->remote_sequence;
	if (connection->flags & _link_connection_unreliable_stream_flag)
	{
		if (reliable)
			function_095580(network_stream_get(connection->stream_index), 6, sizeof(message), &message);
	}
	else
	{
		function_07b140(connection->link, (long)&connection->address, 6, sizeof(message), &message);
	}
}

/* answers a handshake: a connecting connection starts establishing */
// @retail 0x88360
void link_connection_establish(s_link_connection *connection, long remote_sequence)
{
	bool established = false;

	if (connection->state == _link_connection_state_connecting)
	{
		connection->state = _link_connection_state_establishing;
		connection->establish_time = network_time_now();
		connection->remote_sequence = remote_sequence;
		established = true;
		link_connection_reset_timers(connection);
		if (!(connection->flags & _link_connection_unreliable_stream_flag))
			connection->state = _link_connection_state_established;
	}
	link_connection_send_acknowledge(connection, established);
}

/* ---- reading a connection's packets ---- */

#include "bitstream.h"
#include "unknown_0662e0.h"

/* the clients a connection hands its packets to, and the owner that hears
   about its traffic; their vtables are in the client code */
class c_connection_client
{
public:
	virtual void v0() {}
	virtual bool should_close(long *reason) { return false; }
	virtual bool has_data(bool *reliable) { return false; }
	virtual long reserve_bits(long capacity, long available) { return 0; }
	virtual bool write_packet(long sequence, s_bitstream *stream, long bits, long remaining) { return false; }
	virtual long read_packet(long *sequence, s_bitstream *stream) { return 0; }
	virtual void message_delivered(long sequence) {}
	virtual void message_lost(long sequence, bool resent) {}
};

class c_connection_owner
{
public:
	virtual void packet_sent(long connection_id, long size, bool wrote) {}
	virtual void packet_received(long connection_id, long packet_size) {}
	virtual void message_acknowledged(long connection_id, long size, long time) {}
	virtual void message_lost(long connection_id, bool resent, bool late) {}
	virtual void v4() {}
	virtual bool build_packet(long connection_id, bool ordinary, bool priority, bool reliable,
		bool *send_reliable, bool *pad, long *capacity, long *extra_size, long maximum_extra, void *extra) { return false; }
};

/* src/network_streams.cpp (lane J) */
bool __stdcall function_096ce0(c_network_reliable_stream *stream, bool force, long *type, long *sequence);
bool __stdcall function_095840(s_network_stream_header *stream, long *message_type, long *message_size, void *message);

/* src/unknown_075800.cpp (lane J) */
class c_class_938e0
{
public:
	void function_93aa0(long channel_index, long message_type, long message_size, const void *message);
};

/* the bit stream module (src/unknown_195720.cpp) */
bool function_1946f0(s_bitstream *stream);

/* 0x1947a0, which retail inlines here */
static inline void stream_begin_reading(s_bitstream *stream)
{
	stream->mode = 3;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (function_1959c0(stream, 32) == 0x64656267)
	{
		stream->error = true;
		return;
	}
	stream->bit_position = 0;
	stream->error = false;
}

/* the clients of a connection with all the bits of type set: first the
   connection's own, then its class's (index with the top bit set) */
struct s_connection_client_iterator
{
	dword type;
	long index;
	long absolute_index;
	dword client_type;
	c_connection_client *client;
};

static inline void connection_client_iterator_new(s_connection_client_iterator *iterator, dword type)
{
	iterator->type = type;
	iterator->index = NONE;
	iterator->absolute_index = NONE;
	iterator->client_type = 0;
	iterator->client = NULL;
}

/* advances an iterator to the next client handling all of its flags */
// @retail 0x891e0
bool link_connection_next_client(s_link_connection *connection, s_connection_client_iterator *iterator)
{
	long index = iterator->index;
	long absolute_index = iterator->absolute_index;
	s_connection_handler *handler;
	do
	{
		bool started = false;
		handler = NULL;
		if (index == NONE)
		{
			index = 0;
			absolute_index = 0;
			started = true;
		}
		if (!(index & 0x80000000))
		{
			long local_index = index & 0x7fffffff;
			if (!started)
			{
				local_index++;
				started = true;
				index = local_index & 0x7fffffff;
				absolute_index = local_index;
			}
			if (local_index >= 0 && local_index < connection->handler_count)
			{
				handler = &connection->handlers[local_index];
			}
			else
			{
				absolute_index = connection->handler_count;
				index = 0x80000000;
				started = true;
			}
		}
		if (!handler && (index & 0x80000000))
		{
			s_connection_handler *handlers = NULL;
			long count = 0;
			long local_index = index & 0x7fffffff;
			if (!started)
			{
				absolute_index = connection->handler_count;
				local_index++;
				index = local_index | 0x80000000;
				absolute_index += local_index;
			}
			if (connection->callback)
			{
				handlers = connection->callback->handlers;
				count = connection->callback->handler_count;
			}
			if (local_index >= 0 && local_index < count)
			{
				handler = &handlers[local_index];
			}
			else
			{
				index = NONE;
				absolute_index = NONE;
			}
		}
	} while (index != NONE && (handler->type & iterator->type) != iterator->type);
	iterator->index = index;
	iterator->absolute_index = absolute_index;
	if (index == NONE)
	{
		iterator->client = NULL;
		iterator->client_type = 0;
	}
	else
	{
		iterator->client = handler->client;
		iterator->client_type = handler->type;
	}
	return iterator->index != NONE;
}

/* drains the reliable stream's events, telling clients and the owner of delivered and lost messages */
// @retail 0x88db0
void link_connection_update_reliable_stream(s_link_connection *connection)
{
	if (connection->flags & _link_connection_reliable_stream_flag)
	{
		c_network_reliable_stream *stream = (c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index);
		{
			long type;
			long sequence;
			while (function_096ce0(stream, false, &type, &sequence))
				;
		}
		long message_sequence;
		long size;
		long time;
		long event;
		for (event = stream->function_965e0(&message_sequence, &size, &time); event; event = stream->function_965e0(&message_sequence, &size, &time))
		{
			if (event == 4 || event == 1)
			{
				connection->timers[_link_connection_timer_acknowledged].time = network_time_now();
				connection->timers[_link_connection_timer_acknowledged].counter = g_4e6398;
				if (connection->state == _link_connection_state_establishing)
					connection->state = _link_connection_state_established;
				if (connection->owner)
					connection->owner->message_acknowledged(connection->id, size, time);
			}
			if (event == 4)
			{
				s_connection_client_iterator iterator;
				connection_client_iterator_new(&iterator, _connection_client_delivered_flag);
				while (link_connection_next_client(connection, &iterator))
					iterator.client->message_delivered(message_sequence);
			}
			else if (event == 1 || event == 2 || event == 3)
			{
				bool resent = event != 3;
				s_connection_client_iterator iterator;
				connection_client_iterator_new(&iterator, _connection_client_lost_flag);
				while (link_connection_next_client(connection, &iterator))
					iterator.client->message_lost(message_sequence, resent);
				if (connection->owner)
				{
					bool late = false;
					if (resent)
					{
						real scaled_real = (real)stream->m_round_trip_minimum * g_network_configuration.real16e8;
						long scaled;
						__asm
						{
							fld scaled_real
							fistp scaled
						}
						long padded = stream->m_round_trip_minimum + g_network_configuration.value16ec;
						if (scaled > padded)
							padded = scaled;
						if (time >= padded)
							late = true;
					}
					else
					{
						late = true;
					}
					connection->owner->message_lost(connection->id, resent, late);
				}
			}
		}
	}
}

/* hands a packet to the connection's clients, then reads the unreliable stream's messages */
// @retail 0x88750
bool link_connection_read_packet(s_link_connection *connection, s_bitstream *stream, long packet_size, bool out_of_band)
{
	long sequence = NONE;
	bool result = false;
	long status = 0;
	long message_type;
	long message_size;
	s_connection_client_iterator iterator;
	byte data[0x600];
	byte message[0x10000];

	if (!out_of_band)
		stream_begin_reading(stream);
	dword type;
	if (connection->callback && connection->callback->include_all_clients)
	{
		type = 0;
		connection->read_by_all_clients = true;
	}
	else
	{
		type = 1;
		connection->read_by_all_clients = false;
	}
	connection_client_iterator_new(&iterator, type);
	while (link_connection_next_client(connection, &iterator))
	{
		status = iterator.client->read_packet(&sequence, stream);
		if (status)
			break;
	}
	if (!status && function_1957d0(stream))
	{
		long size = function_1959c0(stream, 14);
		if (size > 0 && size <= 0x3000)
		{
			function_195820(stream, data, size);
			if (function_1946f0(stream))
				status = 3;
		}
		else
		{
			status = 3;
		}
	}
	if (stream_overflowed(stream))
		status = 3;
	switch (status)
	{
	case 2:
		break;
	case 3:
		break;
	case 1:
		stream->bit_position = 8 * stream->size_in_bytes;
	case 0:
		stream->mode = 5;
		result = true;
		if (!out_of_band)
		{
			link_connection_reset_timer(connection, _link_connection_timer_packet_received);
			link_connection_update_reliable_stream(connection);
			if (connection->flags & _link_connection_reliable_stream_flag)
				((c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index))->mark_received(sequence);
			if (connection->flags & _link_connection_unreliable_stream_flag)
			{
				s_network_stream_header *unreliable = network_stream_get(connection->stream_index);
				while (function_095840(unreliable, &message_type, &message_size, message))
					connection->handler->function_93aa0(connection->id, message_type, message_size, message);
			}
		}
		break;
	default:
		__assume(0);
	}
	if (connection->owner && !out_of_band)
		connection->owner->packet_received(connection->id, packet_size);
	return result;
}

/* src/unknown_092870.cpp (lane J) */
class c_class_93590;
bool network_link_add_route(c_class_93590 *link, long connection_index, long kind, s_type_99af70 const *address);

static inline bool transport_address_is_loopback_inline(s_type_99af70 const *address)
{
	bool result = false;
	if (address->address_length == k_ipv4_address_length)
		result = address->ipv4_address == 0x7f000001;
	return result;
}

/* the link's next connection sequence number (never NONE) */
static inline long link_next_sequence(s_link *link)
{
	long sequence = link->sequence;
	link->sequence = sequence + 1;
	if (link->sequence == NONE)
		link->sequence = 0;
	return sequence;
}

/* starts connecting to an address: the initiator sends the handshake */
// @retail 0x88220
void link_connection_connect(s_type_99af70 const *address, s_link_connection *connection, bool initiator)
{
	connection->address = *address;
	connection->state = _link_connection_state_connecting;
	connection->initiator = initiator;
	if (!transport_address_is_loopback_inline(address))
		connection->flags &= ~_link_connection_loopback_flags;
	long sequence = link_next_sequence(connection->link_list);
	connection->local_sequence = sequence;
	connection->remote_sequence = NONE;
	if (network_link_add_route((c_class_93590 *)connection->link_list, connection->id, sequence, &connection->address))
	{
		if (connection->initiator)
		{
			connection->handshake_time = network_time_now();
			connection->handshake_next_time = network_time_now();
			connection->handshake_count = 0;
			link_connection_update_handshake(connection);
		}
		link_connection_reset_timers(connection);
		if (connection->reliable_stream_index != NONE)
			function_095cf0(network_reliable_stream_get(connection->reliable_stream_index));
		if (connection->stream_index != NONE)
			function_094bf0(network_stream_get(connection->stream_index));
	}
	else
	{
		link_connection_close(connection, 2);
	}
}

static inline void connection_add_handler(s_link_connection *connection, dword type, c_connection_client *client)
{
	s_connection_handler *handler = &connection->handlers[connection->handler_count];
	handler->type = type;
	handler->client = client;
	connection->handler_count++;
}

/* sets a connection up: its link, its handler and configuration, and the
   streams its flags ask for (the reliable stream, the unreliable stream and
   the connection's own client) */
// @retail 0x88110
bool link_connection_initialize(s_link_connection *connection, long id, dword flags, s_link *link_list, void *link, c_class_938e0 *handler, s_connection_config const *config)
{
	bool result = false;
	connection->state = _link_connection_state_initialized;
	connection->close_reason = 0;
	memset(&connection->previous_address, 0, sizeof(connection->previous_address));
	connection->id = id;
	connection->flags = flags;
	connection->link_list = link_list;
	connection->link = link;
	connection->handler = handler;
	connection->local_sequence = NONE;
	connection->remote_sequence = NONE;
	connection->config = config;
	connection->handler_count = 0;
	memset(connection->handlers, 0, sizeof(connection->handlers));
	connection->callback = 0;
	connection->owner = 0;
	result = true;
	if (connection->flags & _link_connection_reliable_stream_flag)
	{
		connection->reliable_stream_index = network_reliable_stream_allocate(0);
		if (connection->reliable_stream_index == NONE)
			result = false;
		else
			connection_add_handler(connection, _connection_client_packets_flag | _connection_client_can_close_flag | _connection_client_priority_flag, (c_connection_client *)network_reliable_stream_get(connection->reliable_stream_index));
	}
	if (result && (connection->flags & _link_connection_unreliable_stream_flag))
	{
		connection->stream_index = network_stream_allocate(0);
		if (connection->stream_index == NONE)
			result = false;
		else
			connection_add_handler(connection, _connection_client_packets_flag | _connection_client_lost_flag | _connection_client_can_close_flag, (c_connection_client *)network_stream_get(connection->stream_index));
	}
	if (result && (connection->flags & _link_connection_own_client_flag))
	{
		connection_add_handler(connection, _connection_client_packets_flag, (c_connection_client *)&connection->own_client);
	}
	if (!result)
		link_connection_dispose(connection);
	return result;
}

/* sets up the first free connection with the given flags; its index, or NONE */
// @retail 0x82060
long link_connection_allocate(long unused_owner, dword flags)
{
	long result = NONE;
	dword const *flags_reference = &flags;
	long const *owner_reference = &unused_owner;
	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (link_connection_get(i)->state == _link_connection_state_free)
			{
				if (link_connection_initialize(link_connection_get(i), i, flags, &g_528000,
					g_528b28, (c_class_938e0 *)g_529188, &g_4cf6d4))
					result = i;
				break;
			}
		}
	}
	return result;
}

typedef void (__stdcall *connection_callback_function)(void *context);

/* Retail's only caller keeps the callback pointer in a saved register across this call. */
__declspec(noinline) void link_connection_callback_initialize(s_connection_callback *callback,
	c_connection_client *const *clients, void *context, connection_callback_function function, long count,
	const dword *types, bool active);

/* fills in a connection class: its close callback and its clients */
// @retail 0x892f0
inline void link_connection_callback_initialize(s_connection_callback *callback, c_connection_client *const *clients,
	void *context, connection_callback_function function, long count, const dword *types, bool active)
{
	callback->active = active;
	callback->context = context;
	callback->initialized = true;
	callback->function = function;
	callback->handler_count = count;
	for (long i = 0; i < count; i++)
	{
		s_connection_handler *handler = &callback->handlers[i];
		handler->type = types[i];
		handler->client = clients[i];
	}
	callback->include_all_clients = false;
}

class c_class_93590;
void network_link_send_connection_packet(c_class_93590 *link, long connection_index, const s_bitstream *stream,
	long extra_size, const void *extra, long *size_out);
void reliable_stream_set_message_size(c_network_reliable_stream *stream, long sequence, long size);

static __forceinline void packet_stream_begin_writing(s_bitstream *stream)
{
	stream->mode = 1;
	stream->unknown08 = 8;
	memset(stream->data, 0, stream->size_in_bytes);
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (stream->mode == 1)
	{
		stream->unknown2c = 0;
		stream->unknown30 = 0;
	}
	else if (stream->mode == 3 || stream->mode == 4)
	{
		if (function_1959c0(stream, 32) == 0x64656267)
		{
			stream->error = true;
		}
		else
		{
			stream->bit_position = 0;
			stream->error = false;
		}
	}
}

static __forceinline void packet_stream_write_bit(s_bitstream *stream, bool value)
{
	if ((stream->size_in_bytes << 3) - stream->bit_position >= 1 && value)
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	stream->bit_position++;
}

static __forceinline void packet_stream_write_value(s_bitstream *stream, dword value, long bits)
{
	if (bits < 32 && value >= (dword)(1 << bits))
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", value, 1 << bits);
	}
	function_195720(stream, value, bits);
}

/* builds a packet from what each client writes and sends it over the link */
// @retail 0x88980
void link_connection_send_packet(bool reliable, s_link_connection *connection, s_bitstream *stream, bool pad,
	long extra_size, const void *extra, long *packet_size, long *stream_size, long *sent_extra_size)
{
	long sequence = NONE;
	bool wrote = false;
	long sent = 0;
	long bytes = 0;
	long extra_bytes = 0;
	bool ready = true;
	if (reliable)
	{
		c_network_reliable_stream *reliable_stream = (c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index);
		sequence = reliable_stream->allocate_sequence(network_time_now());
		ready = sequence != NONE;
	}
	if (ready)
	{
		long total = 0;
		packet_stream_begin_writing(stream);
		long capacity = (stream->size_in_bytes << 3) - stream->bit_position;
		dword type;
		if (connection->callback && connection->callback->include_all_clients)
		{
			type = 0;
			connection->sent_to_all_clients = true;
		}
		else
		{
			type = 1;
			connection->sent_to_all_clients = false;
		}
		long reserved[7];
		s_connection_client_iterator iterator;
		connection_client_iterator_new(&iterator, type);
		memset(reserved, 0, sizeof(reserved));
		while (link_connection_next_client(connection, &iterator))
		{
			long bits = iterator.client->reserve_bits(capacity, capacity - total);
			total += bits;
			reserved[iterator.absolute_index] = bits;
		}
		total++;
		if (total > capacity)
			goto done;
		connection_client_iterator_new(&iterator, type);
		while (link_connection_next_client(connection, &iterator))
		{
			long bits = reserved[iterator.absolute_index];
			total -= bits;
			if (iterator.client->write_packet(sequence, stream, bits, total))
				wrote = true;
			if ((stream->size_in_bytes << 3) - stream->bit_position < total)
				goto done;
		}
		if (pad && (stream->size_in_bytes << 3) - stream->bit_position > 15)
		{
			long bits = (stream->size_in_bytes << 3) - stream->bit_position - 15;
			long count = (bits + 7) / 8;
			byte padding[0x600];
			packet_stream_write_bit(stream, true);
			packet_stream_write_value(stream, bits, 14);
			memset(padding, 0, count);
			function_1955d0(stream, padding, bits);
		}
		else
		{
			packet_stream_write_bit(stream, false);
		}
		long size = (stream->bit_position + 7) / 8;
		long alignment = stream->unknown08;
		stream->size_in_bytes = size;
		if (size % alignment)
			stream->size_in_bytes = size + alignment - size % alignment;
		stream->mode = 2;
		network_link_send_connection_packet((c_class_93590 *)connection->link_list, connection->id, stream, extra_size, extra, &sent);
		if (sent > 0)
		{
			bytes = stream->size_in_bytes;
			extra_bytes = extra_size;
		}
		if (sequence != NONE)
			reliable_stream_set_message_size((c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index), sequence, sent);
		if (connection->owner)
			connection->owner->packet_sent(connection->id, sent, wrote);
	}
done:
	if (packet_size)
		*packet_size = sent;
	if (stream_size)
		*stream_size = bytes;
	if (sent_extra_size)
		*sent_extra_size = extra_bytes;
}

/* times out handshakes, closes on a client's request, and sends a packet when clients have data */
// @retail 0x883c0
void __stdcall link_connection_update(s_link_connection *connection)
{
 if (connection->state == _link_connection_state_connecting && connection->initiator)
  link_connection_update_handshake(connection);
 if (connection->state == _link_connection_state_establishing && network_ticks_since(connection->establish_time) > connection->config->establish_timeout)
  link_connection_close(connection, 7);
 if (connection->state >= _link_connection_state_establishing)
 {
  s_connection_client_iterator iterator;
  connection_client_iterator_new(&iterator, _connection_client_can_close_flag);
  while (link_connection_next_client(connection, &iterator))
  {
   long reason = 0;
   if (iterator.client->should_close(&reason))
   {
    link_connection_close(connection, reason);
    break;
   }
  }
 }
 if (connection->state >= _link_connection_state_establishing)
 {
  bool ordinary = false;
  bool priority = false;
  bool reliable = false;
  s_connection_client_iterator iterator;
  dword type = connection->callback && connection->callback->include_all_clients ? 0 : 1;
  connection_client_iterator_new(&iterator, type);
  while (link_connection_next_client(connection, &iterator))
  {
   bool client_reliable = false;
   if (iterator.client->has_data(&client_reliable))
   {
    if (iterator.client_type & _connection_client_priority_flag)
     priority = true;
    else
     ordinary = true;
    if (client_reliable)
     reliable = true;
   }
  }
  if (!(connection->flags & _link_connection_reliable_stream_flag) && reliable)
   reliable = false;
  bool send_reliable;
  bool pad;
  long capacity;
  long extra_size;
  byte extra[0x200];
  bool send;
  if (connection->owner)
   send = connection->owner->build_packet(connection->id, ordinary, priority, reliable,
    &send_reliable, &pad, &capacity, &extra_size, sizeof(extra), extra);
  else
  {
   send = ordinary || priority;
   send_reliable = reliable;
   pad = false;
   capacity = 0x600;
   extra_size = 0;
  }
  if (send)
  {
   byte payload[0x600];
   s_bitstream stream;
   stream.data = payload;
   stream.size_in_bytes = capacity;
   stream.unknown08 = 1;
   stream.mode = 0;
   stream.bit_position = 0;
   stream.checkpoint_count = 0;
   stream.error = false;
   long packet_size;
   long stream_size;
   long sent_extra_size;
   link_connection_send_packet(send_reliable, connection, &stream, pad, extra_size,
    extra_size > 0 ? extra : 0, &packet_size, &stream_size, &sent_extra_size);
   link_connection_reset_timer(connection, _link_connection_timer_packet_sent);
   if (send_reliable)
    link_connection_reset_timer(connection, _link_connection_timer_reliable_sent);
   if (extra_size > 0)
    link_connection_reset_timer(connection, _link_connection_timer_extra_sent);
  }
 }
 if (connection->state >= _link_connection_state_establishing)
  link_connection_update_reliable_stream(connection);
}
