/* PENDING_MESSAGES.H: the pending messages (unknown_08e1e0.cpp) and the
   requests that queue them (unknown_080f30.cpp) */

#ifndef PENDING_MESSAGES_H
#define PENDING_MESSAGES_H

#include "unknown_11c920.h"

/* three values a request carries with it */
struct s_pending_message_values
{
	long value0;
	long value1;
	long value2;
};

/* a request to send or receive a message */
struct s_pending_message_header
{
	dword unknown00;
	word flags;
	word unknown06;
	short kind;
	short state;
	long unknown0c;
	long unknown10;
	s_pending_message_values values;
	long pending_index;
	void *data;
	long size;
};

struct s_pending_message
{
	s_pending_message_header *header;
	long task_index;
	long size;
	void *data;
	long unknown10;
};

extern s_pending_message g_4d8c28[32];

long pending_message_add(long size, s_pending_message_header *header, void *data);
long pending_message_add_received(s_pending_message_header *header, void *data, long size);

#endif