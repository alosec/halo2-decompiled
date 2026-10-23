#ifndef FILTER_STORAGE_H
#define FILTER_STORAGE_H
#include "unknown_11c920.h"
union s_transition_scalar
{
	real value;
	long bits;
};
struct s_transition_state
{
	union
	{
		long flags;
		struct { byte active; byte changed; word unknown02; };
	};
	long index;
	long unknown08;
	s_transition_scalar start;
	s_transition_scalar elapsed;
	s_transition_scalar output;
};
struct s_filter_storage { s_transition_state state; };
extern s_filter_storage g_filter_storage;
#define g_4670cc (g_filter_storage.state)
typedef char assert_filter_size[(sizeof(s_filter_storage) == 0x18) ? 1 : -1];
#define g_4670cd (g_filter_storage.state.changed)
#endif
