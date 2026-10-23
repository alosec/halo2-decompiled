/* UNKNOWN_1A58B0.H: the candidate lists of unknown_1a58b0.cpp (guessed
   layouts; only the fields used are named). The slot owner (s_actor_view), the
   handlers and the tag element are in slot_handler.h; the handler table
   g_46eeb8 is defined in actor_behavior.cpp. */

#ifndef UNKNOWN_1A58B0_H
#define UNKNOWN_1A58B0_H

#include "unknown_11c920.h"
#include "slot_handler.h"

/* a node of the list that candidate actions are linked into */
struct s_action_node
{
	short key;
	union
	{
		short flags;
		bool field_2;
	};
	short unknown4;
	byte unknown6[2];
	real unknown8;
	s_action_node *next;
	short order;
	byte unknown12[2];
};

struct s_candidate_entry
{
	short kind;
	short key;
	s_action_node node;
};

struct s_candidate_list
{
	short type;
	byte unknown02[2];
	s_candidate_entry *entries;
	short count;
	byte unknown0a[2];
};

/* three rows of one candidate entry per handler */
struct s_candidate_table
{
	s_candidate_entry entries[0x83];
};

/* five dwords: a set of 0x83 bits */
struct s_flag_bits
{
	dword d[5];
};

/* the sort settings (g_51e99c): flags at +0x14 */
struct s_sort_globals
{
	byte unknown00[0x14];
	dword flags;
};

#endif
