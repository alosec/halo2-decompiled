// @flags /O2 /GL-
// Havok (third-party physics library): registering an agent's handlers for
// shape type 0x11 against any shape type, both ways round.
#include "lane_b_bv_agents.h"

// @retail 0x2e7d10
void __cdecl function_2e7d10(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2e7ca0;
	handlers.handler[1] = (handler_2e5d50)function_2e7850;
	handlers.handler[2] = (handler_2e5d50)function_2e78b0;
	handlers.handler[3] = (handler_2e5d50)function_2e7ab0;
	table->function_2e5d50(&handlers, -1, 0x11);
	handlers.handler[0] = (handler_2e5d50)function_2e6e30;
	handlers.handler[1] = (handler_2e5d50)function_2e7710;
	handlers.handler[2] = (handler_2e5d50)function_2e7510;
	handlers.handler[3] = (handler_2e5d50)function_2e71f0;
	table->function_2e5d50(&handlers, 0x11, -1);
}
