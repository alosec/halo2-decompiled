// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

struct description_2ed170;
struct dispatcher_2ed170;
class swapped_triangle_agent_2f42f0;
struct triangle_2f42f0;
class triangle_agent_2f42f0;
class typed_2ed170;
swapped_triangle_agent_2f42f0 *__cdecl function_2f43b0(triangle_2f42f0 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2f77f0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f78b0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f7910(void const *a, void const *b, void const *input, void *collector, void *second_collector);
triangle_agent_2f42f0 *__cdecl function_2f42f0(description_2ed170 const *description, triangle_2f42f0 const **other, dispatcher_2ed170 **dispatcher, long arg_3);
void __cdecl function_2f6db0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f5960(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2f7a70
void __cdecl function_2f7a70(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f43b0;
	handlers.handler[1] = (handler_2e5d50)function_2f77f0;
	handlers.handler[2] = (handler_2e5d50)function_2f78b0;
	handlers.handler[3] = (handler_2e5d50)function_2f7910;
	table->function_2e5d50(&handlers, 5, 7);
	handlers.handler[0] = (handler_2e5d50)function_2f42f0;
	handlers.handler[1] = (handler_2e5d50)function_2f6db0;
	handlers.handler[2] = (handler_2e5d50)function_2f5960;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 7, 5);
}
