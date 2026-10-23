// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

struct body_2f23c0;
class collector_2f23c0;
class indexed_2f1b20;
class indexed_2f1b50;
indexed_2f1b50 *__cdecl function_2f1b50(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2f2580(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, long value);
void __cdecl function_2f2640(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2f26a0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
indexed_2f1b20 *__cdecl function_2f1b20(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2f23c0(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, collector_2f23c0 *collector);
void __cdecl function_2f2130(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2f2800
void __cdecl function_2f2800(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2f1b50;
	handlers.handler[1] = (handler_2e5d50)function_2f2580;
	handlers.handler[2] = (handler_2e5d50)function_2f2640;
	handlers.handler[3] = (handler_2e5d50)function_2f26a0;
	table->function_2e5d50(&handlers, 6, 4);
	handlers.handler[0] = (handler_2e5d50)function_2f1b20;
	handlers.handler[1] = (handler_2e5d50)function_2f23c0;
	handlers.handler[2] = (handler_2e5d50)function_2f2130;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 4, 6);
}
