// @flags /O2 /GL- /GX
// Havok (third-party physics library): a creation function choosing between
// two kinds of bounded object by comparing a value reached through each of
// its first two arguments.
#include "bounded_2e9530.h"
#include "agent_table_2e5d50.h"

struct info_2ebaf0
{
	char unknown0[0x20];
	long field_20;
};

struct holder_2ebaf0
{
	char unknown0[0x10];
	info_2ebaf0 *info;
};

// the kind made when the first argument's value is the smaller
class class_412b08 : public bounded_2e9530
{
public:
	class_412b08(long value_) : bounded_2e9530(value_) {}
};

// the kind made otherwise
class class_412b28 : public bounded_2e9530
{
public:
	class_412b28(long value_) : bounded_2e9530(value_) {}
};

// @retail 0x2ebaf0
bounded_2e9530 *__cdecl function_2ebaf0(holder_2ebaf0 **arg_0, holder_2ebaf0 **arg_1, long arg_2, long value)
{
	holder_2ebaf0 *first_holder = *arg_0;
	holder_2ebaf0 *second_holder = *arg_1;
	info_2ebaf0 *second_info = second_holder->info;
	info_2ebaf0 *first_info = first_holder->info;
	// the values compared: each info's field_20 less 0x30
	long first = first_info->field_20 - 0x30;
	long second = second_info->field_20 - 0x30;
	if (first < second)
	{
		return new class_412b08(value);
	}
	return new class_412b28(value);
}

struct body_2ea940;
class bounded_2e9530;
class derived_2eb4e0;
struct holder_2ebaf0;
struct input_2ea940;
derived_2eb4e0 *__cdecl function_2eb4e0(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2eb5b0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2eb610(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value);
void __cdecl function_2eb7d0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
bounded_2e9530 *__cdecl function_2e9620(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2ead90(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);
void __cdecl function_2eb670(void const *a, void const *b, void const *input, void *collector, void *second_collector);
bounded_2e9530 *__cdecl function_2ebaf0(holder_2ebaf0 **arg_0, holder_2ebaf0 **arg_1, long arg_2, long value);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2ebbb0
void __cdecl function_2ebbb0(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2eb4e0;
	handlers.handler[1] = (handler_2e5d50)function_2eb5b0;
	handlers.handler[2] = (handler_2e5d50)function_2eb610;
	handlers.handler[3] = (handler_2e5d50)function_2eb7d0;
	table->function_2e5d50(&handlers, 19, -1);
	handlers.handler[0] = (handler_2e5d50)function_2e9620;
	handlers.handler[1] = (handler_2e5d50)function_2ead90;
	handlers.handler[2] = (handler_2e5d50)function_2ea940;
	handlers.handler[3] = (handler_2e5d50)function_2eb670;
	table->function_2e5d50(&handlers, -1, 19);
	handlers.handler[0] = (handler_2e5d50)function_2ebaf0;
	handlers.handler[1] = (handler_2e5d50)function_2ead90;
	handlers.handler[2] = (handler_2e5d50)function_2ea940;
	handlers.handler[3] = (handler_2e5d50)function_2eb670;
	table->function_2e5d50(&handlers, 19, 19);
}
