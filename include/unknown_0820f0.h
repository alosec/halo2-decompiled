/* UNKNOWN_0820F0.H: the connections (0xf8 bytes each, at 0x4d87d4) and
   the two kinds of stream they own (lane D, src/link_connection.cpp) */

#ifndef UNKNOWN_0820F0_H
#define UNKNOWN_0820F0_H

#include "unknown_11c920.h"
#include "unknown_07aec0.h"
#include "unknown_08b110.h"

/* a link's list of connections (src/unknown_092e00.cpp) */
struct s_link_entry
{
	long id;
	byte unknown04[0x1c];
};

struct s_link
{
	byte unknown00[4];
	long sequence;
	byte unknown08[0x18];
	long entry_count;
	s_link_entry entries[1];
};

/* the objects that read a connection's packets and hear about its messages
   (src/link_connection.cpp) */
class c_connection_client;
class c_connection_owner;

/* what a client handles: the bits of s_connection_handler::type */
enum e_connection_client_flags
{
	_connection_client_packets_flag = 0x1,        /* reads and writes packets */
	_connection_client_delivered_flag = 0x4,      /* hears of delivered messages */
	_connection_client_lost_flag = 0x8,           /* hears of lost messages */
	_connection_client_can_close_flag = 0x10,     /* may ask to close the connection */
	_connection_client_priority_flag = 0x20       /* its data makes a priority packet */
};

/* s_link_connection::flags */
enum e_link_connection_flags
{
	_link_connection_reliable_stream_flag = 0x8,
	_link_connection_unreliable_stream_flag = 0x10,
	_link_connection_own_client_flag = 0x20,
	_link_connection_loopback_flags = 0xc0      /* kept only for a loopback address */
};

/* s_link_connection::state */
enum e_link_connection_state
{
	_link_connection_state_free,
	_link_connection_state_initialized,
	_link_connection_state_closed,
	_link_connection_state_connecting,
	_link_connection_state_establishing,
	_link_connection_state_established
};

/* s_link_connection::timers */
enum e_link_connection_timer
{
	_link_connection_timer_packet_sent,
	_link_connection_timer_reliable_sent,
	_link_connection_timer_extra_sent,
	_link_connection_timer_packet_received,
	_link_connection_timer_acknowledged,
	_link_connection_timer_unknown5,

	k_link_connection_timer_count
};

struct s_connection_handler
{
	dword type;
	c_connection_client *client;
};

/* the connection's class: its close callback and the clients every
   connection of the class has */
struct s_connection_callback
{
	bool initialized;
	byte unknown01[3];
	void *context;
	void (__stdcall *function)(void *context);
	long handler_count;
	s_connection_handler handlers[4];
	bool active;
	bool include_all_clients;  /* every client takes part in packets, not only the packet clients */
};

struct s_connection_config
{
	long retry_interval;
	long retry_count;
	long connect_timeout;
	long establish_timeout;
};

struct s_connection_counter
{
	long low;
	long high;
};

struct s_connection_timer
{
	long time;
	long unknown04;
	s_connection_counter counter;
};

struct s_link_connection
{
	s_link_connection();
	s_link *link_list;
	void *link;
	class c_class_938e0 *handler;
	s_connection_config const *config;
	long reliable_stream_index;
	long stream_index;
	c_base_450d40 own_client;
	bool sent_to_all_clients;   /* the last packet written included every client */
	bool read_by_all_clients;   /* the last packet read included every client */
	byte unknown1e[2];
	long handler_count;
	s_connection_handler handlers[3];
	s_connection_callback *callback;
	c_connection_owner *owner;
	long id;
	dword flags;
	long local_sequence;
	long remote_sequence;
	long state;
	long close_reason;
	s_type_99af70 previous_address;
	s_type_99af70 address;
	bool initiator;
	byte unknown85[3];
	long handshake_time;
	long handshake_next_time;
	long handshake_count;
	long establish_time;
	s_connection_timer timers[k_link_connection_timer_count];
};

typedef char assert_link_connection_size[(sizeof(s_link_connection) == 0xf8) ? 1 : -1];

struct s_network_stream_window
{
	long unknown00;
	long unknown04;
	long next;
	long end;
};

/* 0x4d87d8: the reliable streams (0x97c bytes), 0x4d87dc: the streams (0x2850 bytes) */
struct s_network_stream_header
{
	byte unknown00[4];
	bool active;
	byte unknown05[3];
	long owner;
	void *unknown0c;
	s_network_stream_window window;
};

/* src/unknown_081f80.cpp */
extern void *g_4d87d4;
extern void *g_4d87d8;
extern void *g_4d87dc;
extern long g_4d87d0;

bool link_remove_entry(s_link *link, long id);

/* not decompiled yet (src/stubs/session.cpp, src/stubs/lane_d.cpp) */
bool __stdcall function_07b140(void *x, long a, long ten, long twelve, void *local);
void __stdcall function_095580(void *stream, long message_type, long message_size, const void *message);

long network_reliable_stream_allocate(long owner);
long network_stream_allocate(long owner);
bool link_connection_flags_valid(dword flags);
void link_connection_close(s_link_connection *connection, long reason);
void link_connection_dispose(s_link_connection *connection);
void link_connection_reset_timers(s_link_connection *connection);
void link_connection_reset_timer(s_link_connection *connection, long index);
long link_connection_send_capacity(s_link_connection *connection);
void link_connection_update_handshake(s_link_connection *connection);
void link_connection_send_acknowledge(s_link_connection *connection, bool reliable);

static inline s_link_connection *link_connection_get(long index)
{
	return &((s_link_connection *)g_4d87d4)[index];
}

static inline s_network_stream_header *network_reliable_stream_get(long index)
{
	return (s_network_stream_header *)((byte *)g_4d87d8 + index * 0x97c);
}

static inline s_network_stream_header *network_stream_get(long index)
{
	return (s_network_stream_header *)((byte *)g_4d87dc + index * 0x2850);
}

#endif
