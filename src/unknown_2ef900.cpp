// @flags /O2 /GL-
// Havok (third-party physics library): registering the handlers for pairs of
// shape types in the handler table.
#include "agent_table_2e5d50.h"

void *__cdecl function_2ef2f0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2ef7f0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ef5d0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2ef900
void __cdecl function_2ef900(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2ef2f0;
	handlers.handler[1] = (handler_2e5d50)function_2ef7f0;
	handlers.handler[2] = (handler_2e5d50)function_2ef5d0;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 6, 6);
}
