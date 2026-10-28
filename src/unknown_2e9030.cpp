// @flags /O2 /GL-
// Havok (third-party physics library): registering the multisphere agent's
// handlers for a multisphere (type 9) against any shape type, both ways
// round; the flag the bounded agents read; and creating the agent for a pair
// of bodies through the dispatcher's table.
#include "agent_table_2e5d50.h"
#include "lane_b_multisphere_agents.h"
#include "pair_agents_2eab70.h"
#include "unknown_184250.h"

// @retail 0x2e9030
void __cdecl function_2e9030(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2e8fc0;
	handlers.handler[1] = (handler_2e5d50)function_2e8880;
	handlers.handler[2] = (handler_2e5d50)function_2e88e0;
	handlers.handler[3] = (handler_2e5d50)function_2e8940;
	table->function_2e5d50(&handlers, -1, 9);
	handlers.handler[0] = (handler_2e5d50)function_2e8e00;
	handlers.handler[1] = (handler_2e5d50)function_2e86e0;
	handlers.handler[2] = (handler_2e5d50)function_2e80a0;
	handlers.handler[3] = (handler_2e5d50)function_2e83c0;
	table->function_2e5d50(&handlers, 9, -1);
}

extern bool g_461f08;

// @retail 0x2e90a0
void __cdecl function_2e90a0(s_184251 arg_0)
{
	g_461f08 = arg_0.field_0;
}

// @retail 0x2e90f0
pair_agent_2ea410 *__cdecl function_2e90f0(void const *a, void const *b, void const *input, void *collector)
{
	lane_a_dispatch *table = *(lane_a_dispatch *const *)input;
	long first = ((typed_2ed170 *)((body_2ea940 const *)a)->shape)->type();
	long second = ((typed_2ed170 *)((body_2ea940 const *)b)->shape)->type();
	lane_a_create_handler handler = table->create[first][second];
	return (pair_agent_2ea410 *)handler((body_2ea940 const *)a, (body_2ea940 const *)b, (lane_a_dispatch *const *)input, collector);
}
