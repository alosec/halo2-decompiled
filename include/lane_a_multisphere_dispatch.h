#ifndef LANE_A_MULTISPHERE_DISPATCH_H
#define LANE_A_MULTISPHERE_DISPATCH_H
#include "lane_a_multisphere_query.h"

// Dispatcher offsets and 32-wide type indexing are observed in the retail
// calls. These are function tables, not additional object fields.
struct lane_a_dispatch;
typedef lane_a_child_agent *(__cdecl *lane_a_create_handler)(body_2ea940 const *, body_2ea940 const *, lane_a_dispatch *const *, void *);
typedef void (__cdecl *lane_a_query_handler)(body_2ea940 const *, body_2ea940 const *, lane_a_dispatch *const *, void *);
typedef void (__cdecl *lane_a_cast_handler)(body_2ea940 const *, body_2ea940 const *, lane_a_dispatch *const *, void *, void *);
struct lane_a_dispatch
{
	char prefix[0x18c];
	lane_a_create_handler create[32][32];
	lane_a_query_handler penetration[32][32];
	lane_a_query_handler closest[32][32];
	lane_a_cast_handler linear_cast[32][32];
};
typedef char lane_a_table_row_size[(sizeof(((lane_a_dispatch *)0)->closest[0]) == 128) ? 1 : -1];

void __cdecl function_2e80a0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
void __cdecl function_2e83c0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector, void *start_collector);
void __cdecl function_2e86e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
#endif
