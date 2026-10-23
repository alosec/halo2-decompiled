// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a kind of bounded object
// (which takes the value as its fourth argument).
#include "bounded_2e9530.h"
#include "agent_table_2e5d50.h"

class bounded_2fb970 : public bounded_2e9530
{
public:
	bounded_2fb970(long value_) : bounded_2e9530(value_) {}
};

// @retail 0x2fb970
bounded_2fb970 *__cdecl function_2fb970(long arg_0, long arg_1, long arg_2, long value)
{
	return new bounded_2fb970(value);
}

struct body_2ea940;
class bounded_2fb0f0;
class bounded_2fb970;
struct input_2ea940;
bounded_2fb0f0 *__cdecl function_2fb0f0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2fab70(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fabf0(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);
void __cdecl function_2f9790(void const *a, void const *b, void const *input, void *collector, void *second_collector);
bounded_2fb970 *__cdecl function_2fb970(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2fac70(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2facf0(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value);
void __cdecl function_2fad70(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2fb9d0
void __cdecl function_2fb9d0(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2fb0f0;
	handlers.handler[1] = (handler_2e5d50)function_2fab70;
	handlers.handler[2] = (handler_2e5d50)function_2fabf0;
	handlers.handler[3] = (handler_2e5d50)function_2f9790;
	table->function_2e5d50(&handlers, 4, 20);
	handlers.handler[0] = (handler_2e5d50)function_2fb970;
	handlers.handler[1] = (handler_2e5d50)function_2fac70;
	handlers.handler[2] = (handler_2e5d50)function_2facf0;
	handlers.handler[3] = (handler_2e5d50)function_2fad70;
	table->function_2e5d50(&handlers, 20, 4);
}
