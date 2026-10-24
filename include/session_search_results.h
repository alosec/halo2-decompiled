/* SESSION_SEARCH_RESULTS.H: one session found by a search (0x784 bytes), the
   element of the search lists of src/unknown_0b3570.cpp (system link replies
   and Live match results) and of the list src/session_search_results.cpp
   reads. The type keeps its old name, s_0b35e0_entry, because other files
   declare it under that name */

#ifndef SESSION_SEARCH_RESULTS_H
#define SESSION_SEARCH_RESULTS_H

#include "unknown_11c920.h"
#include "network_qos.h"
#include <xtl.h>
#include <stddef.h>

/* the part of a Live match search result 0xb3500 copies (0x68 bytes) */
struct s_online_match_session_info
{
	XNKEY key;
	XNKID session_id;
	XNADDR address;
	byte unknown3c[0x68 - 0x3c];
};
typedef char assert_online_match_session_info_size[(sizeof(s_online_match_session_info) == 0x68) ? 1 : -1];

struct s_0b35e0_entry
{
	bool used;
	byte unknown01[3];
	long time_found;           /* the tick count when it was found or last heard from */
	XNKID session_id;
	XNKEY key;
	XNADDR address;
	bool reply_received;       /* set on every system link reply */
	bool qos_valid;            /* the QoS probe of a Live result gave its result */
	byte unknown46[2];
	long qos_status;           /* the last status of the Live result's QoS probe */
	s_qos_result qos;          /* that result (its data is the session description) */
	bool description_changed;  /* set when a reply brings a different description */
	byte unknown6d[3];
	struct
	{
		byte unknown00[2];
		short kind;            /* 0 or 1 (0x7c110 checks it); only kind 0 entries are offered */
		byte unknown04[0x714 - 4];
	} description;             /* the session description of the reply (read by 0x7c110) */
};
typedef char assert_0b35e0_entry_size[(sizeof(s_0b35e0_entry) == 0x784) ? 1 : -1];
typedef char assert_0b35e0_entry_time_found[(offsetof(s_0b35e0_entry, time_found) == 4) ? 1 : -1];
typedef char assert_0b35e0_entry_session_id[(offsetof(s_0b35e0_entry, session_id) == 8) ? 1 : -1];
typedef char assert_0b35e0_entry_key[(offsetof(s_0b35e0_entry, key) == 0x10) ? 1 : -1];
typedef char assert_0b35e0_entry_address[(offsetof(s_0b35e0_entry, address) == 0x20) ? 1 : -1];
typedef char assert_0b35e0_entry_qos[(offsetof(s_0b35e0_entry, qos) == 0x4c) ? 1 : -1];
typedef char assert_0b35e0_entry_description[(offsetof(s_0b35e0_entry, description) == 0x70) ? 1 : -1];
typedef char assert_0b35e0_entry_kind[(offsetof(s_0b35e0_entry, description.kind) == 0x72) ? 1 : -1];

void session_search_result_initialize(s_online_match_session_info const *match, s_0b35e0_entry *entry);
s_0b35e0_entry *session_search_results_get(long index);

#endif
