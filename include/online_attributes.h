/* ONLINE_ATTRIBUTES.H: the views of XONLINE_ATTRIBUTE and of their inputs
   that the attribute builders of src/online_match_attributes.cpp (0xb4a90,
   0xb4b80) fill, shared with their callers in src/online_matchmaking.cpp */

#ifndef ONLINE_ATTRIBUTES_H
#define ONLINE_ATTRIBUTES_H

#include "unknown_11c920.h"

struct _XONLINE_ATTRIBUTE_SPEC;
extern _XONLINE_ATTRIBUTE_SPEC const g_44050c[7];

/* a search parameter: an XONLINE_ATTRIBUTE holding an integer, or the null
   type for a parameter left out */
struct s_entry_pair
{
	long attribute_id;  /* X_ATTRIBUTE_DATATYPE_INTEGER or X_ATTRIBUTE_DATATYPE_NULL */
	long changed;
	__int64 value;
};
typedef char assert_entry_pair_size[(sizeof(s_entry_pair) == 0x10) ? 1 : -1];

struct s_range_input
{
	long x;
	long y;
	bool has_min;
	long min;
	bool has_max;
	long max;
	bool has_count;
	long count;
};

/* a session attribute: an XONLINE_ATTRIBUTE holding an integer */
struct s_property_entry
{
	long attribute_id;
	long changed;
	__int64 value;
};
typedef char assert_property_entry_size[(sizeof(s_property_entry) == 0x10) ? 1 : -1];

struct s_property_input
{
	long v0;
	long v1;
	long v2;
	long v3;
	long v4;
	long v5;
	dword flags;
};

#endif
