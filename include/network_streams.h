/* NETWORK_STREAMS.H: the two kinds of message stream a connection owns
   (src/network_streams.cpp): the unreliable stream (0x2850 bytes, vtable
   0x450db8) and the reliable stream (0x97c bytes, vtable 0x450dd8) */

#ifndef NETWORK_STREAMS_H
#define NETWORK_STREAMS_H

#include "unknown_11c920.h"
#include <stddef.h>

struct s_bitstream;

/* a window over a range of sequence numbers: the messages oldest+1..newest,
   kept in a ring buffer from head */
struct s_sequence_window
{
	bool valid;
	byte unknown01[3];
	long capacity;
	long newest;
	long oldest;
	long size;
	long head;
	long count;
};

struct s_stream_message
{
	byte flags;
	byte size;
	word unknown02;
	void *data;
	long unknown08;
};

struct s_stream_fragment
{
	byte flags;
	byte size;
	word unknown02;
	void *data;
};

struct s_reliable_message
{
	long time;
	long size;
	long unknown08;
	word distance;
	word flags;
};

class c_network_stream
{
public:
	virtual long v0() { return 0; }
	virtual bool v1(long *reason) { return false; }
	virtual bool v2(bool *pending) { return false; }
	virtual long v3(long a, long b) { return 0; }
	virtual bool v4(long identifier, s_bitstream *stream, long unused, long reserved_bits) { return false; }
	virtual long v5(long const *identifier, s_bitstream *stream) { return 0; }
	virtual void v6() {}
	virtual void v7(long identifier, bool delivered) {}
};

class c_network_unreliable_stream : public c_network_stream
{
public:
	virtual bool v1(long *reason);
	virtual bool v2(bool *pending);
	virtual long v3(long a, long b);
	virtual void v7(long identifier, bool delivered);
	virtual long v5(long const *identifier, s_bitstream *stream);
	virtual bool v4(long identifier, s_bitstream *stream, long unused, long reserved_bits);

	bool m_active;
	bool m_unknown05;
	long m_owner;
	void *m_unknown0c;
	s_sequence_window m_message_window;
	s_stream_message m_messages[512];
	s_sequence_window m_fragment_window;
	s_stream_fragment m_fragments[512];
	long m_message_bytes;
	long m_fragment_bytes;
};

class c_network_reliable_stream : public c_network_stream
{
public:
	virtual bool v4(long arg_0, s_bitstream *arg_1, long arg_2, long arg_3);
	virtual bool v1(long *reason);
	virtual bool v2(bool *pending);
	virtual long v3(long a, long b);
	virtual long v5(long const *identifier, s_bitstream *stream);
	void advance_acknowledgements();
	bool get_next_send(long *type, long *sequence, long *size, long *time);
	long function_965e0(long *sequence, long *size, long *time);
	long allocate_sequence(long time);
	long read_acknowledgement(long *message_sequence, long sequence, bool valid, long distance);
	void mark_received(long sequence);
	void update_round_trip(long type, long round_trip_time, long sequence, long time);
	long function_96910(long sequence, long bit_count, dword const *bits, bool complete);

	bool m_active;
	bool m_unknown05;
	long m_owner;
	s_sequence_window m_acknowledgement_window;
	s_sequence_window m_message_window;
	long m_next_sequence;
	long m_bytes;
	word m_acknowledgements[0x80];
	s_reliable_message m_messages[0x80];
	long m_last_sequence;
	long m_unknown950;
	long m_unknown954;
	bool m_unknown958;
	bool m_unknown959;
	long m_unknown95c;
	long m_unknown960;
	long m_unknown964;
	long m_round_trip_minimum;
	long m_round_trip_average;
	long m_round_trip_deviation;
	long m_timeout;
	long m_backoff;
};

typedef char assert_sequence_window_size[(sizeof(s_sequence_window) == 0x1c) ? 1 : -1];
typedef char assert_network_unreliable_stream_size[(sizeof(c_network_unreliable_stream) == 0x2850) ? 1 : -1];
typedef char assert_network_reliable_stream_size[(sizeof(c_network_reliable_stream) == 0x97c) ? 1 : -1];
typedef char assert_network_reliable_stream_layout[(
	offsetof(c_network_reliable_stream, m_message_window) == 0x28 &&
	offsetof(c_network_reliable_stream, m_next_sequence) == 0x44 &&
	offsetof(c_network_reliable_stream, m_bytes) == 0x48 &&
	offsetof(c_network_reliable_stream, m_last_sequence) == 0x94c &&
	offsetof(c_network_reliable_stream, m_round_trip_minimum) == 0x968) ? 1 : -1];

#endif
